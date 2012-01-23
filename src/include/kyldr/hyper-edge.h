#ifndef HYPER_EDGE__
#define HYPER_EDGE__

namespace kyldr {

typedef enum {
    EDGE_STR,     // A straight non-terminal
    EDGE_INV,     // An inverted non-terminal
    EDGE_TERMSTR, // A straight terminal (all words in order)
    EDGE_TERMINV  // An inverted non-terminal (all words in reverse order)
} HyperEdgeType;

class HyperNode;

// An edge in a hypergraph
class HyperEdge {

public:
    // Constructor
    class HyperEdge(HyperEdgeType type)
        : type_(type), id_(-1), left_child_(NULL), right_child(NULL) { }

private:

    HyperEdgeType type_; // The type of hyperedge
    int id_;             // The ID of the hyperedge in the hypergraph
    HyperNode * left_child_, right_child_;
        // Left and right children, only active for non-terminals

};

}

#endif
