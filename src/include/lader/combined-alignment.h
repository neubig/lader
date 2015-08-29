#ifndef COMBINED_ALIGNMENT__
#define COMBINED_ALIGNMENT__

#include <vector>
#include <lader/alignment.h>

namespace lader {

class CombinedAlign {
public: 

    // How to handle null alignments. Either leave them as-is [-1,-1], or
    // attach them to the left or right
    typedef enum {
        LEAVE_NULL_AS_IS,
        ATTACH_NULL_RIGHT,
        ATTACH_NULL_LEFT
    } NullHandler;

    // How to handle words on the source side that align to overlapping spans
    // on the target side. We can either leave them as is, or combine them
    // together until there are no overlapping alignments
    typedef enum {
        LEAVE_BLOCKS_AS_IS,
        COMBINE_BLOCKS
    } BlockHandler;

    // How to handle null-aligned brackets. Either leave them as-is, or
    // make sure that they are aligned directly to the left and right of
    // the words in-between them
    typedef enum {
        LEAVE_BRACKETS_AS_IS,
        ALIGN_BRACKET_SPANS
    } BracketHandler;
    static const int num_brackets_ = 7;
    static const char* opening_brackets_[num_brackets_];
    static const char* closing_brackets_[num_brackets_];

    // Constructor
    CombinedAlign() { }
    CombinedAlign(const std::vector<std::string> & words,
                  const Alignment & al,
                  NullHandler null_hand = LEAVE_NULL_AS_IS,
                  BlockHandler block_hand = LEAVE_BLOCKS_AS_IS,
                  BracketHandler bracket_hand = LEAVE_BRACKETS_AS_IS) {
        BuildFromAlignment(words, al, null_hand, block_hand, bracket_hand);
    }

    // Build a combined alignment from an uncombined alignment, using
    // the designated null handler to either leave nulls as is, or attach
    // them to their right or left aligned counterparts
    void BuildFromAlignment(const std::vector<std::string> & words,
                            const Alignment & align, 
                            NullHandler null_hand = LEAVE_NULL_AS_IS,
                            BlockHandler block_hand = LEAVE_BLOCKS_AS_IS,
                            BracketHandler bracket_hand = LEAVE_BRACKETS_AS_IS);


    // For brackets that are both null-aligned and matching, find the leftmost
    // and rightmost alignment that falls between them, and align them directly
    // to the left and right of the span
    void AlignBracketSpans(const std::vector<std::string> & words);
    
    // Combine alignments into contiguous blocks that can be recovered by an
    // ITG reordering
    void CombineBlocks();

    // Accessors
    const std::pair<double,double> & operator[] (size_t src) const {
        return SafeAccess(spans_, src);
    }
    const std::vector<std::pair<double,double> > & GetSpans() const {
        return spans_;
    }
    int GetSrcLen() const { return spans_.size(); }

private:
    
    // The set of opening and closing brackets to be used in alignment
    std::vector<std::pair<double,double> > spans_;

};

// Defines an ordering over alignments
// An alignment l is lesser than another alignment r if
//  a) l's beginning or end is less than r's beginning or end respectively
//  b) r's beginning and end are not lesser than l's beginning and end
struct AlignmentIsLesser {
    bool operator()(std::pair<double,double> l, std::pair<double,double> r) const {
        return (l.first < r.first && l.second <= r.second) ||
               (l.second < r.second && l.first <= r.first);
    }
};

}

#endif
