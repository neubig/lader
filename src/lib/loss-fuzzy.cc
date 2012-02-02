#include <kyldr/loss-fuzzy.h>

using namespace kyldr;
using namespace std;

double LossFuzzy::AddLossToProduction(
        int left, int mid_left, int mid_right, int right,
        HyperEdge::Type type, const CombinedAlignment & combined) {
    int loss = 0, len = combined.GetTrgLen();
    switch (type) {
        // For roots, check that both edges are contiguous
        case HyperEdge::EDGE_ROOT:
            if(!CombinedAlignment::IsContiguous(
                                        MakePair(-1,-1), combined[left]))
                loss = weight_;
            if(!CombinedAlignment::IsContiguous(
                                        combined[right], MakePair(len, len)))
                loss += weight_;
            break;
        // For straight non-terms, check inside values are contiguous
        case HyperEdge::EDGE_STR:
            if(!CombinedAlignment::IsContiguous(
                                    combined[mid_left], combined[mid_right]))
                loss = weight_;
            break;
        // For inverted non-terms, check outside values are contiguous
        case HyperEdge::EDGE_INV:
            if(!CombinedAlignment::IsContiguous(
                                    combined[right], combined[left]))
                loss = weight_;
            break;
        case HyperEdge::EDGE_FOR:
            for(int i = left; i < right; i++)
                if(!CombinedAlignment::IsContiguous(combined[i],
                                                    combined[i+1]))
                    loss += weight_;
            break;
        case HyperEdge::EDGE_BAC:
            for(int i = right; i > left; i--)
                if(!CombinedAlignment::IsContiguous(combined[i],
                                                    combined[i-1]))
                    loss += weight_;
            break;
        default:
            THROW_ERROR("Bad edge type in LossFuzzy");
    }
    return loss;
}
