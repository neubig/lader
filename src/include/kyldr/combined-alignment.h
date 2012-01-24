#ifndef COMBINED_ALIGNMENT__
#define COMBINED_ALIGNMENT__

#include <vector>
#include <kyldr/alignment.h>

namespace kyldr {

class CombinedAlignment {
public: 

    // How to handle null alignments. Either leave them as-is [-1,-1], or
    // attach them to the left or right
    typedef enum {
        LEAVE_NULL_AS_IS,
        ATTACH_NULL_RIGHT,
        ATTACH_NULL_LEFT
    } NullHandler;

    // Constructor, do nothing
    CombinedAlignment() { }
    CombinedAlignment(const Alignment & al) { BuildFromAlignment(al); }

    // Build a combined alignment from an uncombined alignment, using
    // the designated null handler to either leave nulls as is, or attach
    // them to their right or left aligned counterparts
    void BuildFromAlignment(const Alignment & align, 
                            NullHandler handler = LEAVE_NULL_AS_IS);

    // Measure whether two combined spans are contiguous.
    // We define two conditions for contiguity
    //   1) (right.first == left.first && right.second >= left.first)
    //   2) (right.first > left.first && right.first <= left.second+1)
    // 
    // This indicates that the following alignments are contiguous
    //   x.   xx    x.   x.
    //   .x   .x    xx   .x
    //                   x.
    // But the following alignments are not
    //   .x   .x
    //   xx   x.
    //        .x
    static bool IsContiguous(std::pair<int,int> left,
                             std::pair<int,int> right) {
        return (right.first == left.first && right.second >= left.first) ||
               (right.first > left.first && right.first <= left.second+1);
    }

    // Accessors
    const std::pair<int,int> & operator[] (size_t src) const {
        return SafeAccess(spans_, src);
    }
    int size() const { return spans_.size(); }

private:

    std::vector<std::pair<int,int> > spans_;

};

}

#endif
