#ifndef FEATURE_DATA_SEQUENCE_H__ 
#define FEATURE_DATA_SEQUENCE_H__

#include <string>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <kyldr/feature-data-base.h>

namespace kyldr {

// A virtual class for the data from a single sentence that is used to calculate
// features. This stores the number of words in a standard format to make it
// easier to check to make sure that all sizes are equal
class FeatureDataSequence : public FeatureDataBase {
public:
    FeatureDataSequence() : FeatureDataBase() { }
    virtual ~FeatureDataSequence() { }

    void ParseInput(const std::string & str) {
        boost::tokenizer<boost::char_separator<char> > 
                                tokens(str, boost::char_separator<char>(" "));
        sequence_.clear();
        BOOST_FOREACH(std::string s, tokens) { sequence_.push_back(s); }
        num_words_ = sequence_.size();
    }

    const std::vector<std::string> & GetSequence() { return sequence_; }

private:

    std::vector<std::string> sequence_;

};

}

#endif

