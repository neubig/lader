#include <kyldr/loss-fuzzy.h>

using namespace kyldr;
using namespace std;

double LossFuzzy::AddLossToProduction(
        int left, int mid_left, int mid_right, int right,
        HyperEdge::Type type, const Ranks & ranks) {
    double loss = 0;
    switch (type) {
        // For roots, check that both edges are contiguous
        case HyperEdge::EDGE_ROOT:
            if(!Ranks::IsContiguous(-1, ranks[left]))
                loss = weight_;
            if(!Ranks::IsContiguous(ranks[right], ranks.GetMaxRank()+1))
                loss += weight_;
            break;
        // For straight and inverse non-terms, check inside values
        case HyperEdge::EDGE_STR:
        case HyperEdge::EDGE_INV:
            if(!Ranks::IsContiguous(ranks[mid_left], ranks[mid_right]))
                loss = weight_;
            break;
        case HyperEdge::EDGE_FOR:
            for(int i = left; i < right; i++)
                if(!Ranks::IsContiguous(ranks[i], ranks[i+1]))
                    loss += weight_;
            break;
        case HyperEdge::EDGE_BAC:
            for(int i = left; i > right; i--)
                if(!Ranks::IsContiguous(ranks[i], ranks[i-1]))
                    loss += weight_;
            break;
        default:
            THROW_ERROR("Bad edge type in LossFuzzy");
    }
    // DEBUG cerr << "Loss at " <<(char)type<<","<< left <<"[" <<ranks[left]<<"]," <<mid_left<<"[" <<(mid_left==-1?-1:ranks[mid_left])<<"]," <<mid_right<<"[" <<(mid_right==-1?-1:ranks[mid_right])<<"]," <<right << "[" <<ranks[right]<<"]," <<" == " << loss << endl;
    return loss;
}
