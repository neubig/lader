#ifndef LOSS_BASE_H__
#define LOSS_BASE_H__

#include <kyldr/combined-alignment.h>
#include <kyldr/hyper-graph.h>
#include <kyldr/hyper-node.h>
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
    
    // Add a loss value to "edge" that is a child of "node"
    virtual double AddLossToEdge(const CombinedAlignment & combined,
                                 const HyperNode * node, 
                                 HyperEdge * edge) = 0;

    // Initializes the loss calculator with a combined 
    virtual void Initialize(const CombinedAlignment & combined) { }

    // Add a loss value to each edge in a hypergraph
    void AddLossToHyperGraph(const CombinedAlignment & combined, 
                             HyperGraph & graph) {
        int added_edges = 0;
        Initialize(combined);
        std::vector<HyperNode*> & nodes = graph.GetNodes();
        for(int i = 0; i < (int)nodes.size(); i++) {
            std::vector<HyperEdge*> & edges = graph.GetEdges();
            for(int j = 0; j < (int)edges.size(); j++) {
                AddLossToEdge(combined, nodes[i], edges[j]);
                added_edges++;
            }
        }
        if(added_edges != (int)graph.GetEdges().size())
            THROW_ERROR("Didn't add loss for all edges ("
                        <<added_edges<<" != "<<graph.GetEdges().size());
    }

protected:
    double weight_;

};

}

#endif

