#ifndef HYPER_NODE_H__ 
#define HYPER_NODE_H__

#include <kyldr/hyper-span.h>

namespace kyldr {

// A node in the hypergraph
class HyperNode {
public:
    HyperNode(int idx = -1, HyperSpan span = HyperSpan()) 
        : idx_(idx), span_(span) { }

    // Add an edge to the array
    void AddEdge(HyperEdge * edge) { edges_.push_back(edge); }

    // Accessors
    int GetIdx() const { return idx_; }
    const HyperSpan & GetSpan() const { return span_; }

private:
    int idx_; // The index of this node in the hypergraph
    HyperSpan span_; // The span that this node covers
    std::vector<HyperEdge*> edges_; // The edges that created this node
};

}

#endif

