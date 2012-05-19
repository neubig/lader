#ifndef FEATURE_DATA_SEQUENCE_H__ 
#define FEATURE_DATA_SEQUENCE_H__

#include <string>
#include <vector>
#include <iostream>
#include <climits>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/join.hpp>
#include <lader/feature-data-base.h>
#include <lader/util.h>

namespace lader {

// A virtual class for the data from a single sentence that is used to calculate
// features. This stores the number of words in a standard format to make it
// easier to check to make sure that all sizes are equal
class FeatureDataSequence : public FeatureDataBase {
public:
    FeatureDataSequence() : FeatureDataBase() { }
    virtual ~FeatureDataSequence() { }

    void FromString(const std::string & str) {
        boost::tokenizer<boost::char_separator<char> > 
                                tokens(str, boost::char_separator<char>(" "));
        sequence_.clear();
        BOOST_FOREACH(std::string s, tokens) { sequence_.push_back(s); }
    }
    std::string ToString() const {
        return boost::algorithm::join(sequence_, " ");
    }
    void Reorder(const std::vector<int> & order) {
        if(order.size() != sequence_.size())
            THROW_ERROR("Vector sizes in Reorder don't match");
        std::vector<std::string> old_seq = sequence_;
        for(int i = 0; i < (int) order.size(); i++)
            sequence_[i] = old_seq[order[i]];
    }
    
    // Return space-separate string representing a sequence
    const std::string GetRangeString(int i, int j, 
                                     int max_len = INT_MAX,
                                     std::string sep = " ") const {
        if(j-i >= max_len) return "";
        std::string ret = boost::algorithm::join(
            MakePair(sequence_.begin() + i, sequence_.begin() + j + 1),
            sep.c_str());
        return ret;
    }

};

}

#endif

