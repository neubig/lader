#include <lader/hyper-graph.h>
#include <lader/util.h>
#include <tr1/unordered_map>
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace lader;
using namespace std;
using namespace boost;

// Return the edge feature vector
// if -save_features, edge features are stored in a stack.
// thus, this is thread safe without lock.
const FeatureVectorInt * HyperGraph::GetEdgeFeatures(
                                ReordererModel & model,
                                const FeatureSet & feature_gen,
                                const Sentence & sent,
                                const HyperEdge & edge) {
	FeatureVectorInt * ret = NULL;
//    if (save_features_) {
    	SpanStack * stack = GetStack(edge.GetLeft(), edge.GetRight());
    	switch (edge.GetType()){
    	case HyperEdge::EDGE_FOR:
    	case HyperEdge::EDGE_STR:
    		ret = stack->GetStraightFeautures(edge.GetCenter() - edge.GetLeft());
    		if (ret == NULL){
    			ret = feature_gen.MakeEdgeFeatures(sent, edge, model.GetFeatureIds(), model.GetAdd());
    			stack->SaveStraightFeautures(edge.GetCenter() - edge.GetLeft(), ret);
    		}
    		break;
    	case HyperEdge::EDGE_BAC:
    	case HyperEdge::EDGE_INV:
    		ret = stack->GetInvertedFeautures(edge.GetCenter() - edge.GetLeft());
    		if (ret == NULL){
    			ret = feature_gen.MakeEdgeFeatures(sent, edge, model.GetFeatureIds(), model.GetAdd());
    			stack->SaveInvertedFeautures(edge.GetCenter() - edge.GetLeft(), ret);
    		}
    		break;
    	case HyperEdge::EDGE_ROOT:
    	default:
    		THROW_ERROR("Invalid hyper edge for GetEdgeFeatures: " << edge);
    		break;
    	}
//    } else {
//    	ret = feature_gen.MakeEdgeFeatures(sent, edge, model.GetFeatureIds(), model.GetAdd());
//    }
    return ret;
}

// Score a span
double HyperGraph::Score(double loss_multiplier, const Hypothesis* hyp) const {
    double score = hyp->GetLoss()*loss_multiplier;
    if(hyp->GetEdgeType() != HyperEdge::EDGE_ROOT) {
    	score += hyp->GetSingleScore();
    }
    if(hyp->GetLeftChild())
		score += Score(loss_multiplier, hyp->GetLeftHyp());
	if(hyp->GetRightChild())
		score += Score(loss_multiplier, hyp->GetRightHyp());
	return score;
}

double HyperGraph::Rescore(double loss_multiplier) {
    // Score all root hypotheses, and place the best hyp at hyps[0].
	// Note that it does not modify the rest of hypotheses under the root.
    // Therefore, this keep the forest structure by BuildHyperGraph
	BOOST_FOREACH(TargetSpan * trg, GetRootStack()->GetSpans()){
		std::vector<Hypothesis*> & hyps = trg->GetHypotheses();
		for(int i = 0; i < (int)hyps.size(); i++) {
			hyps[i]->SetScore(Score(loss_multiplier, hyps[i]));
			if(hyps[i]->GetScore() > hyps[0]->GetScore())
				swap(hyps[i], hyps[0]);
		}
	}
	// Sort to make sure that the spans are all in the right order
	BOOST_FOREACH(SpanStack * stack, stacks_)
		sort(stack->GetSpans().begin(), stack->GetSpans().end(),
										DescendingScore<TargetSpan>());
	TargetSpan* best = (*stacks_.rbegin())->GetSpanOfRank(0);
	return best->GetScore();
}

// Score a span
double HyperGraph::Score(const ReordererModel & model,
                       double loss_multiplier,
                       TargetSpan* span) {
    if(span->GetScore() == -DBL_MAX) {
        std::vector<Hypothesis*> & hyps = span->GetHypotheses();
        for(int i = 0; i < (int)hyps.size(); i++) {
            Score(model, loss_multiplier, hyps[i]);
            if(hyps[i]->GetScore() > hyps[0]->GetScore())
                swap(hyps[i], hyps[0]);
        }
    }
    return span->GetScore();
}

// Score a hypothesis
double HyperGraph::Score(const ReordererModel & model,
                         double loss_multiplier,
                         Hypothesis* hyp) {
    double score = hyp->GetScore();
    if(score == -DBL_MAX) {
        score = hyp->GetLoss()*loss_multiplier;
        int l = hyp->GetLeft(), c = hyp->GetCenter(), r = hyp->GetRight();
        HyperEdge::Type t = hyp->GetEdgeType();
        if(t != HyperEdge::EDGE_ROOT) {
        	SpanStack * stack = GetStack(hyp->GetLeft(), hyp->GetRight());
        	FeatureVectorInt * fvi = NULL;
        	switch(hyp->GetEdgeType()){
        	case HyperEdge::EDGE_FOR:
        	case HyperEdge::EDGE_STR:
        		fvi = stack->GetStraightFeautures(hyp->GetCenter() - hyp->GetLeft());
        		break;
        	case HyperEdge::EDGE_BAC:
        	case HyperEdge::EDGE_INV:
        		fvi = stack->GetInvertedFeautures(hyp->GetCenter() - hyp->GetLeft());
        		break;
        	}
            if(!fvi)
                THROW_ERROR("No features found in Score for " << *hyp << endl);
            score += model.ScoreFeatureVector(*fvi);
        }
        hyp->SetSingleScore(score);
        if(hyp->GetLeftChild())
            score += Score(model, loss_multiplier, hyp->GetLeftChild());
        if(hyp->GetRightChild())
            score += Score(model, loss_multiplier, hyp->GetRightChild());
        hyp->SetScore(score);
    }
    return score;
}

double HyperGraph::Rescore(const ReordererModel & model, double loss_multiplier) {
    // Reset everything to -DBL_MAX to indicate it needs to be recalculated
    BOOST_FOREACH(SpanStack * stack, stacks_)
        BOOST_FOREACH(TargetSpan * trg, stack->GetSpans())
            BOOST_FOREACH(Hypothesis * hyp, trg->GetHypotheses())
                hyp->SetScore(-DBL_MAX);
    // Recursively score all edges from the root
    BOOST_FOREACH(TargetSpan * trg, (*stacks_.rbegin())->GetSpans())
        Score(model, loss_multiplier, trg);
    // Sort to make sure that the spans are all in the right order
    BOOST_FOREACH(SpanStack * stack, stacks_)
        sort(stack->GetSpans().begin(), stack->GetSpans().end(),
                                        DescendingScore<TargetSpan>());
    TargetSpan* best = (*stacks_.rbegin())->GetSpanOfRank(0);
    return best->GetScore();
}

// Get the score for a single edge
double HyperGraph::GetEdgeScore(ReordererModel & model,
                                const FeatureSet & feature_gen,
                                const Sentence & sent,
                                const HyperEdge & edge) {
    const FeatureVectorInt * vec = 
                GetEdgeFeatures(model, feature_gen, sent, edge);
    double score = model.ScoreFeatureVector(SafeReference(vec));
//    // features are not stored, thus delete
//    if (!save_features_)
//    	delete vec;
    return score;
}

// If the length is OK, add a terminal
void HyperGraph::AddTerminals(int l, int r, const FeatureSet & features,
		ReordererModel & model, const Sentence & sent, HypothesisQueue * q,
		bool save_trg)
{
    // If the length is OK, add a terminal
    if((model.GetMaxTerm() == 0) || (r-l < model.GetMaxTerm())){
        int tl = (save_trg ? l : -1);
        int tr = (save_trg ? r : -1);
        double score;
        // Create a hypothesis with the forward terminal
        HyperEdge *edge = new HyperEdge(l, -1, r, HyperEdge::EDGE_FOR);
        score = GetEdgeScore(model, features, sent, *edge);
        q->push(new Hypothesis(score, score, edge, tl, tr));
        if(model.GetUseReverse()){
            edge = new HyperEdge(l, -1, r, HyperEdge::EDGE_BAC);
            score = GetEdgeScore(model, features, sent, *edge);
            q->push(new Hypothesis(score, score, edge, tr, tl));
        }
    }
}


// Increment the left side if there is still a hypothesis left
void HyperGraph::IncrementLeft(const Hypothesis *old_hyp, TargetSpan *new_left,
		HypothesisQueue & q, int & pop_count)
{
   Hypothesis * old_left  = old_hyp->GetLeftHyp();
   Hypothesis * old_right = old_hyp->GetRightHyp();
   if (!old_left || !old_right)
	   THROW_ERROR("Fail to IncrementLeft for " << *old_hyp << endl)
	// Clone this hypothesis
	Hypothesis * new_hyp = old_hyp->Clone();
	new_hyp->SetScore(old_hyp->GetScore()
			- old_left->GetScore() + new_left->GetScore());
	new_hyp->SetLeftRank(old_hyp->GetLeftRank()+1);
	new_hyp->SetLeftChild(new_left);
	if(new_hyp->GetEdgeType() == HyperEdge::EDGE_STR)
		new_hyp->SetTrgLeft(new_left->GetTrgLeft());
	else
		new_hyp->SetTrgRight(new_left->GetTrgRight());
	if (new_hyp->GetTrgLeft() < new_hyp->GetLeft()
	|| new_hyp->GetTrgRight() < new_hyp->GetLeft()
	|| new_hyp->GetRight() < new_hyp->GetTrgLeft()
	|| new_hyp->GetRight() < new_hyp->GetTrgRight()) {
		new_hyp->PrintParse(cerr);
		THROW_ERROR("Malformed hypothesis in IncrementLeft " << *new_hyp << endl)
	}
	q.push(new_hyp);
}

// Increment the right side if there is still a hypothesis right
void HyperGraph::IncrementRight(const Hypothesis *old_hyp, TargetSpan *new_right,
		HypothesisQueue & q, int & pop_count)
{
	Hypothesis * old_left  = old_hyp->GetLeftHyp();
	Hypothesis * old_right = old_hyp->GetRightHyp();
	if (!old_left || !old_right)
		THROW_ERROR("Fail to IncrementRight for " << *old_hyp << endl)
	// Clone this hypothesis
	Hypothesis * new_hyp = old_hyp->Clone();
	double non_local_score = 0.0;
	new_hyp->SetScore(old_hyp->GetScore()
			- old_right->GetScore() + new_right->GetScore());
	new_hyp->SetRightRank(old_hyp->GetRightRank()+1);
	new_hyp->SetRightChild(new_right);
	if(new_hyp->GetEdgeType() == HyperEdge::EDGE_STR)
		new_hyp->SetTrgRight(new_right->GetTrgRight());
	else
		new_hyp->SetTrgLeft(new_right->GetTrgLeft());
	if (new_hyp->GetTrgLeft() < new_hyp->GetLeft()
	|| new_hyp->GetTrgRight() < new_hyp->GetLeft()
	|| new_hyp->GetRight() < new_hyp->GetTrgLeft()
	|| new_hyp->GetRight() < new_hyp->GetTrgRight()) {
		new_hyp->PrintParse(cerr);
		THROW_ERROR("Malformed hypothesis in IncrementRight " << *new_hyp << endl)
	}
	q.push(new_hyp);
}

// For cube growing
void HyperGraph::LazyNext(HypothesisQueue & q, const Hypothesis * hyp,
		int & pop_count) {
	TargetSpan * new_left = NULL, *new_right = NULL;

	// Skip terminals
	if(hyp->GetCenter() == -1) return;
	RUN(cerr << "LazyNext from " << *hyp << endl);
	RUN(cerr << "Before increment: " << q.size() << " candidates" << endl);
	SpanStack *left_stack = GetStack(hyp->GetLeft(), hyp->GetCenter()-1);
	new_left = LazyKthBest(left_stack, hyp->GetLeftRank() + 1, pop_count);
	if (new_left)
		IncrementLeft(hyp, new_left, q, pop_count);

	SpanStack *right_span = GetStack(hyp->GetCenter(), hyp->GetRight());
	new_right = LazyKthBest(right_span, hyp->GetRightRank() + 1, pop_count);
	if (new_right)
		IncrementRight(hyp, new_right, q, pop_count);
	RUN(cerr << "After increment: " << q.size() << " candidates" << endl);
}

// For cube growing
TargetSpan * HyperGraph::LazyKthBest(SpanStack * stack, int k, int & pop_count) {
	TargetSpan * trg_span = NULL;
	while (((!beam_size_ && stack->size() < k + 1)
			|| stack->size() < std::min(k + 1, beam_size_))
			&& stack->CandSize() > 0) {
		HypothesisQueue * q = &stack->GetCands();
		Hypothesis * hyp = q->top(); q->pop();
		trg_span = stack->GetTargetSpan(hyp);
		// Insert the hypothesis
//		RUN(cerr << "Insert " << k << "th hypothesis " << *hyp);
//		RUN(hyp->PrintChildren(cerr));
		trg_span->AddHypothesis(hyp);
		LazyNext(*q, hyp, pop_count);
        // If the next hypothesis on the stack is equal to the current
        // hypothesis, remove it, as this just means that we added the same
        // hypothesis
        while(q->size() && q->top() == hyp) {
        	delete q->top();
        	q->pop();
        }
		if (pop_limit_ && ++pop_count > pop_limit_)
			break;
	}
	return stack->GetSpanOfRank(k);
}

// Build a hypergraph using beam search and cube pruning
void HyperGraph::ProcessOneSpan(ReordererModel & model,
                                       const FeatureSet & features,
                                       const Sentence & source,
                                       int l, int r,
                                       int beam_size, bool save_trg) {
    // Get a map to store identical target spans
    SpanStack * ret = GetStack(l, r);
    if (!ret)
    	THROW_ERROR("SetStack is required: [" << l << ", " << r << "]")
	ret->Clear();
    HypothesisQueue * q;
	if (cube_growing_)
		q = &ret->GetCands();
    else
        // Create the temporary data members for this span
    	q = new HypothesisQueue;

    double score, viterbi_score;
    // If the length is OK, add a terminal
    AddTerminals(l, r, features, model, source, q, save_trg);

    TargetSpan *left_trg, *right_trg;
    Hypothesis *new_left_hyp, *old_left_hyp,
               *new_right_hyp, *old_right_hyp;
    // Add the best hypotheses for each non-terminal to the queue
    for(int c = l+1; c <= r; c++) {
        // Find the best hypotheses on the left and right side
        left_trg = GetTrgSpan(l, c-1, 0);
        right_trg = GetTrgSpan(c, r, 0);
        if(left_trg == NULL) THROW_ERROR("Target l="<<l<<", c-1="<<c-1);
        if(right_trg == NULL) THROW_ERROR("Target c="<<c<<", r="<<r);
        Hypothesis * left, *right;
       	left = left_trg->GetHypothesis(0);
       	right = right_trg->GetHypothesis(0);
        // Add the straight terminal
        HyperEdge * edge = new HyperEdge(l, c, r, HyperEdge::EDGE_STR);
        score = GetEdgeScore(model, features, source, *edge);
    	viterbi_score = score + left->GetScore() + right->GetScore();
    	q->push(new Hypothesis(viterbi_score, score, edge,
                         left->GetTrgLeft(),
                         right->GetTrgRight(),
                         0, 0, left_trg, right_trg));
        // Add the inverted terminal
        edge = new HyperEdge(l, c, r, HyperEdge::EDGE_INV);
        score = GetEdgeScore(model, features, source, *edge);
    	viterbi_score = score + left->GetScore() + right->GetScore();
    	q->push(new Hypothesis(viterbi_score, score, edge,
						 right->GetTrgLeft(),
						 left->GetTrgRight(),
                         0, 0, left_trg, right_trg));

    }
    // For cube growing, search is lazy
    if (cube_growing_)
    	return;

    TargetSpan * trg_span = NULL;
    // Start beam search
    int num_processed = 0;
    int pop_count = 0;
    TargetSpan *new_left_trg, *new_right_trg;
    while((!beam_size || num_processed < beam_size) && q->size()) {
        // Pop a hypothesis from the stack and get its target span
        Hypothesis * hyp = q->top(); q->pop();
        trg_span = ret->GetTargetSpan(hyp);

        // Insert the hypothesis
//		RUN(cerr << "Insert " << ret->HypSize() << "th hypothesis " << *hyp);
//		RUN(hyp->PrintChildren(cerr));
        trg_span->AddHypothesis(hyp);
        num_processed++;
        // If the next hypothesis on the stack is equal to the current
        // hypothesis, remove it, as this just means that we added the same
        // hypothesis
        while(q->size() && q->top() == hyp) {
        	delete q->top();
        	q->pop();
        }
        // Skip terminals
        if(hyp->GetCenter() == -1) continue;

    	Hypothesis * new_left = NULL, *new_right = NULL;

        // Increment the left side if there is still a hypothesis left
        new_left_trg = GetTrgSpan(l, hyp->GetCenter()-1, hyp->GetLeftRank()+1);
        if (new_left_trg)
        	IncrementLeft(hyp, new_left_trg, *q, pop_count);
        // Increment the right side if there is still a hypothesis right
        new_right_trg = GetTrgSpan(hyp->GetCenter(),r,hyp->GetRightRank()+1);
        if(new_right_trg)
        	IncrementRight(hyp, new_right_trg, *q, pop_count);
    }
    while(!q->empty()){
        delete q->top();
        q->pop();
    }
    delete q;
}

// for cube growing, trigger the best hypothesis
void HyperGraph::Trigger(int l, int r) {
	int pop_count = 0;
	RUN(cerr << "Trigger the best hypothesis " << *HyperGraph::GetStack(l, r) << endl);
	TargetSpan * best = LazyKthBest(GetStack(l, r), 0, pop_count);
	if (!best)
		THROW_ERROR("Fail to produce hypotheses " << *GetStack(l, r) << endl);
}

// Build a hypergraph using beam search and cube pruning/growing
void HyperGraph::BuildHyperGraph(ReordererModel & model,
        const FeatureSet & features,
        const Sentence & source, bool save_trg) {
    SetNumWords(source[0]->GetNumWords());
    // if -save_features=true, reuse the stacks storing edge features
    if (!save_features_)
    	SetAllStacks();
    // Iterate through the left side of the span
    for(int L = 1; L <= n_; L++) {
        // Move the span from l to r, building hypotheses from small to large
    	for(int l = 0; l <= n_-L; l++){
    		int r = l+L-1;
    		ProcessOneSpan(model, features,
    		    						source, l, r, beam_size_, save_trg);
    	}
    	// for cube growing, trigger the best hypothesis
    	if (cube_growing_)
    		for(int l = 0; l <= n_-L; l++)
    			Trigger(l, l+L-1);
    }
    // Build the root node
    SpanStack * top = GetStack(0,n_-1);
    SpanStack * root_stack = GetStack(0,n_);
    if (!root_stack)
    	THROW_ERROR("Either -save_features=false or InitStacks first")
    root_stack->Clear();
    for(int i = 0; !beam_size_ || i < beam_size_; i++){
    	TargetSpan * best = NULL;
    	if (cube_growing_){
    		int pop_count = 0;
    		best = LazyKthBest(top, i, pop_count);
    	}
    	else if (i < (int)(top->size()))
            best = (*top)[i];

		if(!best)
			break;

        // each hypothesis in a different target span
        TargetSpan * root = new TargetSpan(0, n_-1, best->GetTrgLeft(), best->GetTrgRight());
        root->AddHypothesis(new Hypothesis(
        		best->GetScore() , 0,
        		new HyperEdge(0, -1, n_-1, HyperEdge::EDGE_ROOT),
        		best->GetTrgLeft(), best->GetTrgRight(),
                i, -1,
                best, NULL));
        root_stack->AddSpan(root);
    }
}

// Add up the loss over an entire subtree defined by span
double HyperGraph::AccumulateLoss(const TargetSpan* span) {
    const Hypothesis * hyp = span->GetHypothesis(0);
    double score = hyp->GetLoss();
    if(hyp->GetLeftChild())  score += AccumulateLoss(hyp->GetLeftChild());
    if(hyp->GetRightChild())  score += AccumulateLoss(hyp->GetRightChild());
    return score;
}

void HyperGraph::AddLoss(LossBase* loss,
		const Ranks * ranks, const FeatureDataParse * parse) {
    // Initialize the loss
    loss->Initialize(ranks, parse);
    // For each span in the hypergraph
    int n = n_;
    for(int r = 0; r <= n; r++) {
        // When r == n, we want the root, so only do -1
        for(int l = (r == n ? -1 : 0); l <= (r == n ? -1 : r); l++) {
            // DEBUG cerr << "l=" << l << ", r=" << r << ", n=" << n << endl;
        	BOOST_FOREACH(TargetSpan* span, GetStack(l,r)->GetSpans()) {
        		BOOST_FOREACH(Hypothesis* hyp, span->GetHypotheses())  {
        			// DEBUG cerr << "GetLoss = " <<hyp->GetLoss()<<endl;
        			hyp->SetLoss(hyp->GetLoss() +
        					loss->AddLossToProduction(hyp, ranks, parse));
        		}
            }
        }
    }
}


// Accumulate edge features under a hyper-edge
void HyperGraph::AccumulateFeatures(std::tr1::unordered_map<int,double> & feat_map,
										ReordererModel & model,
		                                const FeatureSet & features,
		                                const Sentence & sent,
		                                const TargetSpan * span){
	const Hypothesis * hyp = span->GetHypothesis(0);
	// root has no edge feature
	if (hyp->GetEdgeType() != HyperEdge::EDGE_ROOT){
		// accumulate edge features
		const FeatureVectorInt * fvi = GetEdgeFeatures(model, features, sent, *hyp->GetEdge());
		BOOST_FOREACH(FeaturePairInt feat_pair, *fvi)
			feat_map[feat_pair.first] += feat_pair.second;
//		// if fvi is not stored, delete
//		if (!save_features_)
//			delete fvi;
	}
    if(hyp->GetLeftChild()) AccumulateFeatures(feat_map, model, features, sent, hyp->GetLeftChild());
    if(hyp->GetRightChild())AccumulateFeatures(feat_map, model, features, sent, hyp->GetRightChild());
}


template <class T>
inline string GetNodeString(char type, const T * hyp) {
    if(!hyp) return "";
    ostringstream oss;
    oss << type << "-" << hyp->GetLeft() << "-" << hyp->GetRight();
    return oss.str(); 
}

// Print the whole hypergraph
void HyperGraph::PrintHyperGraph(const std::vector<std::string> & strs,
                                 std::ostream & out) {
    SymbolSet<int> nodes, rules;
    vector<vector<string> > node_strings;
    // Reset the node IDs
    BOOST_FOREACH(SpanStack * stack, stacks_)
        BOOST_FOREACH(TargetSpan * span, stack->GetSpans())
            span->ResetId();
    // Add the node IDs
    int last_id = 0;
    GetRoot()->LabelWithIds(last_id); 
    // For each ending point of a span
    set<char> null_set; null_set.insert(0);
    for(int j = 0; j <= (int)strs.size(); j++) {
        // For each starting point of a span
        bool root = j==(int)strs.size();
        for(int i = (root?-1:j); i >= (root?-1:0); i--) {
            SpanStack * stack = GetStack(i, j);
            // For each target span
            BOOST_FOREACH(TargetSpan * span, stack->GetSpans()) {
                if(span->GetId() == -1)
                    continue;
                // For each hypothesis
                BOOST_FOREACH(const Hypothesis * hyp, span->GetHypotheses()) {
                    span->SetHasType(hyp->GetEdgeType());
                    int top_id = nodes.GetId(GetNodeString(hyp->GetEdgeType(), hyp),true);
                    if((int)node_strings.size() <= top_id)
                        node_strings.resize(top_id+1);
                    TargetSpan *left_child = hyp->GetLeftChild();
                    TargetSpan *right_child = hyp->GetRightChild();
                    // For each type in the left
                    BOOST_FOREACH(char left_type, 
                                  left_child ? left_child->GetHasTypes() : null_set) {
                        int left_id = nodes.GetId(GetNodeString(left_type, left_child));
                        // For each type in the right
                        BOOST_FOREACH(char right_type, 
                                      right_child ? right_child->GetHasTypes() : null_set) {
                            int right_id = nodes.GetId(GetNodeString(right_type, right_child));
                            int rule_id = 1 + rules.GetId(hyp->GetRuleString(strs, left_type, right_type), true);
                            ostringstream rule_oss;
                            rule_oss << "{";
                            if(left_id != -1) {
                                rule_oss << "\"tail\":[" << left_id;
                                if(right_id != -1)
                                    rule_oss << "," << right_id;
                                rule_oss<<"],";
                            }
                            rule_oss << "\"feature\":{\"parser\":" << hyp->GetSingleScore()<<"},"
                                    << "\"rule\":" << rule_id << "}";
                            node_strings[top_id].push_back(rule_oss.str());
                        }
                    }
                }
            }
            // We only need one time for the root
            if(i == -1) break;
        }
    }
    const vector<string*> & rule_vocab = rules.GetSymbols();
    out << "{\"rules\": [";
    for(int i = 0; i < (int)rule_vocab.size(); i++) {
        if(i != 0) out << ", ";
        out << "\"" << *rule_vocab[i] << "\"";
    }
    out << "], \"nodes\": [";
    for(int i = 0; i < (int)node_strings.size(); i++) {
        if(i != 0) out << ", ";
        out << "[" << algorithm::join(node_strings[i], ", ") << "]";
    }
    out << "], \"goal\": " << node_strings.size()-1 << "}";
    
}
