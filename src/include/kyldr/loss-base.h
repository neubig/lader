#ifndef LOSS_BASE_H__
#define LOSS_BASE_H__

#include <kyldr/combined-alignment.h>
#include <kyldr/hyper-graph.h>
#include <kyldr/hyper-edge.h>

namespace kyldr {

// A base class that adds losses to all of the cells and edges in a hypergraph
// To create new loss functions, it is necessary to re-implement 
//   AddLossToEdge (and also optionally Initialize)
class LossBase {
public:

    // Constructor and destructor
    LossBase() : weight_(1.0) { }
    virtual ~LossBase() { }
    
    // Add a loss value to a production. left and right are the left and right
    // sides of the span on the target side. mid_left and mid_right are active
    // for non-terminals, and represent the right side of the left span and the
    // left side of the right span respectively
    virtual double AddLossToProduction(
        int left, int mid_left, int mid_right, int right,
        HyperEdge::Type type, const CombinedAlignment & combined) = 0;

    // Initializes the loss calculator with a combined 
    virtual void Initialize(const CombinedAlignment & combined) { }

    void AddLossToHyperGraph(const CombinedAlignment & combined,
                             HyperGraph & hyper_graph);

    // Create a new sub-class of a particular type
    //  type=fuzzy --> LossFuzzy
    static LossBase* CreateNew(const std::string & type);

    // Accessors
    void SetWeight(double weight) { weight_ = weight; }

protected:
    double weight_;

};

}

#endif

