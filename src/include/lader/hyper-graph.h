#ifndef HYPER_GRAPH_H__ 
#define HYPER_GRAPH_H__

#ifdef DEBUG
#define LOG(args...) fprintf (stderr, args)
#define RUN(statement) statement
#else
#define LOG(args...)
#define RUN(statement)
#endif

#include <iostream>
#include <lader/loss-base.h>
#include <lader/feature-data-base.h>
#include <lader/reorderer-model.h>
#include <lader/span-stack.h>
#include <lader/util.h>
#include <tr1/unordered_map>
#include <lader/thread-pool.h>
namespace lader {

template <class T>
struct DescendingScore {
  bool operator ()(T *lhs, T *rhs) { return rhs->GetScore() < lhs->GetScore(); }
};

class HyperGraph {
public:
    
    friend class TestHyperGraph;

    class TargetSpanTask : public Task {
    public:
    	TargetSpanTask(HyperGraph * graph, ReordererModel & model,
    			const FeatureSet & features,
    			const Sentence & sent, int l, int r, int beam_size, bool save_trg) :
    				graph_(graph), model_(model), features_(features),
    				source_(sent), l_(l), r_(r), beam_size_(beam_size), save_trg_(save_trg) { }
    	void Run(){
    		graph_->ProcessOneSpan(model_, features_,
    		    						source_, l_, r_, beam_size_, save_trg_);
    	}
    private:
    	HyperGraph * graph_;
    	ReordererModel & model_;
    	const FeatureSet & features_;
    	const Sentence & source_;
    	int l_, r_, beam_size_;
    	bool save_trg_;
    };

    class SpanStackTask : public Task {
        public:
        	SpanStackTask(HyperGraph * graph, ReordererModel & model,
    			const FeatureSet & features, const Sentence & source, int l, int r) :
        				graph_(graph), model_(model), features_(features),
        				source_(source), l_(l), r_(r) { }
        	void Run(){
    			graph_->SaveEdgeFeatures(l_, r_, model_, features_, source_);
        	}
        private:
        	HyperGraph * graph_;
        	ReordererModel & model_;
        	const FeatureSet & features_;
        	const Sentence & source_;
        	int l_, r_;
    };
    
    HyperGraph(bool cube_growing = false) : 
    	beam_size_(0), pop_limit_(0), n_(-1), threads_(1), cloned_(false), cube_growing_(cube_growing) { }

    virtual void Clear() {
		BOOST_FOREACH(SpanStack * stack, stacks_)
			delete stack;
		stacks_.clear();
	}

    virtual ~HyperGraph()
    {
        Clear();
    }

    virtual HyperGraph * Clone(){
    	HyperGraph * cloned = new HyperGraph(*this);
    	cloned->cloned_ = true;
    	return cloned;
    }

    // Build the hypergraph using the specified model, features and sentence
    void BuildHyperGraph(ReordererModel & model,
                         const FeatureSet & features,
                         const Sentence & sent, bool save_trg = true);

    const SpanStack * GetStack(int l, int r) const {
        return SafeAccess(stacks_, GetTrgSpanID(l,r));
    }
    SpanStack * GetStack(int l, int r) {
        return SafeAccess(stacks_, GetTrgSpanID(l,r));
    }
    const std::vector<SpanStack*> & GetStacks() const { return stacks_; }
    std::vector<SpanStack*> & GetStacks() { return stacks_; }


    // Scoring functions
    double Score(const ReordererModel & model, double loss_multiplier,
                 TargetSpan* span);
    double Score(const ReordererModel & model, double loss_multiplier,
                 Hypothesis* hyp);
    TargetSpan * GetTrgSpan(int l, int r, int rank) {
#ifdef LADER_SAFE
        if(l < 0 || r < 0 || rank < 0)
            THROW_ERROR("Bad GetTrgSpan (l="<<l<<", r="<<r<<")"<<std::endl);
#endif
        int idx = GetTrgSpanID(l,r);
        if((int)stacks_.size() <= idx)
            return NULL;
        else
            return SafeAccess(stacks_, idx)->GetSpanOfRank(rank);
    }

    // Print the whole hypergraph in JSON format
    void PrintHyperGraph(const std::vector<std::string> & sent,
                         std::ostream & out);

    // Rescore the hypergraph using the given model and a loss multiplier
    double Rescore(const ReordererModel & model, double loss_multiplier);

    const TargetSpan * GetRoot() const {
        return SafeAccess(stacks_, stacks_.size()-1)->GetSpanOfRank(0);
    }
    TargetSpan * GetRoot() {
        return SafeAccess(stacks_, stacks_.size()-1)->GetSpanOfRank(0);
    }
    SpanStack * GetRootStack() {
            return SafeAccess(stacks_, stacks_.size()-1);
    }
    Hypothesis * GetBest() {
		if (stacks_.empty() || !GetRoot())
			return NULL;
		return GetRoot()->GetHypothesis(0);
	}
    // Add up the loss over an entire sentence
    virtual void AddLoss(
    		LossBase* loss,
    		const Ranks * ranks,
            const FeatureDataParse * parse) ;

    // Add up the loss over an entire subtree defined by span
    double AccumulateLoss(const TargetSpan* span);

	virtual void AccumulateFeatures(std::tr1::unordered_map<int, double> & feat_map,
			ReordererModel & model, const FeatureSet & features,
			const Sentence & sent,
			const TargetSpan * span);

	int GetSrcLen() const { return n_; }

	void SetThreads(int threads) { threads_ = threads; }
    void SetBeamSize(int beam_size) { beam_size_ = beam_size; }
    void SetPopLimit(int pop_limit) { pop_limit_ = pop_limit; }
    void SetNumWords(int n){ n_ = n;}
    
   	// IO Functions for stored features
	virtual void FeaturesToStream(std::ostream & out){
		BOOST_FOREACH(SpanStack * stack, stacks_)
			stack->FeaturesToStream(out);
	}
	virtual void FeaturesFromStream(std::istream & in){
		BOOST_FOREACH(SpanStack * stack, stacks_)
			stack->FeaturesFromStream(in);
	}

	void SetAllStacks(int N){
		n_ = N;
		stacks_.resize(n_ * (n_+1) / 2 + 1, NULL); // resize stacks in advance
		for(int L = 1; L <= n_; L++)
			for(int l = 0; l <= n_-L; l++)
				SetStacks(l, l+L-1, true);
		// Set root stack at the end of the list
		SpanStack * root_stack = new SpanStack(0,n_);
		stacks_[n_ * (n_+1) / 2] = root_stack;
	}

    // edge-level parallelization of feature generation
    void SaveAllEdgeFeatures(ReordererModel & model,
    		const FeatureSet & features, const Sentence & source)
    {
        ThreadPool pool(threads_, 1000);
        for(int L = 1; L <= n_ ; L++)
            for(int l = 0 ; l <= n_-L ; l++){
                Task *task = new SpanStackTask(this, model, features, source, l, l + L - 1);
                pool.Submit(task);
            }

        pool.Stop(true);
    }

protected:
    void AddTerminals(int l, int r, const FeatureSet & features,
			ReordererModel & model, const Sentence & sent,
			HypothesisQueue & q, bool save_trg);
    // For both cube pruning/growing
    void IncrementLeft(const Hypothesis *old_hyp, TargetSpan *new_left,
			HypothesisQueue & q, int & pop_count);
	void IncrementRight(const Hypothesis *old_hyp, TargetSpan *new_right,
			HypothesisQueue & q, int & pop_count);

	virtual void SaveEdgeFeatures(int l, int r, ReordererModel & model,
			const FeatureSet & features, const Sentence & sent) {
		for(int c = l+1; c <= r; c++) {
			GetEdgeFeatures(model, features, sent,
					HyperEdge(l, c, r, HyperEdge::EDGE_STR));
			GetEdgeFeatures(model, features, sent,
					HyperEdge(l, c, r, HyperEdge::EDGE_INV));
		}
	}
	// For cube pruning/growing with threads > 1, we need to set same-sized stacks
	// in advance to ProcessOneSpan
	virtual void SetStacks(int l, int r, bool init = false) {
		// if -save_features, stack may exist
		if (init || !GetStack(l, r))
			SetStack(l, r, new SpanStack(l, r));
	}
	// For cube growing
	virtual void Trigger(int l, int r);
	virtual void LazyNext(HypothesisQueue & q, const Hypothesis * hyp,
			int & pop_count);
    virtual TargetSpan * LazyKthBest(SpanStack * stack, int k, int & pop_count);
    virtual void ProcessOneSpan(ReordererModel & model,
    		const FeatureSet & features,
			const Sentence & sent, int l, int r,
			int beam_size = 0, bool save_trg = true);
			
    const FeatureVectorInt * GetEdgeFeatures(
                                ReordererModel & model,
                                const FeatureSet & feature_gen,
                                const Sentence & sent,
                                const HyperEdge & edge);

    double GetEdgeScore(ReordererModel & model,
                        const FeatureSet & feature_gen,
                        const Sentence & sent,
                        const HyperEdge & edge);

    inline int GetTrgSpanID(int l, int r) const { 
        // If l=-1, we want the root, so return the last element of stacks_
        if(l == -1) return stacks_.size() - 1;
        return r*(r+1)/2 + l;
    }

    void SetStack(int l, int r, SpanStack * stack) {
#ifdef LADER_SAFE
        if(l < 0 || r < 0)
            THROW_ERROR("Bad SetStack (l="<<l<<", r="<<r<<")"<<std::endl);
#endif
        int idx = HyperGraph::GetTrgSpanID(l,r);
        if((int)stacks_.size() <= idx)
            stacks_.resize(idx+1, NULL);
        if(stacks_[idx]) //delete stacks_[idx];
	        THROW_ERROR("Stack exist [l="<<l<<", r="<<r<<"]"<<std::endl)
        stacks_[idx] = stack;
    }

private:
    // Stacks containing the hypotheses for each span
    // The indexing for the outer vector is:
    //  0-0 -> 0, 0-1 -> 1, 1-1 -> 2, 0-2 -> 3 ...
    // And can be recovered by GetHypothesis.
    // The inner vector contains target spans in descending rank of score
    std::vector<SpanStack*> stacks_;

    int pop_limit_;	// the pop limit used for cube growing (default=0, unlimited)
    int beam_size_;	// the beam size used for cube pruning/growing (default=0, unlimited)
    int threads_; 	// the number of threads for parallel feature generation (default=1)
    int n_;			// the length of the sentence
    bool cube_growing_, cloned_;

};

}

#endif

