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
    virtual std::pair<double,double> CalculateSentenceLoss(
            const std::vector<int> order, const Ranks & ranks) {
        std::pair<double,double> ret(0,0);
        // If the maximum rank is zero, there is no way to mess up the loss,
        // so we just return zero
        if(ranks.GetMaxRank() == 0) return ret;
        // Otherwise, we assume the maximum loss is n+1, because there are n-1
        // bigrams between actual words, and 2 on the sentence boundaries. In
        // reality, for shorter sentences that have multiple words of the same
        // ranks this assumption will not hold (ie 0 0 1), but it is not worth
        // complicating things just for this
        int n = order.size();
        ret.second = n + 1;
        // Calculate the boundaries
        if(!Ranks::IsContiguous(-1, ranks[order[0]])) {
            ret.first++;
        }
        if(!Ranks::IsContiguous(ranks[order[n-1]], ranks.GetMaxRank()+1)) {
            ret.first++;
        }
        // Calculate all the rest
        for(int i = 1; i < n; i++)
            if(!Ranks::IsContiguous(ranks[order[i-1]], ranks[order[i]]))
                ret.first++;
        return ret;
    }

    virtual std::string GetName() const { return "fuzzy"; }


private:

};

}

#endif

