#ifndef HYPER_GRAPH_H__ 
#define HYPER_GRAPH_H__

#include <map>
#include <climits>
#include <boost/foreach.hpp>
#include <kyldr/combined-alignment.h>
#include <kyldr/hyper-edge.h>
#include <kyldr/hyper-node.h>

namespace kyldr {

// A hypergraph representing a packed parse forest
class HyperGraph {
public:

    // Constructor
    HyperGraph() { }
    HyperGraph(int src_len, int trg_len) {
        InitializeStructures(src_len, trg_len);
    }

    // Destructor
    ~HyperGraph() {
        for(int i = 0; i < (int)nodes_.size(); i++)
            delete nodes_[i];
        for(int i = 0; i < (int)edges_.size(); i++)
            delete edges_[i];
    }

    // Initialize intermediate data structures based on sentence length
    void InitializeStructures(int src_len, int trg_len) {
        left_neighbors_ = std::vector<std::vector<HyperNode*> >(src_len);
        right_neighbors_ = std::vector<std::vector<HyperNode*> >(src_len);
        span_node_map_ = SpanNodeMap();
    }
    // Clear intermediate data structures when done building the graph
    void ClearStructures() {
        left_neighbors_.clear();
        right_neighbors_.clear();
        span_node_map_.clear();
    }

    // Get a node at a particular span and update all the indices needed for
    // building efficiently
    HyperNode * GetNodeAtSpan(const HyperSpan & span, bool add = true);

    // Add the non-terminal nodes to the hypergraph
    //  max_len:     The maximum length of a single terminal
    //  use_reverse: Whether to use reversed terminals where all of the words
    //               in a single span are in reverse order
    void AddTerminals(const CombinedAlignment & combined_alignment,
                      int max_len = INT_MAX,
                      bool use_reverse = true);

    // Add the non-terminal nodes to the hypergraph
    void AddNonTerminals();

    // Delete the parse by resetting the best edge of all nodes to null
    void DeleteParse() {
        BOOST_FOREACH(HyperNode * node, nodes_)
            node->SetBestEdge(-1);
    }

    // Add a single new edge and return a pointer to it
    HyperEdge * AddNewEdge(HyperEdge::Type type) {
        HyperEdge * edge = new HyperEdge(edges_.size(), type);
        edges_.push_back(edge);
        return edge;
    }

    // Accessors
    const std::vector<HyperNode*> & GetNodes() const { return nodes_; }
    const std::vector<HyperEdge*> & GetEdges() const { return edges_; }
    std::vector<HyperNode*> & GetNodes() { return nodes_; }
    std::vector<HyperEdge*> & GetEdges() { return edges_; }
    const std::vector<HyperNode*> & GetLeftNeighbors(int id) const {
        return SafeAccess(left_neighbors_, id);
    }
    const std::vector<HyperNode*> & GetRightNeighbors(int id) const {
        return SafeAccess(right_neighbors_, id);
    }

private:

    // Add non-terminals for a single pair of nodes
    void AddNonTerminalPair(const HyperNode * left, const HyperNode * right);

    // Permanent data structures
    std::vector<HyperNode*> nodes_; // The nodes of the hypergraph
    std::vector<HyperEdge*> edges_; // The edges of the hypergraph

    // Intermediate data structures used in the building of the graph
    std::vector<std::vector<HyperNode*> > left_neighbors_, right_neighbors_;

    // The map used in holding the mapping between spans and hypernodes
    typedef std::map<HyperSpan, HyperNode*> SpanNodeMap;
    SpanNodeMap span_node_map_;

};

}

#endif

