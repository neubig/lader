#include <kyldr/loss-fuzzy.h>

using namespace kyldr;
using namespace std;

double LossFuzzy::AddLossToProduction(
        int src_left, int src_mid, int src_right,
        int trg_left, int trg_midleft, int trg_midright, int trg_right,
        HyperEdge::Type type,
        const Ranks * ranks, const FeatureDataParse * parse) {
    if(!ranks) THROW_ERROR("Fuzzy loss requires alignment input");
    int loss = 0;
    switch (type) {
        // For roots, check that both edges are contiguous
        case HyperEdge::EDGE_ROOT:
            if(!Ranks::IsContiguous(-1, (*ranks)[trg_left]))
                loss++;
            if(!Ranks::IsContiguous((*ranks)[trg_right], (*ranks).GetMaxRank()+1))
                loss++;
            break;
        // For straight and inverse non-terms, check inside values
        case HyperEdge::EDGE_STR:
        case HyperEdge::EDGE_INV:
            if(!Ranks::IsContiguous((*ranks)[trg_midleft], (*ranks)[trg_midright]))
                loss++;
            break;
        case HyperEdge::EDGE_FOR:
            for(int i = trg_left; i < trg_right; i++)
                if(!Ranks::IsContiguous((*ranks)[i], (*ranks)[i+1]))
                    loss++;
            break;
        case HyperEdge::EDGE_BAC:
            for(int i = trg_left; i > trg_right; i--)
                if(!Ranks::IsContiguous((*ranks)[i], (*ranks)[i-1]))
                    loss++;
            break;
        default:
            THROW_ERROR("Bad edge type in LossFuzzy");
    }
    return loss*weight_;
}


// Calculate the accuracy of a single sentence
std::pair<double,double> LossFuzzy::CalculateSentenceLoss(
        const std::vector<int> order,
        const Ranks * ranks, const FeatureDataParse * parse) {
    if(!ranks) THROW_ERROR("Fuzzy loss requires alignment input");
    std::pair<double,double> ret(0,0);
    // If the maximum rank is zero, there is no way to mess up the loss,
    // so we just return zero
    if(ranks->GetMaxRank() == 0) return ret;
    // Otherwise, we assume the maximum loss is n+1, because there are n-1
    // bigrams between actual words, and 2 on the sentence boundaries. In
    // reality, for shorter sentences that have multiple words of the same
    // ranks this assumption will not hold (ie 0 0 1), but it is not worth
    // complicating things just for this
    int n = order.size();
    ret.second = n + 1;
    // Calculate the boundaries
    if(!Ranks::IsContiguous(-1, (*ranks)[order[0]])) {
        ret.first++;
    }
    if(!Ranks::IsContiguous((*ranks)[order[n-1]], (*ranks).GetMaxRank()+1)) {
        ret.first++;
    }
    // Calculate all the rest
    for(int i = 1; i < n; i++)
        if(!Ranks::IsContiguous((*ranks)[order[i-1]], (*ranks)[order[i]]))
            ret.first++;
    ret.first *= weight_;
    ret.second *= weight_;
    return ret;
}
