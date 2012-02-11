#ifndef KYLDR_DICTIONARY_H__
#define KYLDR_DICTIONARY_H__

#include <iostream>
#include <vector>
#include <string>
#include <boost/foreach.hpp>
#include <kyldr/util.h>

namespace kyldr {

// Create a dictionary
//  The dictionary can be input from a stream that takes the form of a Moses
//  phrase table. i.e.
//  src ||| target ||| scores
// The target is actually optional, and this will treat the first column as
// the source and the last column as the scores no matter how many columns
// there are
class Dictionary {

public:
    Dictionary() : max_len_(0) { }    
    ~Dictionary() { }

    // Input output functions
    //  FromStream will overwrite entries with identical source strings,
    //  so it is recommended to only use files that have single source strings
    static Dictionary * FromStream(std::istream & in);
    void ToStream(std::ostream & out) const;

    // Find whether a string exists in the dictionary
    bool Exists(const std::string & str) {
        DictionaryData::const_iterator it = dict_.find(str);
        return it != dict_.end();        
    }

    // Find whether a string exists in the dictionary
    double GetFeature(const std::string & str, int idx) {
        DictionaryData::const_iterator it = dict_.find(str);
        return it == dict_.end() ? 0.0 : SafeAccess(it->second, idx);
    }

    // Add an entry to the dictionary
    void AddEntry(const std::string & str, const std::vector<double> & val);


    // Comparators
    bool operator ==(const Dictionary & rhs) const {
        // This is only a simple check for now
        if(dict_.size() != rhs.dict_.size() ||
           max_len_ != rhs.max_len_) return 0;
        BOOST_FOREACH(const DictionaryPair & val, rhs.dict_) {
            DictionaryData::const_iterator it = dict_.find(val.first);
            if(it == dict_.end() || it->second != val.second)
                return 0;
        }
        return 1;
    }

    bool operator !=(const Dictionary & rhs) const { return !(*this == rhs); }

private:
    typedef StringMap<std::vector<double> > DictionaryData;
    typedef std::pair<std::string,std::vector<double> > DictionaryPair;
    DictionaryData dict_;
    int max_len_;

};

}

#endif

