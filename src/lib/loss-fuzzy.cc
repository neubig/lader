#include <kyldr/loss-fuzzy.h>

using namespace kyldr;
using namespace std;

double LossFuzzy::AddLossToEdge(const CombinedAlignment & combined,
                     const HyperNode * node,
                     HyperEdge * edge) {
    int loss = 0;
    const HyperSpan & span = node->GetSpan();
    switch (edge->GetType()) {
        // For roots, check that both edges are contiguous
        case HyperEdge::EDGE_ROOT:
#ifdef KYLDR_SAFE
            if(!edge->GetLeftChild())
                THROW_ERROR("Root edge without left child");
#endif
            if(!CombinedAlignment::IsContiguous(
                    span.GetTrgLeft(),
                    edge->GetLeftChild()->GetSpan().GetTrgLeft()))
                loss = weight_;
            if(!CombinedAlignment::IsContiguous(
                     edge->GetLeftChild()->GetSpan().GetTrgRight(),
                     span.GetTrgRight()))
                loss += weight_;
            break;
        // For straight non-terms, check inside values are contiguous
        case HyperEdge::EDGE_STR:
#ifdef KYLDR_SAFE
            if(!edge->GetLeftChild() || !edge->GetLeftChild())
                THROW_ERROR("Straight edge without two children");
#endif
            if(!CombinedAlignment::IsContiguous(
                    edge->GetLeftChild()->GetSpan().GetTrgRight(),
                    edge->GetRightChild()->GetSpan().GetTrgLeft()))
                loss = weight_;
            break;
        // For inverted non-terms, check outside values are contiguous
        case HyperEdge::EDGE_INV:
#ifdef KYLDR_SAFE
            if(!edge->GetLeftChild() || !edge->GetLeftChild())
                THROW_ERROR("Inverted edge without two children");
#endif
            if(!CombinedAlignment::IsContiguous(
                    edge->GetRightChild()->GetSpan().GetTrgRight(),
                    edge->GetLeftChild()->GetSpan().GetTrgLeft()))
                loss = weight_;
            break;
        case HyperEdge::EDGE_TERMSTR:
            for(int i = span.GetLeft(); i < span.GetRight(); i++)
                if(!CombinedAlignment::IsContiguous(combined[i],
                                                    combined[i+1]))
                    loss += weight_;
            break;
        case HyperEdge::EDGE_TERMINV:
            for(int i = span.GetRight(); i > span.GetLeft(); i--)
                if(!CombinedAlignment::IsContiguous(combined[i],
                                                    combined[i-1]))
                    loss += weight_;
            break;
        default:
            THROW_ERROR("Bad edge type in LossFuzzy");
    }
    // Add the loss to the edge
    edge->AddLoss(loss);
    return loss;
}
