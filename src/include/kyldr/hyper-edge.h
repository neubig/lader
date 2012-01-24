#ifndef HYPER_EDGE__
#define HYPER_EDGE__

namespace kyldr {

class HyperNode;

// An edge in a hypergraph
class HyperEdge {

public:
    // The type of edge
    typedef enum {
        EDGE_STR,     // A straight non-terminal
        EDGE_INV,     // An inverted non-terminal
        EDGE_TERMSTR, // A straight terminal (all words in order)
        EDGE_TERMINV, // An inverted non-terminal (all words in reverse order)
        EDGE_NO_TYPE  // No edge type, shouldn't be used normally
    } Type;

    // Constructor
    HyperEdge(int id = -1, Type type = EDGE_NO_TYPE, 
              HyperNode * left_child = NULL, HyperNode * right_child = NULL)
        : id_(id), type_(type),
          left_child_(left_child), right_child_(right_child) { }

    // Add a single value to the loss vector
    void AddLoss(double loss) { losses_.push_back(loss); }

    // Accessors
    Type GetType() const { return type_; }
    const HyperNode* GetLeftChild() const { return left_child_; }
    const HyperNode* GetRightChild() const { return right_child_; }

private:

    int id_;    // The ID of the hyperedge in the hypergraph
    Type type_; // The type of hyperedge
    // Left and right children, only active for non-terminals
    HyperNode *left_child_, *right_child_;
    std::vector<double> losses_; // The losses assigned to this edge

};

}

#endif
