#include <kyldr/feature-sequence.h>

#include <sstream>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace kyldr;
using namespace std;

bool FeatureSequence::FeatureTemplateIsLegal(const string & name) {
    if(name.length() < 2 || name.length() > 3)
        return false;
    if(name[0] != 'S' && name[0] != 'L' && name[0] != 'R' && 
       name[0] != 'E' && name[0] != 'C')
        return false;
    if(name[0] == 'E') {
        if(name[1] != 'T')
            return false;
    } else if(name[0] == 'C') {
        if(name[1] != 'D')
            return false;
    } else {
        if(name[1] != 'S' && name[1] != 'N' && name[1] != 'L' && name[1] != 'R')
            return false;
    }
    if(name.length() == 3 && name[2] != '#')
        return false;
    return true;
}

// Parse the comma-separated list of configuration options
void FeatureSequence::ParseConfiguration(const string & str) {
    feature_templates_.clear();
    // Iterate through all the comma-separated strings
    tokenizer<char_separator<char> > feats(str, char_separator<char>(","));
    BOOST_FOREACH(string feat, feats) {
        // First assume this is node-factored until we find something that
        // indicates that it shouldn't be
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
    data->ParseInput(str);
    return data;
}

string FeatureSequence::GetSpanFeatureString(const FeatureDataSequence & sent,
                                             int l, int r,
                                             char type) {
    ostringstream oss;
    switch (type) {
        case 'L':
            return sent.GetElement(l);
        case 'R':
            return sent.GetElement(r);
        case 'S':
            return sent.GetRangeString(l, r);
        case 'N':
            oss << r - l + 1;
            return oss.str();
        default:
            THROW_ERROR("Bad feature type " << type);
    }
    return "";
}


string FeatureSequence::GetEdgeFeatureString(const FeatureDataSequence & sent,
                                             const HyperEdge & edge,
                                             char type) {
    ostringstream oss;
    switch (type) {
        // Get the difference between values
        case 'D':
            // Distance is (r-c+1)-(c-l)
            oss << 
                abs(edge.GetRight()-2*edge.GetCenter()+edge.GetLeft()+1);
            return oss.str();
        case 'T':
            oss << (char)edge.GetType();
            return oss.str();
        default:
            THROW_ERROR("Bad feature type " << type);
    }
    return "";
}

// Generates the features that can be factored over an edge
FeatureVectorString FeatureSequence::GenerateEdgeFeatures(
                            const FeatureDataBase & sent,
                            const HyperEdge & edge) {
    const FeatureDataSequence & sent_seq = (const FeatureDataSequence &)sent;
    FeatureVectorString ret;
    bool is_nonterm = (edge.GetType() == HyperEdge::EDGE_INV || 
                       edge.GetType() == HyperEdge::EDGE_STR);
    // Iterate over each feature
    BOOST_FOREACH(FeatureTemplate templ, feature_templates_) {
        // Make sure that this feature is compatible with the edge
        if (templ.first == ALL_FACTORED || is_nonterm) {
            vector<string> values = templ.second;
            for(int i = 1; i < (int)values.size(); i++) {
                // Choose which span to use
                switch (values[i][0]) {
                    case 'S':
                        values[i] = GetSpanFeatureString(sent_seq,
                            edge.GetLeft(), edge.GetRight(), values[i][1]);
                        break;
                    case 'L':
                        values[i] = GetSpanFeatureString(sent_seq,
                            edge.GetLeft(), edge.GetCenter()-1, values[i][1]);
                        break;
                    case 'R':
                        values[i] = GetSpanFeatureString(sent_seq,
                            edge.GetCenter(), edge.GetRight(), values[i][1]);
                        break;
                    case 'E':
                    case 'C':
                        values[i] = GetEdgeFeatureString(
                                sent_seq, edge, values[i][1]);
                        break;
                    default:
                        THROW_ERROR("Bad feature template " << values[i]); 
                }
            }
            ret.push_back(MakePair(algorithm::join(values, "||"), 1));
        }
    }
    return ret;
}
