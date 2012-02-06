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
        int src_left, int src_mid, int src_right,
        int trg_left, int trg_midleft, int trg_midright, int trg_right,
        HyperEdge::Type type, const Ranks & ranks);

    // Calculate the accuracy of a single sentence
    virtual std::pair<double,double> CalculateSentenceLoss(
            const std::vector<int> order, const Ranks & ranks);

    virtual std::string GetName() const { return "fuzzy"; }


private:

};

}

#endif

