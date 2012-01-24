#ifndef ALIGNMENT_H__ 
#define ALIGNMENT_H__

#include <vector>
#include <kyldr/util.h>

namespace kyldr {

// A string of alignments for a particular sentence
class Alignment {
public:
    // Constructor with lengths of the source and target sentence
    Alignment(int src_len, int trg_len) : src_len_(src_len), trg_len_(trg_len) { }

    // Add a single alignment
    void AddAlignment(int src, int trg) {
#ifdef KYLDR_SAFE
        if(src >= src_len_ || trg >= trg_len_)
            THROW_ERROR("Out of bounds in AddAlignment");
#endif
        vec_.push_back(MakePair(src,trg));
    }

    // ------------- Accessors --------------
    const std::vector<std::pair<int,int> > & GetAlignmentVector() const {
        return vec_;
    }
    int GetSrcLen() const { return src_len_; }
    int GetTrgLen() const { return trg_len_; }

private:

    std::vector<std::pair<int,int> > vec_;
    int src_len_, trg_len_;

};

}

#endif
