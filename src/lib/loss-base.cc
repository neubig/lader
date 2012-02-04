#include <kyldr/loss-fuzzy.h>
#include <kyldr/loss-base.h>

using namespace kyldr;
using namespace std;

LossBase * LossBase::CreateNew(const string & type) {
    // Create a new feature base based on the type
    if(type == "fuzzy")
        return new LossFuzzy;
    else
        THROW_ERROR("Bad feature type " << type << " (must be 'seq')");
    return NULL;
}

void LossBase::AddLossToHyperGraph(const Ranks & ranks,
                                   HyperGraph & hyper_graph) {
    // For each span in the hypergraph
    int n = ranks.GetSrcLen();
    for(int r = 0; r <= n; r++) {
        // When r == n, we want the root, so only do -1
        for(int l = (r == n ? -1 : 0); l <= (r == n ? 0 : r); l++) {
            BOOST_FOREACH(TargetSpan* span,
                                    hyper_graph.GetStack(l,r)->GetSpans()) {
                BOOST_FOREACH(Hypothesis* hyp, span->GetHypotheses()) {
                    int trg_left = span->GetTrgLeft(),
                        trg_right = span->GetTrgRight(),
                        mid_left = -1, mid_right = -1;
                    if(hyp->GetLeftChild())
                        mid_left = hyp->GetLeftChild()->GetTrgRight();
                    if(hyp->GetRightChild())
                        mid_right = hyp->GetRightChild()->GetTrgLeft();
                    hyp->SetLoss(hyp->GetLoss() +
                                 AddLossToProduction(trg_left, mid_left,
                                                     mid_right, trg_right,
                                                     hyp->GetType(),
                                                     ranks));
                }
            }
        }
    }
}
