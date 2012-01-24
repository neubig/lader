#ifndef HYPER_SPAN_H__
#define HYPER_SPAN_H__

#include <iostream>
#include <vector>
#include <kyldr/util.h>

namespace kyldr {

// A bilingual span delimiting the a span of source words, and the trg
// words corresponding to the left and right sides of the source span
class HyperSpan {

public:

    // Constructor, trg is absent by default
    HyperSpan(int left = -1, int right = -1,
              std::pair<int,int> trg_left = std::pair<int,int>(-1,-1),
              std::pair<int,int> trg_right = std::pair<int,int>(-1,-1)) :
              left_(left), right_(right),
              trg_left_(trg_left), trg_right_(trg_right) { }

    // ---- Comparators ----
    inline bool operator==(const HyperSpan & b) const {
        return (
            left_ == b.left_ &&
            right_ == b.right_ &&
            trg_left_ == b.trg_left_ &&
            trg_right_ == b.trg_right_
        );
    }

    inline bool operator!=(const HyperSpan & b) const {
        return !(*this == b);
    }

    // ---- Comparators ----
    inline bool operator<(const HyperSpan & b) const {
        return (
            left_ < b.left_ || (left_ == b.left_ && (
            right_ < b.right_ || (right_ == b.right_ && (
            trg_left_ < b.trg_left_ || (trg_left_ == b.trg_left_ && (
            trg_right_ < b.trg_right_
        )))))));
    }

    // ---- Accessors ----
    int GetLeft() const { return left_; }
    int GetRight() const { return right_; }
    std::pair<int,int> GetTrgLeft() const { return trg_left_; }
    std::pair<int,int> GetTrgRight() const { return trg_right_; }

private:
    int left_; // The index of the leftmost word in the source
    int right_; // The index of the rightmost word in the span
    std::pair<int,int> trg_left_; // The span corresponding to "left_"
    std::pair<int,int> trg_right_; // The span corresponding to "right_"

};

}

namespace std {

// Output for HyperSpans
inline std::ostream& operator << ( std::ostream& out, 
                                   const kyldr::HyperSpan & rhs ) {
    out << "< " << rhs.GetLeft() << " " << rhs.GetRight() << " " 
        << rhs.GetTrgLeft() << " " << rhs.GetTrgRight() << " >";
    return out;
}

}

#endif
