#ifndef RANKS_H__ 
#define RANKS_H__

#include <vector>
#include <kyldr/combined-alignment.h>

namespace kyldr {

class Ranks {
public:
    Ranks() : max_rank_(-1) { };
    // Turn a combined alignment into its corresponding ranks
    Ranks(const CombinedAlign & combined);

    // Check whether two ranks are contiguous (IE same, or step one up)
    static bool IsContiguous(int l, int r) {
        return l == r || l+1 == r;
    }
    
    // Access the rank
    int operator[](int i) const { return SafeAccess(ranks_, i); }
    int GetMaxRank() const { return max_rank_; }
    const std::vector<int> & GetRanks() const { return ranks_; }
    int GetSrcLen() const { return ranks_.size(); }

private:
    std::vector<int> ranks_;
    int max_rank_;
};

}

#endif

