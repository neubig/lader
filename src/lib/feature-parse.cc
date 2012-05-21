#include <lader/feature-parse.h>

#include <sstream>
#include <cfloat>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace lader;
using namespace std;

bool FeatureParse::FeatureTemplateIsLegal(const string & name) {
    if(name.length() < 2)
        return false;
    // For edge values, the only type is 'T'
    if(name[0] == 'E') {
        return name.length() == 2 && name[1] == 'T';
    } else {
        // For span values, only type is production P
        if(name[1] == 'P') {
            return name.length() == 2;
        } else {
            return false;
        }
    }
}

// Parse the comma-separated list of configuration options
void FeatureParse::ParseConfiguration(const string & str) {
    feature_templates_.clear();
    // Iterate through all the comma-separated strings
    tokenizer<char_separator<char> > feats(str, char_separator<char>(","));
    BOOST_FOREACH(string feat, feats) {
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
                if(item[0] == 'L' || item[0] == 'R')
                    my_type = NONTERM_FACTORED;
            }
            my_name.push_back(item);
        }
        feature_templates_.push_back(MakePair(my_type, my_name));
    }
}

// Parses an input string of data (that is in the appropriate format for
// the feature generator) into the generator's internal representation
FeatureDataBase * FeatureParse::ParseData(const string & str) {
    FeatureDataParse * data = new FeatureDataParse;
    data->FromString(str);
    return data;
}

string FeatureParse::GetSpanFeatureString(const FeatureDataParse & sent,
                                             int l, int r,
                                             const string & str) {
    ostringstream oss;
    char type = str[1];
    switch (type) {
        case 'N':
            oss << l-r+1;
            return oss.str();
        case 'P':
            return sent.GetSpanLabel(l,r);
        default:
            THROW_ERROR("Bad span feature type " << type);
    }
    return "";
}

double FeatureParse::GetSpanFeatureValue(const FeatureDataParse & sent,
                                             int l, int r,
                                             const std::string & str) {
    char type = str[1];
    switch (type) {
        case 'N':
            return r - l + 1;
        default:
            THROW_ERROR("Bad span feature value " << type);
    }
    return -DBL_MAX;
}


string FeatureParse::GetEdgeFeatureString(const FeatureDataParse & sent,
                                             const HyperEdge & edge,
                                             const std::string & str) {
    char type = str[1];
    ostringstream oss;
    switch (type) {
        // Get the difference between values
        case 'D':
            // Distance is (r-c+1)-(c-l)
            oss << 
                abs(edge.GetRight()-2*edge.GetCenter()+edge.GetLeft()+1);
            return oss.str();
        case 'B':
            // Get the balance between the values
            oss << 
                edge.GetRight()-2*edge.GetCenter()+edge.GetLeft()+1;
            return oss.str();
        case 'T':
            oss << (char)edge.GetType();
            return oss.str();
        default:
            THROW_ERROR("Bad edge feature type " << type);
    }
    return "";
}

double FeatureParse::GetEdgeFeatureValue(const FeatureDataParse & sent,
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
void FeatureParse::GenerateEdgeFeatures(
                            const FeatureDataBase & sent,
                            const HyperEdge & edge,
                            SymbolSet<int> & feature_ids,
                            bool add,
                            FeatureVectorInt & feat) {
    const FeatureDataParse & sent_seq = (const FeatureDataParse &)sent;
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
                feat.push_back(
                    MakePair(
                        feature_ids.GetId(algorithm::join(values,"||"), add),
                                          feat_val));
        }
    }
}
