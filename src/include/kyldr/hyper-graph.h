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

    // Constructor. Initialize the root node, which by default travels from -1
    // to the length of the source or target
    HyperGraph(int src_len, int trg_len) :
            nodes_(1, new HyperNode(0,HyperSpan(-1,
                                                src_len,
                                                MakePair(-1,-1),
                                                MakePair(trg_len,trg_len)))) {
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
        node_features_ = std::vector<FeatureVector*>(src_len*src_len, 0);
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

    double Parse() {
        // Delete the parse by resetting the best edge of all nodes to null
        BOOST_FOREACH(HyperNode * node, nodes_)
            node->SetBestEdge(-1);
        // Getting the cumulative score for the root will find the best
        // edge for all of the nodes, which allows us to find a parse
        return nodes_[0]->GetCumulativeScore();
    }

    // Add a single new edge and return a pointer to it
    HyperEdge * AddNewEdge(HyperEdge::Type type,
                           HyperNode* left = NULL,
                           HyperNode* right = NULL) {
        HyperEdge * edge = new HyperEdge(edges_.size(), type, left, right);
        edges_.push_back(edge);
        return edge;
    }

    // Accumulate features
    FeatureVector AccumulateFeatures() {
        std::map<std::string,double> feature_map;
        AccumulateFeatures(nodes_[0], feature_map);
        FeatureVector ret;
        typedef std::pair<std::string,double> ValPair;
        BOOST_FOREACH(ValPair feat_pair, feature_map)
            if(feat_pair.second != 0)
                ret.push_back(FeatureTuple(feat_pair.first,-1,feat_pair.second));
        return ret;
    }
    void AccumulateFeatures(const HyperNode * node,
                            std::map<std::string,double> & feature_map) const;

    // Accessors
    const int GetSrcLen() const { 
        return nodes_[0]->GetSpan().GetRight();
    }
    const int GetTrgLen() const {
        return nodes_[0]->GetSpan().GetTrgRight().first;
    }
    const std::vector<HyperNode*> & GetNodes() const { return nodes_; }
    const std::vector<HyperEdge*> & GetEdges() const { return edges_; }
    std::vector<HyperNode*> & GetNodes() { return nodes_; }
    std::vector<HyperEdge*> & GetEdges() { return edges_; }
    HyperNode* GetRoot() { return nodes_[0]; }
    const std::vector<HyperNode*> & GetLeftNeighbors(int id) const {
        return SafeAccess(left_neighbors_, id);
    }
    const std::vector<HyperNode*> & GetRightNeighbors(int id) const {
        return SafeAccess(right_neighbors_, id);
    }

    const FeatureVector * GetNodeFeatures(const HyperNode & node) const {
        return SafeAccess(node_features_, 
                          node.GetLeft()*GetSrcLen()+node.GetRight());
    }

    const FeatureVector * GetEdgeFeatures(
                                    const HyperNode & node,
                                    const HyperEdge & edge) const {
        int l = node.GetLeft(), r = node.GetRight(),
            c = edge.GetLeftChild()==NULL?-1:edge.GetLeftChild()->GetRight();
        EdgeFeatureMap::const_iterator eit =
            edge_features_.find(EdgeLocation(l, c, r, edge.GetType()));
        if(eit == edge_features_.end())
            return NULL;
        return eit->second;
    }

private:

    // Add non-terminals for a single pair of nodes
    void AddNonTerminalPair(HyperNode * left, HyperNode * right);

    // Permanent data structures
    std::vector<HyperNode*> nodes_; // The nodes of the hypergraph
    std::vector<HyperEdge*> edges_; // The edges of the hypergraph

    // Structures to hold features for nodes and edges, this is necessary
    // because many nodes will share the same features
    std::vector<FeatureVector*> node_features_;
    typedef quadruple<int,int,int,HyperEdge::Type> EdgeLocation;
    typedef std::map<EdgeLocation, FeatureVector> EdgeFeatureMap;
    EdgeFeatureMap edge_features_;

    // Intermediate data structures used in the building of the graph
    std::vector<std::vector<HyperNode*> > left_neighbors_, right_neighbors_;

    // The map used in holding the mapping between spans and hypernodes
    typedef std::map<HyperSpan, HyperNode*> SpanNodeMap;
    SpanNodeMap span_node_map_;

};

}

#endif

