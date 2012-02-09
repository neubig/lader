
#include <kyldr/combined-alignment.h>
#include <iostream>

using namespace kyldr;
using namespace std; 

void CombinedAlign::BuildFromAlignment(
        const Alignment & align,
        CombinedAlign::NullHandler null_hand,
        CombinedAlign::BlockHandler block_hand) {
    // Save the lengths
    trg_len_ = align.GetTrgLen();
    // Create the new alignment vector
    const vector<pair<int,int> > & vec = align.GetAlignmentVector();
    spans_ = vector<pair<int,int> >(align.GetSrcLen(),
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

    // If we want to combine blocks
    if(block_hand == COMBINE_BLOCKS) {
        // Sort the source values in ascending order of target alignment
        vector<pair<pair<int,int>, int> > word_spans(spans_.size());
        for(int i = 0; i < (int)spans_.size(); i++)
            word_spans[i] = MakePair(spans_[i], i);
        sort(word_spans.begin(), word_spans.end());
        // Step through in ascending order of target alignment, and combine
        // all spans with alignments that overlap at all
        int i = 0;
        while(i < (int)spans_.size()) {
            // Start a new span with a single source word
            int f1 = word_spans[i].second, f2 = f1,
                e1 = spans_[f1].first, e2 = spans_[f1].second;
            // Skip null aligned spans
            if(e1 == -1) { i++; continue;  }
            // Step to the next word and check if it overlaps with this span
            while(++i < (int)spans_.size() &&
                  spans_[word_spans[i].second].first <= e2) {
                // If it does, we need to expand the source span, considering
                // that this might increase the maximum value of the target.
                // The minimum is guaranteed to be OK, because we sorted.
                while(f1 > word_spans[i].second)
                    e2 = max(e2, spans_[--f1].second);
                while(f2 < word_spans[i].second)
                    e2 = max(e2, spans_[++f2].second);
            }
            // For all words from f1 to f2, set the new span to e1, e2
            while(f1 <= f2)
                spans_[f1++] = MakePair(e1,e2);
        }
    }

    // Attach any null alignments
    int i;
    if(null_hand == ATTACH_NULL_LEFT) {
        // Attach all words to the left
        for(i = 1; i < (int)spans_.size(); i++)
            if(spans_[i].first == -1)
                spans_[i] = spans_[i-1];
        // Attach trailing nulls to the right
        for(i = 0; i < (int)spans_.size() && spans_[i].first == -1; i++);
        if(i != (int)spans_.size()) // As long as all are not null
            for( ; i > 0; i--)
                spans_[i-1] = spans_[i];
    } else if(null_hand == ATTACH_NULL_RIGHT) {
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
