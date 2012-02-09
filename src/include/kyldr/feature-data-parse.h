#ifndef FEATURE_DATA_PARSE_H__ 
#define FEATURE_DATA_PARSE_H__

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/join.hpp>
#include <kyldr/feature-data-base.h>
#include <kyldr/util.h>

namespace kyldr {

// A virtual class for the data from a single sentence that is used to calculate
// features. This stores the number of words in a standard format to make it
// easier to check to make sure that all sizes are equal
class FeatureDataParse : public FeatureDataBase {
public:
    FeatureDataParse() : FeatureDataBase() { }
    virtual ~FeatureDataParse() { }

    void FromString(const std::string & str);
    std::string ToString() const {
        THROW_ERROR("FeatureDataParse::ToString not implemented");
        return "";
    }
    void Reorder(const std::vector<int> & order) {
        if(order.size() != sequence_.size())
            THROW_ERROR("Vector sizes in Reorder don't match");
        std::vector<std::string> old_seq = sequence_;
        for(int i = 0; i < (int) order.size(); i++)
            sequence_[i] = old_seq[order[i]];
    }

    const std::vector<std::string> & GetSequence() const { return sequence_; }
    const std::string & GetElement(int i) const {
        return SafeAccess(sequence_, i); 
    }
    
    // Return space-separate string representing a sequence
    const std::string GetRangeString(int i, int j, 
                                     std::string sep = " ") const {
        std::string ret = boost::algorithm::join(
            MakePair(sequence_.begin() + i, sequence_.begin() + j + 1),
            sep.c_str());
        return ret;
    }

    // Get the label of a particular span (default is X)
    const std::string GetSpanLabel(int l, int r) const {
        SpanMap::const_iterator it = spans_.find(MakePair(l,r));
        return it == spans_.end() ? "X" : it->second;
    }

private:

    std::vector<std::string> sequence_;
    typedef std::map<std::pair<int,int>, std::string> SpanMap;
    SpanMap spans_;

};

}

#endif

