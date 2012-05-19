#ifndef FEATURE_DATA_ALIGN_H__ 
#define FEATURE_DATA_ALIGN_H__

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <lader/feature-data-base.h>
#include <lader/combined-alignment.h>
#include <lader/loss-tau.h>
#include <lader/ranks.h>
#include <lader/util.h>

namespace lader {

// A virtual class for the data from a single sentence that is used to calculate
// features. This stores the number of words in a standard format to make it
// easier to check to make sure that all sizes are equal
class FeatureDataAlign : public FeatureDataBase {
public:
    FeatureDataAlign() : FeatureDataBase(), ranks_(NULL) { }
    virtual ~FeatureDataAlign() {
        delete ranks_;
    }

    void FromString(const std::string & str) {
        // Get the source file
        std::vector<std::string> cols, strs;
        boost::algorithm::split_regex(cols, str, boost::regex(" \\|\\|\\| "));
        if(cols.size() != 3)
            THROW_ERROR("Align data requires three columns: " << str);
        boost::algorithm::split_regex(strs, cols[0], boost::regex(" "));
        // Get the ranks
        ranks_ = new Ranks(CombinedAlign(
                               strs,
                               Alignment::FromString(cols[1]+" ||| "+cols[2]), 
                               attach_,
                               CombinedAlign::COMBINE_BLOCKS,
                               CombinedAlign::ALIGN_BRACKET_SPANS));
    }
    std::string ToString() const {
        THROW_ERROR("FeatureDataAlign::ToString not implemented");
        return "";
    }
    void Reorder(const std::vector<int> & order) {
        THROW_ERROR("FeatureDataAlign::Reorder not implemented");
    }
    void SetAttach(CombinedAlign::NullHandler attach) {
        attach_ = attach;
    }
    const Ranks * GetRanks() const { return ranks_; }

private:

    Ranks * ranks_;
    CombinedAlign::NullHandler attach_;

};

}

#endif

