#ifndef LOSS_CHUNK_H__
#define LOSS_CHUNK_H__

#include <lader/loss-base.h>

namespace lader {

// Loss similar to the "fuzzy reordering score" of
//  A Lightweight Evaluation Framework for Machine Translation Reordering
//  David Talbot et al., WMT 2011
// (called "chunk fragmentation" in METEOR and lader papers)
//
// This essentially measures the total number of words on the target side
// that are not contiguous after reordering
class LossChunk : public LossBase {
public:

	virtual double AddLossToProduction(Hypothesis * hyp,
	    		const Ranks * ranks, const FeatureDataParse * parse);
	    		
    virtual double AddLossToProduction(
        int src_left, int src_mid, int src_right,
        int trg_left, int trg_midleft, int trg_midright, int trg_right,
        HyperEdge::Type type,
        const Ranks * ranks, const FeatureDataParse * parse);

    // Calculate the accuracy of a single sentence
    virtual std::pair<double,double> CalculateSentenceLoss(
            const std::vector<int> order,
            const Ranks * ranks, const FeatureDataParse * parse);

    virtual std::string GetName() const { return "chunk"; }


private:
    bool IsStraight(const Ranks * ranks, int trg_midleft, int trg_midright);

};

}

#endif

