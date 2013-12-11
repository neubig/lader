#ifndef HYPER_EDGE_H__ 
#define HYPER_EDGE_H__

#include <string>
#include <sstream>
#include <iostream>

namespace lader {

#define HYPEREDGE_CMULT 255
#define HYPEREDGE_LMULT 65535
#define HYPEREDGE_TMULT 16777215

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

    // virtual desctuctor
    virtual ~HyperEdge(){}

    // Clone this object
    virtual HyperEdge * Clone() const{
    	return new HyperEdge(l_, c_, r_, t_);
    }
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
    
    bool operator!= (const HyperEdge & rhs) const {
    	return !(*this == rhs);
    }
    // TODO: l < c <= r or c = -1, can we use this property?
    size_t hash() const {
        return t_*HYPEREDGE_TMULT+l_*HYPEREDGE_LMULT+c_*HYPEREDGE_CMULT+r_;
    }

    // Accessors
    int GetLeft() const { return l_; }
    int GetRight() const { return r_; }
    int GetCenter() const { return c_; }
    Type GetType() const { return t_; }
    void SetType(Type t) { t_ = t; } // only for testing

private:
    // The left, center, and right words of the span
    // The center word is defined as -1 for terminals, and the first word in
    // the right-hand span for non-terminals
    int l_, c_, r_;
    // The type of the edge
    Type t_;

};

}

namespace std {
// Output function
inline std::ostream& operator << ( std::ostream& out,
                                   const lader::HyperEdge & rhs )
{
    out << " l=" << rhs.GetLeft() << ", c=" << rhs.GetCenter() << ", r=" << rhs.GetRight()
    	<< " : " << (char)rhs.GetType();
    return out;
}
}
#endif

