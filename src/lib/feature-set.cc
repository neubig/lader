
#include <kyldr/feature-set.h>
#include <boost/algorithm/string.hpp>

using namespace kyldr;
using namespace std;
using namespace boost;


// Generates the features that can be factored over a node
FeatureVectorString * FeatureSet::MakeEdgeFeatures(
        const vector<FeatureDataBase*> & sent, const HyperEdge & edge) const {
    // Otherwise generate the features
    FeatureVectorString * feats = new FeatureVectorString;
    for(int i = 0; i < (int)sent.size(); i++)
        feature_gens_[i]->GenerateEdgeFeatures(*sent[i], edge, *feats);
    return feats;
}

vector<FeatureDataBase*> FeatureSet::ParseInput(const string & line) const {
    vector<string> columns;
    algorithm::split(columns, line, is_any_of("\t"));
    if(feature_gens_.size() != columns.size()) {
        THROW_ERROR("Number of columns ("<<columns.size()<<
                    ") didn't equal number of columns ("<<columns.size()<<
                    ")"<<endl<<line);
    }
    vector<FeatureDataBase*> ret(columns.size());
    for(int i = 0; i < (int)columns.size(); i++) {
        ret[i] = feature_gens_[i]->ParseData(columns[i]);
    }
    return ret;
}

void FeatureSet::ParseConfiguration(const string & str) {
    config_str_ = str;
    // Split configurations into sizes
    vector<string> configs;
    algorithm::split(configs, str, is_any_of("|"));
    feature_gens_ = vector<FeatureBase*>(configs.size());
    // Iterate over multiple pipe-separated config strings of format type=config
    for(int i = 0; i < (int)configs.size(); i++) {
        size_t pos = configs[i].find_first_of("=");
        if(pos == string::npos || pos == 0 || pos == configs[i].length()-1)
            THROW_ERROR("Bad configuration string " << str);
        string type = configs[i].substr(0, pos);
        feature_gens_[i] = FeatureBase::CreateNew(type);
        string sub_config = configs[i].substr(pos+1);
        feature_gens_[i]->ParseConfiguration(sub_config);
    }
}

// IO Functions
void FeatureSet::ToStream(ostream & out) {
    out << "feature_set" << endl;
    out << "config_str " << config_str_ << endl;
    out << "max_term " << max_term_ << endl;
    out << "use_reverse " << use_reverse_ << endl;
    out << endl;
}
FeatureSet * FeatureSet::FromStream(istream & in) {
    string line;
    GetlineEquals(in, "feature_set");
    string config;
    GetConfigLine(in, "config_str", config);
    FeatureSet * ret = new FeatureSet;
    ret->ParseConfiguration(config);
    GetConfigLine(in, "max_term", config);
    ret->SetMaxTerm(atoi(config.c_str()));
    GetConfigLine(in, "use_reverse", config);
    ret->SetUseReverse(config == "true" || config == "1");
    GetlineEquals(in, "");
    return ret;
}
