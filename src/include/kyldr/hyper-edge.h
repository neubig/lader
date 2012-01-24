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
    HyperEdge(int id = -1, Type type = EDGE_NO_TYPE)
        : id_(id), type_(type), left_child_(NULL), right_child_(NULL) { }

private:

    int id_;    // The ID of the hyperedge in the hypergraph
    Type type_; // The type of hyperedge
    HyperNode *left_child_, *right_child_;
        // Left and right children, only active for non-terminals

};

}

#endif
