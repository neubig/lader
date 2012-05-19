#include <lader/loss-fuzzy.h>
#include <lader/loss-tau.h>
#include <lader/loss-bracket.h>
#include <lader/loss-base.h>

using namespace lader;
using namespace std;

LossBase * LossBase::CreateNew(const string & type) {
    // Create a new feature base based on the type
    if(type == "fuzzy")
        return new LossFuzzy;
    else if(type == "tau")
        return new LossTau;
    else if(type == "bracket")
        return new LossBracket;
    else
        THROW_ERROR("Bad loss type " << type << " (must be fuzzy/tau)");
    return NULL;
}

void LossBase::AddLossToHyperGraph(const Ranks * ranks,
                                   const FeatureDataParse * parse,
                                   HyperGraph & hyper_graph) {
    // Initialize the loss
    Initialize(ranks, parse);
    // For each span in the hypergraph
    int n = hyper_graph.GetSrcLen();
    for(int r = 0; r <= n; r++) {
        // When r == n, we want the root, so only do -1
        for(int l = (r == n ? -1 : 0); l <= (r == n ? -1 : r); l++) {
            // DEBUG cerr << "l=" << l << ", r=" << r << ", n=" << n << endl;
            BOOST_FOREACH(TargetSpan* span,
                                    hyper_graph.GetStack(l,r)->GetSpans()) {
                BOOST_FOREACH(Hypothesis* hyp, span->GetHypotheses()) {
                    int trg_left = span->GetTrgLeft(),
                        trg_right = span->GetTrgRight(),
                        trg_midleft = -1, trg_midright = -1,
                        src_mid = -1;
                    if(hyp->GetType() == HyperEdge::EDGE_STR) {
                        trg_midleft = hyp->GetLeftChild()->GetTrgRight();
                        trg_midright = hyp->GetRightChild()->GetTrgLeft();
                        src_mid = hyp->GetCenter();
                    } else if(hyp->GetType() == HyperEdge::EDGE_INV) {
                        trg_midleft = hyp->GetRightChild()->GetTrgRight();
                        trg_midright = hyp->GetLeftChild()->GetTrgLeft();
                        src_mid = hyp->GetCenter();
                    }
                    // DEBUG cerr << "GetLoss = " <<hyp->GetLoss()<<endl;
                    hyp->SetLoss(hyp->GetLoss() +
                                 AddLossToProduction(l, src_mid, r,
                                                     trg_left, trg_midleft,
                                                     trg_midright, trg_right,
                                                     hyp->GetType(),
                                                     ranks, parse));
                }
            }
        }
    }
}
