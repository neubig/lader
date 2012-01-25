#ifndef FEATURE_SET_H__ 
#define FEATURE_SET_H__

#include <kyldr/feature-base.h>
#include <kyldr/symbol-set.h>

namespace kyldr {

// A class containing a set of features defined over various data types
// Can assign features to the nodes and edges of a hypergraph
class FeatureSet {
public:

    FeatureSet() : add_(true) { }
    ~FeatureSet() {
        BOOST_FOREACH(FeatureBase * gen, feature_gens_)
            if(gen)
                delete gen;
    }

    // Add a feature generator, and take control of it
    void AddFeatureGenerator(FeatureBase * gen) {
        feature_gens_.push_back(gen);
    }

    // Generates the features that can be factored over a node
    void AddNodeFeatures(const std::vector<FeatureDataBase*> & sent,
                         HyperNode & node) {
        FeatureVectorInt feats;
        for(int i = 0; i < (int)sent.size(); i++) {
            FeatureVectorString str_feats = 
                feature_gens_[i]->GenerateNodeFeatures(*sent[i], node);
            for(int j = 0; j < (int)str_feats.size(); j++)
                feats.push_back(
                    MakePair(feature_ids_.GetId(str_feats[j].first, add_),
                             str_feats[j].second));
        }
        sort(feats.begin(), feats.end());
        node.SetFeatureVector(feats);
    }

    // Generates the features that can be factored over a node
    void AddEdgeFeatures(const std::vector<FeatureDataBase*> & sent,
                         const HyperNode & node,
                         HyperEdge & edge) {
        FeatureVectorInt feats;
        for(int i = 0; i < (int)sent.size(); i++) {
            FeatureVectorString str_feats = 
                feature_gens_[i]->GenerateEdgeFeatures(*sent[i], node, edge);
            for(int j = 0; j < (int)str_feats.size(); j++)
                feats.push_back(
                    MakePair(feature_ids_.GetId(str_feats[j].first, add_),
                             str_feats[j].second));
        }
        sort(feats.begin(), feats.end());
        edge.SetFeatureVector(feats);
    }
    
    // Change an integer-indexed feature vector into a string-indexed vector
    FeatureVectorString StringifyFeatureIndices(const FeatureVectorInt & vec) {
        FeatureVectorString ret(vec.size());
        for(int i = 0; i < (int)vec.size(); i++)
            ret[i] = MakePair(feature_ids_.GetSymbol(vec[i].first),
                              vec[i].second);
        return ret;
    }

private:

    std::vector<FeatureBase*> feature_gens_; // Feature generators
    SymbolSet<string, int> feature_ids_; // The index of feature names and IDs
    bool add_; // Whether to allow the adding of new features

};

}

#endif

