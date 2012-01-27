#ifndef ALIGNMENT_H__ 
#define ALIGNMENT_H__

#include <vector>
#include <kyldr/util.h>


namespace kyldr {

// A string of alignments for a particular sentence
class Alignment {
public:
    // Pair of words in an alignment
    typedef std::pair<int,int> AlignmentPair;

    // Constructor with lengths of the source and target sentence
    Alignment(const AlignmentPair & lens)
        : len_(lens) { }

    // Add a single alignment
    void AddAlignment(const AlignmentPair & al) {
#ifdef KYLDR_SAFE
        if(al.first >= len_.first || al.second >= len_.second)
            THROW_ERROR("Out of bounds in AddAlignment: "<< al << ", " << len_);
#endif
        vec_.push_back(al);
    }

    // Convert to and from strings
    std::string ToString() const;
    static Alignment FromString(const std::string & str);

    // Comparators
    bool operator== (const Alignment & rhs) {
        if(len_ != rhs.len_ ||
           vec_.size() != rhs.vec_.size())
            return false;
        for(int i = 0; i < (int)vec_.size(); i++)
            if(vec_[i] != rhs.vec_[i])
                return false;
        return true;
    }
    bool operator!= (const Alignment & rhs) {
        return !(*this == rhs);
    }

    // ------------- Accessors --------------
    const std::vector<AlignmentPair> & GetAlignmentVector() const {
        return vec_;
    }
    int GetSrcLen() const { return len_.first; }
    int GetTrgLen() const { return len_.second; }

private:

    // Split a string in the form X-Y into an alignment pair
    static AlignmentPair SplitAlignment(const std::string & str);

    AlignmentPair len_;
    std::vector<AlignmentPair > vec_;

};

}

#endif
