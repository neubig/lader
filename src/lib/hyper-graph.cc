#include <lader/hyper-edge.h>
#include <lader/hyper-graph.h>
#include <lader/hypothesis-queue.h>
#include <lader/feature-vector.h>
#include <lader/feature-set.h>
#include <lader/reorderer-model.h>
#include <lader/target-span.h>
#include <lader/util.h>
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace lader;
using namespace std;
using namespace boost;

template <class T>
struct DescendingScore {
  bool operator ()(T *lhs, T *rhs) { return rhs->GetScore() < lhs->GetScore(); }
};

// Return the edge feature vector
const FeatureVectorInt * HyperGraph::GetEdgeFeatures(
                                ReordererModel & model,
                                const FeatureSet & feature_gen,
                                const Sentence & sent,
                                const HyperEdge & edge) {
    FeatureVectorInt * ret;
    if(features_ == NULL) features_ = new EdgeFeatureMap;
    EdgeFeatureMap::const_iterator it = features_->find(edge);
    if(it == features_->end()) {
        FeatureVectorString * str = feature_gen.MakeEdgeFeatures(sent, edge);
        ret = model.IndexFeatureVector(*str);
        delete str;
        features_->insert(MakePair(edge, ret));
    } else {
        ret = it->second;
    }
    return ret;
}

// Score a span
double HyperGraph::Score(const ReordererModel & model,
                         double loss_multiplier,
                         TargetSpan* span) {
    double max_score = span->GetScore();
    if(max_score == -DBL_MAX) {
        std::vector<Hypothesis*> & hyps = span->GetHypotheses();
        BOOST_FOREACH(Hypothesis *hyp, hyps)
            max_score = max(max_score, Score(model, loss_multiplier, hyp));
        sort(hyps.begin(), hyps.end(), DescendingScore<Hypothesis>());
    }
    return max_score;
}

// Score a hypothesis
double HyperGraph::Score(const ReordererModel & model,
                         double loss_multiplier,
                         Hypothesis* hyp) {
    double score = hyp->GetScore();
    if(score == -DBL_MAX) { 
        score = hyp->GetLoss()*loss_multiplier;
        int l = hyp->GetLeft(), c = hyp->GetCenter(), r = hyp->GetRight();
        HyperEdge::Type t = hyp->GetType();
        if(t != HyperEdge::EDGE_ROOT) {
            EdgeFeatureMap::const_iterator fit = 
                                        features_->find(HyperEdge(l,c,r,t));
            if(fit == features_->end())
                THROW_ERROR("No features found in Score for l="
                                        <<l<<", c="<<c<<", r="<<r<<", t="<<(char)t);
            score += model.ScoreFeatureVector(*fit->second);
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
    return model.ScoreFeatureVector(SafeReference(vec));
}

// Build a hypergraph using beam search and cube pruning
SpanStack * HyperGraph::ProcessOneSpan(ReordererModel & model,
                                       const FeatureSet & features,
                                       const Sentence & sent,
                                       int l, int r,
                                       int beam_size, bool save_trg) {
    // Create the temporary data members for this span
    HypothesisQueue q;
    double score, viterbi_score;
    // If the length is OK, add a terminal
    if((features.GetMaxTerm() == 0) || (r-l < features.GetMaxTerm())) {
        int tl = (save_trg ? l : -1);
        int tr = (save_trg ? r : -1);
        // Create a hypothesis with the forward terminal
        score = GetEdgeScore(model, features, sent,
                                HyperEdge(l, -1, r, HyperEdge::EDGE_FOR));
        q.push(Hypothesis(score, score, l, r, tl, tr, HyperEdge::EDGE_FOR));
        if(features.GetUseReverse()) {
            // Create a hypothesis with the backward terminal
            score = GetEdgeScore(model, features, sent, 
                                    HyperEdge(l, -1, r, HyperEdge::EDGE_BAC));
            q.push(Hypothesis(score, score, l, r, tr, tl, HyperEdge::EDGE_BAC));
        }
    }
    TargetSpan *left_trg, *right_trg, 
               *new_left_trg, *old_left_trg,
               *new_right_trg, *old_right_trg;
    // Add the best hypotheses for each non-terminal to the queue
    for(int c = l+1; c <= r; c++) {
        // Find the best hypotheses on the left and right side
        left_trg = GetTrgSpan(l, c-1, 0);
        right_trg = GetTrgSpan(c, r, 0);
        if(left_trg == NULL) THROW_ERROR("Target l="<<l<<", c-1="<<c-1);
        if(right_trg == NULL) THROW_ERROR("Target c="<<c<<", r="<<r);
        // Add the straight terminal
        score = GetEdgeScore(model, features, sent, 
                                HyperEdge(l, c, r, HyperEdge::EDGE_STR));
        viterbi_score = score + left_trg->GetScore() + right_trg->GetScore();
        q.push(Hypothesis(viterbi_score, score, l, r,
                         left_trg->GetTrgLeft(), right_trg->GetTrgRight(),
                         HyperEdge::EDGE_STR, c, 0, 0, left_trg, right_trg));
        // Add the inverted terminal
        score = GetEdgeScore(model, features, sent, 
                                HyperEdge(l, c, r, HyperEdge::EDGE_INV));
        viterbi_score = score + left_trg->GetScore() + right_trg->GetScore();
        q.push(Hypothesis(viterbi_score, score, l, r,
                         right_trg->GetTrgLeft(), left_trg->GetTrgRight(),
                         HyperEdge::EDGE_INV, c, 0, 0, left_trg, right_trg));

    }
    // Get a map to store identical target spans
    map<pair<int,int>, TargetSpan*> spans;
    TargetSpan * trg_span = NULL;
    // Start beam search
    int num_processed = 0;
    while((!beam_size || num_processed < beam_size) && q.size()) {
        // Pop a hypothesis from the stack and get its target span
        Hypothesis hyp = q.top(); q.pop();
        pair<int,int> trg_idx(hyp.GetTrgLeft(), hyp.GetTrgRight());
        map<pair<int,int>, TargetSpan*>::iterator it = spans.find(trg_idx);
        if(it != spans.end()) {
            trg_span = it->second;
        } else {
            trg_span = new TargetSpan(hyp.GetLeft(), hyp.GetRight(), 
                                      hyp.GetTrgLeft(), hyp.GetTrgRight());
            spans.insert(MakePair(trg_idx, trg_span));
        }
        // Insert the hypothesis
        trg_span->AddHypothesis(hyp);
        num_processed++;
        // If the next hypothesis on the stack is equal to the current
        // hypothesis, remove it, as this just means that we added the same
        // hypothesis
        while(q.size() && q.top() == hyp) q.pop();
        // Skip terminals
        if(hyp.GetCenter() == -1) continue;
        // Increment the left side if there is still a hypothesis left
        new_left_trg = GetTrgSpan(l, hyp.GetCenter()-1, hyp.GetLeftRank()+1);
        if(new_left_trg) {
            old_left_trg = GetTrgSpan(l,hyp.GetCenter()-1,hyp.GetLeftRank());
            Hypothesis new_hyp(hyp);
            new_hyp.SetScore(hyp.GetScore() 
                        - old_left_trg->GetScore() + new_left_trg->GetScore());
            new_hyp.SetLeftRank(hyp.GetLeftRank()+1);
            new_hyp.SetLeftChild(new_left_trg);
            if(new_hyp.GetType() == HyperEdge::EDGE_STR) {
                new_hyp.SetTrgLeft(new_left_trg->GetTrgLeft());
            } else {
                new_hyp.SetTrgRight(new_left_trg->GetTrgRight());
            }
            q.push(new_hyp);
        }
        // Increment the right side if there is still a hypothesis right
        new_right_trg = GetTrgSpan(hyp.GetCenter(),r,hyp.GetRightRank()+1);
        if(new_right_trg) {
            old_right_trg = GetTrgSpan(hyp.GetCenter(),r,hyp.GetRightRank());
            Hypothesis new_hyp(hyp);
            new_hyp.SetScore(hyp.GetScore() 
                    - old_right_trg->GetScore() + new_right_trg->GetScore());
            new_hyp.SetRightRank(hyp.GetRightRank()+1);
            new_hyp.SetRightChild(new_right_trg);
            if(new_hyp.GetType() == HyperEdge::EDGE_STR) {
                new_hyp.SetTrgRight(new_right_trg->GetTrgRight());
            } else {
                new_hyp.SetTrgLeft(new_right_trg->GetTrgLeft());
            }
            q.push(new_hyp);
        }
    }
    SpanStack * ret = new SpanStack;
    typedef pair<pair<int,int>, TargetSpan*> MapPair;
    BOOST_FOREACH(const MapPair & map_pair, spans)
        ret->AddSpan(map_pair.second);
    sort(ret->GetSpans().begin(), ret->GetSpans().end(), 
                                DescendingScore<TargetSpan>());
    return ret;
}

// Build a hypergraph using beam search and cube pruning
void HyperGraph::BuildHyperGraph(ReordererModel & model,
                                 const FeatureSet & features,
                                 const Sentence & sent,
                                 int beam_size, bool save_trg) {
    n_ = sent[0]->GetNumWords();
    // Iterate through the right side of the span
    for(int r = 0; r < n_; r++) {
        // Move the span from l to r, building hypotheses from small to large
        for(int l = r; l >= 0; l--) {
            SetStack(l, r, ProcessOneSpan(model, features, sent, 
                                          l, r, beam_size, save_trg));
        }
    }
    // Build the root node
    SpanStack * top = GetStack(0,n_-1);
    SpanStack * root_stack = new SpanStack;
    for(int i = 0; i < (int)top->size(); i++) {
        TargetSpan * root = new TargetSpan(0, n_-1, (*top)[i]->GetTrgLeft(), (*top)[i]->GetTrgRight());
        root->AddHypothesis(Hypothesis((*top)[i]->GetScore(), 0, 0, n_-1, 0, n_-1,
                                    HyperEdge::EDGE_ROOT, -1, i, -1, (*top)[i]));
        root_stack->AddSpan(root);
    }
    stacks_.push_back(root_stack);
}

// Add up the loss over an entire subtree defined by span
double HyperGraph::AccumulateLoss(const TargetSpan* span) {
    const Hypothesis * hyp = span->GetHypothesis(0);
    double score = hyp->GetLoss();
    if(hyp->GetLeftChild())  score += AccumulateLoss(hyp->GetLeftChild());
    if(hyp->GetRightChild())  score += AccumulateLoss(hyp->GetRightChild());
    return score;
}

FeatureVectorInt HyperGraph::AccumulateFeatures(const TargetSpan* span) {
    std::map<int,double> feat_map;
    AccumulateFeatures(span, feat_map);
    FeatureVectorInt ret;
    BOOST_FOREACH(FeaturePairInt feat_pair, feat_map)
        ret.push_back(feat_pair);
    return ret;
}

void HyperGraph::AccumulateFeatures(const TargetSpan* span, 
                        std::map<int,double> & feat_map) {
    const Hypothesis * hyp = span->GetHypothesis(0);
    int l = hyp->GetLeft(), c = hyp->GetCenter(), r = hyp->GetRight();
    HyperEdge::Type t = hyp->GetType();
    // Find the features
    if(hyp->GetType() != HyperEdge::EDGE_ROOT) {
        EdgeFeatureMap::const_iterator fit = 
                                    features_->find(HyperEdge(l,c,r,t));
        if(fit == features_->end())
            THROW_ERROR("No features found in Accumulate for l="
                                    <<l<<", c="<<c<<", r="<<r<<", t="<<t);
        BOOST_FOREACH(FeaturePairInt feat_pair, *(fit->second))
            feat_map[feat_pair.first] += feat_pair.second;
    }
    if(hyp->GetLeftChild()) AccumulateFeatures(hyp->GetLeftChild(), feat_map);
    if(hyp->GetRightChild())AccumulateFeatures(hyp->GetRightChild(),feat_map);
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
                    span->SetHasType(hyp->GetType());
                    int top_id = nodes.GetId(GetNodeString(hyp->GetType(), hyp),true);
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
