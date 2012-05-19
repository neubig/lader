#include <lader/dictionary.h>
#include <boost/algorithm/string.hpp>
#include <lader/util.h>
#include <boost/algorithm/string/regex.hpp>

using namespace lader;
using namespace std;
using namespace boost;

void Dictionary::AddEntry(const string & str, const vector<double> & val) {
    vector<string> vals;
    algorithm::split(vals, str, is_any_of(" "));
    max_len_ = max((int)vals.size(), max_len_);
    dict_.insert(MakePair(str, val));
}

Dictionary * Dictionary::FromStream(istream & in) {
    string line;
    Dictionary * dict = new Dictionary;
    while(getline(in, line) && line.length()) {
        vector<string> cols;
        algorithm::split_regex(cols, line, regex(" \\|\\|\\| "));
        if(cols.size() < 2)
            THROW_ERROR("Bad line in dictionary or phrase table " << line);
        double val; vector<double> lev;
        istringstream iss(cols[cols.size()-1]);
        while(iss >> val)
            lev.push_back(val);
        dict->AddEntry(cols[0], lev);
    }
    return dict;
}

void Dictionary::ToStream(ostream & out) const {
    BOOST_FOREACH(const DictionaryPair & pair, dict_) {
        out << pair.first << " |||";
        BOOST_FOREACH(double val, pair.second)
            out << " " << val;
        out << endl;
    }
}
