#ifndef HYPER_NODE_H__ 
#define HYPER_NODE_H__

#include <kyldr/feature-vector.h>
#include <kyldr/hyper-span.h>
#include <kyldr/hyper-edge.h>

namespace kyldr {

// A node in the hypergraph
class HyperNode {
public:
    HyperNode(int idx = -1, HyperSpan span = HyperSpan()) 
        : idx_(idx), score_(0), cumulative_score_(0), span_(span),
          best_edge_(-1) { }

    // Get the Viterbi score of the best subtree up to this node.
    // If we already know the best edge, we're fine, just return the score.
    // If we don't already have the best edge, find it using memoed recursion
    double GetCumulativeScore();

    // Add an edge to the array
    void AddEdge(HyperEdge * edge) { edges_.push_back(edge); }

    // Accessors
    int GetIdx() const { return idx_; }
    double GetScore() const { return score_; }
    const HyperSpan & GetSpan() const { return span_; }
    const FeatureVectorInt & GetFeatureVector() const { 
        return feature_vector_;
    }

    void SetScore(double score) { score_ = score; }
    void SetCumulativeScore(double score) { cumulative_score_ = score; }
    void SetFeatureVector(const FeatureVectorInt & feature_vector) {
        feature_vector_ = feature_vector;
    }
    void SetBestEdge(int best_edge) { best_edge_ = best_edge; }

private:
    int idx_; // The index of this node in the hypergraph
    double score_; // The score of this node
    double cumulative_score_; // The viterbi score of this node in a derivation
    HyperSpan span_; // The span that this node covers
    std::vector<HyperEdge*> edges_; // The edges that created this node
    int best_edge_; // The edge that is used in the viterbi parse rooted here
    FeatureVectorInt feature_vector_; // The vector of features
};

}

#endif

