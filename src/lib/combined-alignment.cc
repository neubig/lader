
#include <kyldr/combined-alignment.h>

using namespace kyldr;
using namespace std; 

void CombinedAlign::BuildFromAlignment(
        const Alignment & align,
        CombinedAlign::NullHandler handler) {
    // Save the lengths
    trg_len_ = align.GetTrgLen();
    // Create the new alignment vector
    const std::vector<pair<int,int> > & vec = align.GetAlignmentVector();
    spans_ = std::vector<std::pair<int,int> >(align.GetSrcLen(),
                                              MakePair(-1,-1));
    // Combine the spans
    for(int i = 0; i < (int)vec.size(); i++) {
        int src = vec[i].first, trg = vec[i].second;
        if(spans_[src].first == -1) {
            spans_[src] = MakePair(trg,trg);
        } else {
            spans_[src].first = min(spans_[src].first, trg);
            spans_[src].second = max(spans_[src].second, trg);
        }
    }
    // Attach any null alignments
    int i;
    if(handler == ATTACH_NULL_LEFT) {
        // Attach all words to the left
        for(i = 1; i < (int)spans_.size(); i++)
            if(spans_[i].first == -1)
                spans_[i] = spans_[i-1];
        // Attach trailing nulls to the right
        for(i = 0; i < (int)spans_.size() && spans_[i].first == -1; i++);
        if(i != (int)spans_.size()) // As long as all are not null
            for( ; i > 0; i--)
                spans_[i-1] = spans_[i];
    } else if(handler == ATTACH_NULL_RIGHT) {
        // Attach all words to the right
        for(i = spans_.size()-2; i >= 0; i--)
            if(spans_[i].first == -1)
                spans_[i] = spans_[i+1];
        // Attach trailing nulls to the left
        for(i = spans_.size()-1; i >= 0 && spans_[i].first == -1; i--);
        if(i != -1) // As long as all are not null
            for( ; i < (int)spans_.size()-1; i++)
                spans_[i+1] = spans_[i];
    }
}
