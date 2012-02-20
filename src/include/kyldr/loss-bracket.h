#ifndef LOSS_BRACKET_H__
#define LOSS_BRACKET_H__

#include <kyldr/loss-base.h>

namespace kyldr {

// Calculates the loss according to bracketing precision over the true parse
// tree
class LossBracket : public LossBase {
public:

    virtual double AddLossToProduction(
        int src_left, int src_mid, int src_right,
        int trg_left, int trg_midleft, int trg_midright, int trg_right,
        HyperEdge::Type type,
        const Ranks * ranks, const FeatureDataParse * parse);

    // Calculate the accuracy of a single sentence
    virtual std::pair<double,double> CalculateSentenceLoss(
            const std::vector<int> order,
            const Ranks * ranks, const FeatureDataParse * parse);

    virtual std::string GetName() const { return "bracket"; }


private:

};

}

#endif

