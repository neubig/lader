#include <kyldr/ranks.h>
#include <kyldr/util.h>
#include <boost/foreach.hpp>
#include <map>

using namespace std;
using namespace kyldr;

// Find the ranks of each 
Ranks::Ranks(const CombinedAlign & combined) {
    // Sort the combined alignments in rank order (allowing ties)
    typedef pair<pair<int,int>, vector<int> > RankPair;
    typedef map<pair<int,int>, vector<int>, AlignmentIsLesser> RankMap;
    RankMap rank_map;
    for(int i = 0; i < (int)combined.GetSrcLen(); i++) {
        RankMap::iterator it = rank_map.find(combined[i]);
        if(it == rank_map.end()) {
            rank_map.insert(MakePair(combined[i], vector<int>(1,i)));
        } else {
            it->second.push_back(i);
        }
    }
    // Build the ranks
    ranks_.resize(combined.GetSrcLen());
    max_rank_ = -1;
    BOOST_FOREACH(RankPair rp, rank_map) {
        ++max_rank_;
        BOOST_FOREACH(int i, rp.second)
            ranks_[i] = max_rank_;
    }
}
