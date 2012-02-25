#include <kyldr/feature-sequence.h>

#include <sstream>
#include <cfloat>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace kyldr;
using namespace std;

bool FeatureSequence::FeatureTemplateIsLegal(const string & name) {
    if(name.length() < 2)
        return false;
    // For edge values, the only type is 'T'
    if(name[0] == 'E') {
        return name[1] == 'T';
    // For comparison values, difference, balance, or longer side
    } else if(name[0] == 'C') {
        return name[1] == 'D' || name[1] == 'B' || name[1] == 'L';
    } else {
        // For sequence matcher Q, check to make sure that the type
        //  name[2] = E (existance indicator) or # (feature)
        //  name[3] = [0-9] (for the number of the dictionary)
        //  name[4] = [0-9] (for the number of the feature if #)
        if(name[1] == 'Q') {
            return ((name.length() == 5 && name[2] == '#' 
                            && IsDigit(name[3]) && IsDigit(name[4])) || 
                   (name.length() == 4 && name[2] == 'E' && IsDigit(name[3])));
        // For other span values
        } else if(name[1] == 'S') {
            return name.length() == 2 || 
                (name.length() == 3 && 
                    (name[2] == '#' || (name[2] >= '0' && name[2] <= '9')));
        // For random values
        } else if(name[1] == 'M') {
            return name.length() == 3 && name[2] == '#';
        // For other values
        } else if (name[1] == 'N' || name[1] == 'L' || name[1] == 'R'
                                  || name[1] == 'B' || name[1] == 'A') {
            return name.length() == 2 || (name.length() == 3 && name[2] == '#');
        } else {
            return false;
        }
    }
}

// Parse the comma-separated list of configuration options
void FeatureSequence::ParseConfiguration(const string & str) {
    feature_templates_.clear();
    // Iterate through all the comma-separated strings
    tokenizer<char_separator<char> > feats(str, char_separator<char>(","));
    BOOST_FOREACH(string feat, feats) {
        // First check if this is specifying a special option
        if(!feat.compare(0, 5, "dict=")) {
            ifstream in(feat.substr(5).c_str());
            if(!in)
                THROW_ERROR("Could not find dictionary: " << feat.substr(5).c_str());
            dicts_.push_back(Dictionary::FromStream(in));
            continue;
        }
        // Otherwise assume a feature template
        FeatureType my_type = ALL_FACTORED;
        vector<string> my_name;
        // Iterate through all the percent-separated features (first is name)
        tokenizer<char_separator<char> > 
                        items(feat, char_separator<char>("%"));
        BOOST_FOREACH(string item, items) {
            if(my_name.size()) {
                if(!FeatureTemplateIsLegal(item))
                    THROW_ERROR("Illegal feature template "<<
                                item<<" in "<<str);
                // If left span, right span, or comparison are necessary,
                // this feature is only applicable to non-terminals
                if(item[0] == 'L' || item[0] == 'R' || item[0] == 'C')
                    my_type = NONTERM_FACTORED;
            }
            my_name.push_back(item);
        }
        feature_templates_.push_back(MakePair(my_type, my_name));
    }
}

// Parses an input string of data (that is in the appropriate format for
// the feature generator) into the generator's internal representation
FeatureDataBase * FeatureSequence::ParseData(const string & str) {
    FeatureDataSequence * data = new FeatureDataSequence;
    data->FromString(str);
    return data;
}

string FeatureSequence::GetSpanFeatureString(const FeatureDataSequence & sent,
                                             int l, int r,
                                             const string & str) {
    ostringstream oss;
    char type = str[1];
    switch (type) {
        case 'L':
            return sent.GetElement(l);
        case 'R':
            return sent.GetElement(r);
        case 'B':
            return l == 0 ? "<s>" : sent.GetElement(l-1);
        case 'A':
            return r == sent.GetNumWords() - 1 ? "<s>" : sent.GetElement(r+1);
        case 'S':
            return sent.GetRangeString(l, r, (str.length() == 3 ? str[2]-'0' : INT_MAX));
        case 'N':
            oss << r - l + 1;
            return oss.str();
        case 'Q':
            return SafeAccess(dicts_, 
                        str[3]-'0')->Exists(sent.GetRangeString(l, r)) ?
                    "+" : "-";
        default:
            THROW_ERROR("Bad span feature type " << type);
    }
    return "";
}

double FeatureSequence::GetSpanFeatureValue(const FeatureDataSequence & sent,
                                             int l, int r,
                                             const std::string & str) {
    char type = str[1];
    switch (type) {
        case 'N':
            return r - l + 1;
        case 'M':
            return rand()/(double)RAND_MAX;
        case 'Q':
            return SafeAccess(dicts_,str[3]-'0')->GetFeature(
                        sent.GetRangeString(l, r), str[4]-'0');
        default:
            THROW_ERROR("Bad span feature value " << type);
    }
    return -DBL_MAX;
}


string FeatureSequence::GetEdgeFeatureString(const FeatureDataSequence & sent,
                                             const HyperEdge & edge,
                                             const std::string & str) {
    char type = str[1];
    ostringstream oss;
    int bal;
    switch (type) {
        // Get the difference between values
        case 'D':
            // Distance is (r-c+1)-(c-l)
            oss << 
                abs(edge.GetRight()-2*edge.GetCenter()+edge.GetLeft()+1);
            return oss.str();
        case 'B':
        case 'L':
            // Get the balance between the values
            bal = edge.GetRight()-2*edge.GetCenter()+edge.GetLeft()+1;
            if(type == 'B') { oss << bal; return oss.str(); }
            else if(bal < 0) { return "L"; }
            else if(bal > 0) { return "R"; }
            else { return "E"; }
        case 'T':
            oss << (char)edge.GetType();
            return oss.str();
        default:
            THROW_ERROR("Bad edge feature type " << type);
    }
    return "";
}

double FeatureSequence::GetEdgeFeatureValue(const FeatureDataSequence & sent,
                                             const HyperEdge & edge,
                                             const std::string & str) {
    char type = str[1];
    switch (type) {
        // Get the difference between values
        case 'D':
            // Distance is (r-c+1)-(c-l)
            return abs(edge.GetRight()-2*edge.GetCenter()+edge.GetLeft()+1);
        case 'B':
            // Get the balance between the values
            return edge.GetRight()-2*edge.GetCenter()+edge.GetLeft()+1;
        default:
            THROW_ERROR("Bad edge feature value " << type);
    }
    return -DBL_MAX;
}

// Generates the features that can be factored over an edge
void FeatureSequence::GenerateEdgeFeatures(
                            const FeatureDataBase & sent,
                            const HyperEdge & edge,
                            FeatureVectorString & feat) {
    const FeatureDataSequence & sent_seq = (const FeatureDataSequence &)sent;
    bool is_nonterm = (edge.GetType() == HyperEdge::EDGE_INV || 
                       edge.GetType() == HyperEdge::EDGE_STR);
    // Iterate over each feature
    BOOST_FOREACH(FeatureTemplate templ, feature_templates_) {
        // Make sure that this feature is compatible with the edge
        if (templ.first == ALL_FACTORED || is_nonterm) {
            vector<string> values(1,templ.second[0]);
            double feat_val = 1;
            for(int i = 1; i < (int)templ.second.size(); i++) {
                // Choose which span to use
                pair<int,int> span(-1, -1);
                switch (templ.second[i][0]) {
                    case 'S':
                        span = pair<int,int>(edge.GetLeft(), edge.GetRight());
                        break;
                    case 'L':
                        span = pair<int,int>(edge.GetLeft(),edge.GetCenter()-1);
                        break;
                    case 'R':
                        span = pair<int,int>(edge.GetCenter(), edge.GetRight());
                        break;
                }
                if(templ.second[i].length() >= 3 && templ.second[i][2] == '#') {
                    feat_val = (span.first == -1 ?
                        GetEdgeFeatureValue(sent_seq, edge,templ.second[i]) :
                        GetSpanFeatureValue(sent_seq, span.first, span.second, 
                                                        templ.second[i]));
                } else {
                    values.push_back(span.first == -1 ?
                        GetEdgeFeatureString(sent_seq, edge,templ.second[i]):
                        GetSpanFeatureString(sent_seq, span.first, span.second, 
                                                        templ.second[i]));
                }
            }
            if(feat_val)
                feat.push_back(MakePair(algorithm::join(values,"||"),feat_val));
        }
    }
}
