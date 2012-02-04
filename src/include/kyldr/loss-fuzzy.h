#ifndef LOSS_FUZZY_H__
#define LOSS_FUZZY_H__

#include <kyldr/loss-base.h>

namespace kyldr {

// Loss similar to the "fuzzy reordering score" of
//  A Lightweight Evaluation Framework for Machine Translation Reordering
//  David Talbot et al., WMT 2011
//
// This essentially measures the total number of words on the target side
// that are not contiguous after reordering
class LossFuzzy : public LossBase {
public:

    virtual double AddLossToProduction(
        int left, int mid_left, int mid_right, int right,
        HyperEdge::Type type, const Ranks & combined);

    // Calculate the accuracy of a single sentence
    virtual double CalculateSentenceAcccuracy(
            const std::vector<int> order, const Ranks & ranks) {
        THROW_ERROR("LossFuzzy::CalculateSentenceAccuracy is not implemented.");
    }


private:

};

}

#endif

