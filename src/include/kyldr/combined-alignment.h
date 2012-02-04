#ifndef COMBINED_ALIGNMENT__
#define COMBINED_ALIGNMENT__

#include <vector>
#include <kyldr/alignment.h>

namespace kyldr {

class CombinedAlign {
public: 

    // How to handle null alignments. Either leave them as-is [-1,-1], or
    // attach them to the left or right
    typedef enum {
        LEAVE_NULL_AS_IS,
        ATTACH_NULL_RIGHT,
        ATTACH_NULL_LEFT
    } NullHandler;

    // Constructor, do nothing
    CombinedAlign() : trg_len_(-1) { }
    CombinedAlign(const Alignment & al,
                      NullHandler handler = LEAVE_NULL_AS_IS) {
        BuildFromAlignment(al, handler);
    }

    // Build a combined alignment from an uncombined alignment, using
    // the designated null handler to either leave nulls as is, or attach
    // them to their right or left aligned counterparts
    void BuildFromAlignment(const Alignment & align, 
                            NullHandler handler = LEAVE_NULL_AS_IS);

    // Accessors
    const std::pair<int,int> & operator[] (size_t src) const {
        return SafeAccess(spans_, src);
    }
    int GetSrcLen() const { return spans_.size(); }
    int GetTrgLen() const { return trg_len_; }

private:

    std::vector<std::pair<int,int> > spans_;
    int trg_len_;

};

// Defines an ordering over alignments
// An alignment l is lesser than another alignment r if
//  a) l's beginning or end is less than r's beginning or end respectively
//  b) r's beginning and end are not lesser than l's beginning and end
struct AlignmentIsLesser {
    bool operator()(std::pair<int,int> l, std::pair<int,int> r) {
        return (l.first < r.first && l.second <= r.second) ||
               (l.second < r.second && l.first <= r.first);
    }
};

}

#endif
