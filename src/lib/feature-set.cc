
#include <kyldr/feature-set.h>
#include <boost/algorithm/string.hpp>

using namespace kyldr;
using namespace std;
using namespace boost;

// Generates the features that can be factored over a node
void FeatureSet::AddNodeFeatures(const vector<FeatureDataBase*> & sent,
                                 HyperNode & node,
                                 bool add) {
    // No features are generated over root nodes
    if(node.IsRoot()) return;
    // Otherwise generate features
    FeatureVector feats;
    for(int i = 0; i < (int)sent.size(); i++)
        feature_gens_[i]->GenerateNodeFeatures(*sent[i], node, feats);
    node.SetFeatureVector(feats);
}

// Generates the features that can be factored over a node
void FeatureSet::AddEdgeFeatures(const vector<FeatureDataBase*> & sent,
                     const HyperNode & node,
                     HyperEdge & edge,
                     bool add) {
    // No features are generated over root nodes
    if(node.IsRoot()) return;
    // Otherwise generate the features
    FeatureVector feats;
    for(int i = 0; i < (int)sent.size(); i++)
        feature_gens_[i]->GenerateEdgeFeatures(*sent[i], node, edge, feats);
    edge.SetFeatureVector(feats);
}

// Add features to the entire hypergraph
void FeatureSet::AddHyperGraphFeatures(const vector<FeatureDataBase*> & sent,
                           HyperGraph & graph) {
    BOOST_FOREACH(HyperNode * node, graph.GetNodes()) {
        AddNodeFeatures(sent, *node);
        BOOST_FOREACH(HyperEdge * edge, node->GetEdges())
            AddEdgeFeatures(sent, *node, *edge);
    }
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
            THROW_ERROR("Bad configsiuration string " << str);
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
    out << endl;
}
FeatureSet * FeatureSet::FromStream(istream & in) {
    string line;
    GetlineEquals(in, "feature_set");
    string config;
    GetConfigLine(in, "config_str", config);
    FeatureSet * ret = new FeatureSet;
    ret->ParseConfiguration(config);
    return ret;
}
