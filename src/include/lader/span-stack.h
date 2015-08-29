#ifndef SPAN_STACK_H__ 
#define SPAN_STACK_H__

#include <target-span.h>
#include <lader/feature-vector.h>

namespace lader {

class SpanStack {
public:
	SpanStack(int left, int right): left_(left), right_(right) {
        // resize the feature list in advance
    	if (right - left == 0){
    		straight.resize(1, NULL);
    		inverted.resize(1, NULL);
    	}
    	else{
    		straight.resize(right - left + 2, NULL);
    		inverted.resize(right - left + 2, NULL);
    	}
	}

	void Clear() {
		BOOST_FOREACH(TargetSpan * span, spans_)
			 delete span;
        span_map.clear();
        spans_.clear();
        while(!cands_.empty()){
            delete cands_.top();
            cands_.pop();
        }
	}
    virtual ~SpanStack() {
        Clear();
        BOOST_FOREACH(FeatureVectorInt* fvi, straight)
			if (fvi)
				delete fvi;
        BOOST_FOREACH(FeatureVectorInt* fvi, inverted)
			if (fvi)
				delete fvi;
    }

    TargetSpan* GetSpanOfRank(int rank) {
        if(rank < 0 || rank >= (int)spans_.size())
            return NULL;
        return spans_[rank];
    }
    const TargetSpan* GetSpanOfRank(int rank) const {
        if(rank < 0 || rank >= (int)spans_.size())
            return NULL;
        return spans_[rank];
    }

    void AddSpan(TargetSpan* span) {
        spans_.push_back(span);
    }
    void push_back(TargetSpan* span) {
        spans_.push_back(span);
    }

    size_t size() const { return spans_.size(); }
    const TargetSpan* operator[] (size_t val) const { return spans_[val]; }
    TargetSpan* operator[] (size_t val) { return spans_[val]; }
    const std::vector<TargetSpan*> & GetSpans() const { return spans_; }
    std::vector<TargetSpan*> & GetSpans() { return spans_; }
    int GetLeft() const { return left_; }
    int GetRight() const { return right_; }
    HypothesisQueue & GetCands() { return cands_; }
    size_t CandSize() const { return cands_.size(); }
    // for cube pruning/growing
    // add the new target span into this stack at first time
    // reuse the added target span later
    // this is safe whenever the search satisfies monotonicity
    TargetSpan* GetTargetSpan(const Hypothesis * hyp){
    	TargetSpan* trg_span;
    	int trg_idx = hyp->GetTrgLeft()*(right_+1) + hyp->GetTrgRight();
		IntMap<TargetSpan*>::iterator it = span_map.find(trg_idx);
		if(it != span_map.end())
			trg_span = it->second;
		else {
			trg_span = new TargetSpan(hyp->GetLeft(), hyp->GetRight(),
									  hyp->GetTrgLeft(), hyp->GetTrgRight());
			span_map.insert(MakePair(trg_idx, trg_span));
			AddSpan(trg_span);
		}
		return trg_span;
    }

    void SaveStraightFeautures(int c, FeatureVectorInt * fvi) {
    	if (straight[c < 0? 0 : c])
    		THROW_ERROR("Features are already saved")
		if (c < 0)
			straight[0] = fvi;
		else
			straight[c] = fvi;
	}
	void SaveInvertedFeautures(int c, FeatureVectorInt * fvi) {
		if (inverted[c < 0? 0 : c])
			THROW_ERROR("Features are already saved")
		if (c < 0)
			inverted[0] = fvi;
		else
			inverted[c] = fvi;
	}
	FeatureVectorInt * GetStraightFeautures(int c) {
		if (c < 0)
			return straight[0];
		else
			return straight[c];
	}
	FeatureVectorInt * GetInvertedFeautures(int c) {
		if (c < 0)
			return inverted[0];
		else
			return inverted[c];
	}
    void WriteFeatures(std::ostream & out)
    {
        int n = left_ == right_ ? 1 : right_ - left_ + 2;
        for (int c = 0 ; c < n ; c++){
			int i = 0;
			if (straight[c]) {
				BOOST_FOREACH(FeaturePairInt & feat, *(straight[c])){
					if (i++ != 0) out << " ";
					out << feat.first << ":" << feat.second;
				}
            }
			out << endl;
		}
        for (int c = 0 ; c < n ; c++){
			int i = 0;
			if (inverted[c]) {
				BOOST_FOREACH(FeaturePairInt & feat, *(inverted[c])){
					if (i++ != 0) out << " ";
					out << feat.first << ":" << feat.second;
				}
            }
			out << endl;
		}
    }

	void ReadFeatures(std::istream & in)
    {
        int n = left_ == right_ ? 1 : right_ - left_ + 2;
        for (int c = 0 ; c < n ; c++){
			std::string line;
			std::getline(in, line);
			if (line.empty()){
				SaveStraightFeautures(c, NULL);
				continue;
			}
			FeatureVectorInt * fvi = new FeatureVectorInt;
			int id;
			float value;
			const char * data = line.c_str();
			while(sscanf(data, "%d:%f", &id, &value) == 2){
				fvi->push_back(MakePair(id, value));
				while(!(*data == ' ' || *data == '\0'))
					data++;
				if (*data == ' ')
					data++;
				else
					break;
			}
			SaveStraightFeautures(c, fvi);
		}
        for (int c = 0 ; c < n ; c++){
			std::string line;
			std::getline(in, line);
			if (line.empty()){
				SaveInvertedFeautures(c, NULL);
				continue;
			}
			FeatureVectorInt * fvi = new FeatureVectorInt;
			int id;
			float value;
			const char * data = line.c_str();
			while(sscanf(data, "%d:%f", &id, &value) == 2){
				fvi->push_back(MakePair(id, value));
				while(!(*data == ' ' || *data == '\0'))
					data++;
				if (*data == ' ')
					data++;
				else
					break;
			}
			SaveInvertedFeautures(c, fvi);
		}
    }


    // IO Functions for stored features
    virtual void FeaturesToStream(std::ostream & out)
    {
        out << "[" << left_ << ", " << right_ << "]" << std::endl;
        WriteFeatures(out);
	}

    virtual void FeaturesFromStream(std::istream & in)
    {
        std::string line;
        std::stringstream header;
        header << "[" << left_ << ", " << right_ << "]";
        GetlineEquals(in, header.str());
        ReadFeatures(in);
	}

private:
    int left_, right_;
    std::vector<TargetSpan*> spans_;
	IntMap<TargetSpan*> span_map;

    // Store edge features
    // There are two possible orientations
    // Each list of FeatureVectorInts contains
	// list[0] is the terminal node (forward/backward)
	// list[1:] are the non-terminal nodes (straight/inverted)
    // It is possible to have NULL elements
    std::vector<FeatureVectorInt*> straight;
    std::vector<FeatureVectorInt*> inverted;

    // For cube growing
    HypothesisQueue cands_;
};

}

namespace std {
// Output function for pairs
inline std::ostream& operator << ( std::ostream& out,
                                   const lader::SpanStack & rhs )
{
    out << "[" << rhs.GetLeft() << ", " << rhs.GetRight() << "]";
    return out;
}
}
#endif

