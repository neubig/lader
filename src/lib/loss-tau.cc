#include <kyldr/loss-tau.h>

using namespace kyldr;
using namespace std;

double LossTau::AddLossToProduction(
        int src_left, int src_mid, int src_right,
        int trg_left, int trg_midleft, int trg_midright, int trg_right,
        HyperEdge::Type type, const Ranks & ranks) {
    int loss = 0;
    switch (type) {
        // For roots, check that both edges are contiguous
        case HyperEdge::EDGE_ROOT:
            break;
        // For straight and inverse non-terms, check inside values
        case HyperEdge::EDGE_STR:
            loss = GetLossStraight(ranks, src_left, src_mid, src_right);
            break;
        case HyperEdge::EDGE_INV:
            loss = GetLossInverse(ranks, src_left, src_mid, src_right);
            break;
        case HyperEdge::EDGE_FOR:
            for(int i = src_left+1; i <= src_right; i++)
                loss += GetLossStraight(ranks, src_left, i, i);
            break;
        case HyperEdge::EDGE_BAC:
            for(int i = src_left+1; i <= src_right; i++)
                loss += GetLossInverse(ranks, src_left, i, i);
            break;
        default:
            THROW_ERROR("Bad edge type in LossTau");
    }
    return loss*weight_;
}

// Get the loss
int LossTau::GetLoss(const Ranks & ranks, int l, int c, int r,
               bool straight, std::vector<int> & losses) {
    // First check if this is already calculated
    int & loss = AccessArray(losses, l, c, r, ranks.GetSrcLen());
    // If the loss hasn't already been calculated
    if(loss == -1) {
        // Get the loss for everything but the rightmost value
        loss = (c==r?0:GetLoss(ranks,l,c,r-1,straight,losses));
        // For the rightmost value, calculate the number of times the rank
        // is out of order
        if(straight) {
            for(int i = l; i < c; i++)
                if(ranks[i] > ranks[r])
                    loss++;
        } else {
            for(int i = l; i < c; i++)
                if(ranks[i] < ranks[r])
                    loss++;
        }
    }
    return loss;
}

// Calculate the accuracy of a single sentence
std::pair<double,double> LossTau::CalculateSentenceLoss(
        const std::vector<int> order, const Ranks & ranks) {
    std::pair<double,double> ret(0,0);
    // Calculate the actual loss
    for(int i = 0; i < (int)order.size(); i++)
        for(int j = i+1; j < (int)order.size(); j++)
            if(ranks[order[i]] > ranks[order[j]])
                ret.first++;
    // Calculate the potential loss
    vector<int> sorted = ranks.GetRanks();
    sort(sorted.begin(), sorted.end());
    for(int i = 0; i < (int)order.size(); i++) {
        for(int j = i+1; j < (int)order.size(); j++) {
            if(sorted[j] > sorted[i]) {
                ret.second += (order.size() - j);
                break;
            }
        }
    }
    ret.first *= weight_;
    ret.second *= weight_;
    return ret;
}

void LossTau::Initialize(const Ranks & ranks) {
    int n = ranks.GetSrcLen();
    if((int)straight_.size() < n*n*n) straight_.resize(n*n*n);
    fill(straight_.begin(), straight_.begin() + n*n*n, -1);
    if((int)inverse_.size() < n*n*n) inverse_.resize(n*n*n);
    fill(inverse_.begin(), inverse_.begin() + n*n*n, -1);
}
