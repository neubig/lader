#ifndef HYPER_EDGE_H__ 
#define HYPER_EDGE_H__

namespace kyldr {

class HyperEdge {
public:
    // The type of edge, STR and INV are straight and inverse non-terminals,
    // FOR and BAC are forward and backward terminals, and ROOT is a link
    // from the root node
    typedef enum {
        EDGE_STR = 'S',
        EDGE_INV = 'I',
        EDGE_FOR = 'F',
        EDGE_BAC = 'B',
        EDGE_ROOT = 'R'
    } Type;

    // Constructor
    HyperEdge(int l, int c, int r, Type t) : l_(l), c_(c), r_(r), t_(t) { }

    // Comparators
    bool operator< (const HyperEdge & rhs) const {
        return 
            l_ < rhs.l_ || (l_ == rhs.l_ && (
            c_ < rhs.c_ || (c_ == rhs.c_ && (
            r_ < rhs.r_ || (r_ == rhs.r_ && (
            t_ < rhs.t_))))));
    }
    bool operator== (const HyperEdge & rhs) const {
        return l_ == rhs.l_ && c_ == rhs.c_ && r_ == rhs.r_ && t_ == rhs.t_;
    }

    // Accessors
    int GetLeft() const { return l_; }
    int GetRight() const { return r_; }
    int GetCenter() const { return c_; }
    Type GetType() const { return t_; }

private:
    // The left, center, and right words of the span
    // The center word is defined as -1 for terminals, and the first word in
    // the right-hand span for non-terminals
    int l_, c_, r_;
    // The type of the edge
    Type t_;


};

}

#endif

