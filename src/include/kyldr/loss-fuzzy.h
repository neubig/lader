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

    double AddLossToEdge(const CombinedAlignment & combined,
                         const HyperNode * node,
                         HyperEdge * edge) {
        int loss = 0;
        const HyperSpan & span = node->GetSpan();
        // For straight non-terms, check if the inside values are contiguous
        if(edge->GetType() == HyperEdge::EDGE_STR)
            loss = CombinedAlignment::IsContiguous(
                                edge->GetLeftChild()->GetSpan().GetTrgRight(),
                                edge->GetRightChild()->GetSpan().GetTrgLeft()) ? 0 : 1;
        // For inverted non-terms, check if the outside values are contiguous
        else if(edge->GetType() == HyperEdge::EDGE_INV)
            loss = CombinedAlignment::IsContiguous(
                                edge->GetRightChild()->GetSpan().GetTrgRight(),
                                edge->GetLeftChild()->GetSpan().GetTrgLeft()) ? 0 : 1;
        // For straight terminals, check in order
        else if(edge->GetType() == HyperEdge::EDGE_TERMSTR) {
            for(int i = span.GetLeft(); i < span.GetRight(); i++)
                loss += CombinedAlignment::IsContiguous(combined[i],
                                                        combined[i+1]) ? 0 : 1;
        }
        // For inverted terminals, check in reverse order
        else if(edge->GetType() == HyperEdge::EDGE_TERMINV) {
            for(int i = span.GetRight(); i > span.GetLeft(); i--)
                loss += CombinedAlignment::IsContiguous(combined[i],
                                                        combined[i-1]) ? 0 : 1;
        }
        // Add the loss to the edge
        edge->AddLoss(loss);
        return loss;
    }

private:

};

}

#endif

