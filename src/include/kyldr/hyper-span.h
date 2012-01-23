#ifndef HYPER_SPAN_H__
#define HYPER_SPAN_H__

namespace kyldr {

// A bilingual span delimiting the a span of source words, and the target
// words corresponding to the left and right sides of the source span
class HyperSpan {

public:

    // Constructor, target is absent by default
    class HyperSpan(int left, int right, 
                    pair<int,int> target_left = pair<int,int>(-1,-1),
                    pair<int,int> target_right = pair<int,int>(-1,-1)) :
                    left_(left), right_(right),
                    target_left_(target_left), target_right_(target_right) { }

private:
    int left_; // The index of the leftmost word in the source
    int right_; // The index of the rightmost word in the span
    pair<int,int> target_left_; // The span corresponding to "left_"
    pair<int,int> target_right_; // The span corresponding to "right_"

};

}

#endif
