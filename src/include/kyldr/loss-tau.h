#ifndef LOSS_TAU_H__
#define LOSS_TAU_H__

#include <kyldr/loss-base.h>

namespace kyldr {

// Loss inspired by Kendall's tau rank correlation measure
//  Kendall
// Which is used in machine translation by
//  Birch & Osborne and Isozaki et al.
//
// This counts all of the pairwise correlations between words, and loss is
// equal to the number of times words are out of order
class LossTau : public LossBase {
public:

    virtual double AddLossToProduction(
        int src_left, int src_mid, int src_right,
        int trg_left, int trg_midleft, int trg_midright, int trg_right,
        HyperEdge::Type type, const Ranks & ranks);

    // Calculate the accuracy of a single sentence
    virtual std::pair<double,double> CalculateSentenceLoss(
            const std::vector<int> order, const Ranks & ranks);

    // Initializes the loss calculator with a ranks 
    virtual void Initialize(const Ranks & ranks);

    virtual std::string GetName() const { return "tau"; }

private:

    // Get the loss of a single span
    double GetLoss(const Ranks & ranks, int l, int c, int r,
                    bool straight,std::vector<int> & losses);
    
    double GetLossStraight(const Ranks & ranks, int l, int c, int r) {
        return GetLoss(ranks, l, c, r, true, straight_);
    }
    double GetLossInverse(const Ranks & ranks, int l, int c, int r) {
        return GetLoss(ranks, l, c, r, false, inverse_);
    }
    int & AccessArray(std::vector<int> & arr, int l, int c, int r, int n) {
        return SafeAccess(arr, l*n*n+c*n+r);
    }

    // Get the loss for a particular array
    std::vector<int> straight_, inverse_;

};

}

#endif

