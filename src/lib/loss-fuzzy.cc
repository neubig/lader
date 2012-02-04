#include <kyldr/loss-fuzzy.h>

using namespace kyldr;
using namespace std;

double LossFuzzy::AddLossToProduction(
        int left, int mid_left, int mid_right, int right,
        HyperEdge::Type type, const Ranks & ranks) {
    int loss = 0;
    switch (type) {
        // For roots, check that both edges are contiguous
        case HyperEdge::EDGE_ROOT:
            if(!Ranks::IsContiguous(-1, ranks[left]))
                loss = weight_;
            if(!Ranks::IsContiguous(ranks[right], ranks.GetMaxRank()+1))
                loss += weight_;
            break;
        // For straight non-terms, check inside values are contiguous
        case HyperEdge::EDGE_STR:
            if(!Ranks::IsContiguous(ranks[mid_left], ranks[mid_right]))
                loss = weight_;
            break;
        // For inverted non-terms, check outside values are contiguous
        case HyperEdge::EDGE_INV:
            if(!Ranks::IsContiguous(ranks[right], ranks[left]))
                loss = weight_;
            break;
        case HyperEdge::EDGE_FOR:
            for(int i = left; i < right; i++)
                if(!Ranks::IsContiguous(ranks[i], ranks[i+1]))
                    loss += weight_;
            break;
        case HyperEdge::EDGE_BAC:
            for(int i = right; i > left; i--)
                if(!Ranks::IsContiguous(ranks[i], ranks[i-1]))
                    loss += weight_;
            break;
        default:
            THROW_ERROR("Bad edge type in LossFuzzy");
    }
    return loss;
}
