#ifndef HYPER_EDGE__
#define HYPER_EDGE__

#include <kyldr/feature-vector.h>

namespace kyldr {

class HyperNode;

// An edge in a hypergraph
class HyperEdge {

public:
    // The type of edge
    typedef enum {
        EDGE_STR = 'S',     // A straight non-terminal
        EDGE_INV = 'I',     // An inverted non-terminal
        EDGE_TERMSTR = 'F', // A straight terminal (all words in order)
        EDGE_TERMINV = 'B', // An inverted non-terminal (all words in reverse)
        EDGE_NO_TYPE = 'N'  // No edge type, shouldn't be used normally
    } Type;

    // Constructor
    HyperEdge(int id = -1, Type type = EDGE_NO_TYPE, 
              HyperNode * left_child = NULL, HyperNode * right_child = NULL)
        : id_(id), type_(type), score_(0.0),
          left_child_(left_child), right_child_(right_child) { }

    // Add a single value to the loss vector
    void AddLoss(double loss) { losses_.push_back(loss); }

    // Accessors
    int GetIndex() const { return id_; }
    Type GetType() const { return type_; }
    double GetScore() const { return score_; }
    const HyperNode* GetLeftChild() const { return left_child_; }
    const HyperNode* GetRightChild() const { return right_child_; }
    HyperNode* GetLeftChild() { return left_child_; }
    HyperNode* GetRightChild() { return right_child_; }
    const std::vector<double> & GetLossVector() const { return losses_; }
    const FeatureVectorInt & GetFeatureVector() const { 
        return feature_vector_;
    }

    void SetScore(double score) { score_ = score; }
    void SetFeatureVector(const FeatureVectorInt & feature_vector) {
        feature_vector_ = feature_vector;
    }
    void SetLossVector(const std::vector<double> & losses) { losses_ = losses; }
    void SetLeftChild (HyperNode* child) { left_child_ = child; }
    void SetRightChild(HyperNode* child) { right_child_ = child; }


private:

    int id_;    // The ID of the hyperedge in the hypergraph
    Type type_; // The type of hyperedge
    double score_; // The score assigned to this edge by the weights
    // Left and right children, only active for non-terminals
    HyperNode *left_child_, *right_child_;
    std::vector<double> losses_; // The losses assigned to this edge
    FeatureVectorInt feature_vector_; // The vector of features

};

}

#endif
