#include <cfloat>
#include <kyldr/hyper-node.h>
#include <kyldr/feature-vector.h>
#include <boost/foreach.hpp>

using namespace kyldr;
using namespace std;

double HyperNode::GetCumulativeScore() { 
    if(best_edge_ == -1) {
        cumulative_score_ = -DBL_MAX;
        for(int i = 0; i < (int)edges_.size(); i++) {
            double my_score = score_;
            my_score += SafeAccess(edges_, i)->GetScore();
            if(edges_[i]->GetLeftChild() != NULL)
                my_score += edges_[i]->GetLeftChild()->GetCumulativeScore();
            if(edges_[i]->GetRightChild() != NULL)
                my_score += edges_[i]->GetRightChild()->GetCumulativeScore();
            if(my_score > cumulative_score_) {
                cumulative_score_ = my_score;
                best_edge_ = i;
            }
        }
    }
    return cumulative_score_;
}

double HyperNode::AccumulateLoss() const {
    HyperEdge * edge = SafeAccess(edges_, best_edge_);
    double loss = edge->GetLoss();
    if(edge->GetLeftChild() != NULL)
        loss += edge->GetLeftChild()->AccumulateLoss();
    if(edge->GetRightChild() != NULL)
        loss += edge->GetRightChild()->AccumulateLoss();
    return loss;
}
