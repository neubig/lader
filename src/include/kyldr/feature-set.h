#ifndef FEATURE_SET_H__ 
#define FEATURE_SET_H__

#include <kyldr/feature-base.h>
#include <kyldr/symbol-set.h>
#include <kyldr/hyper-graph.h>

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
                         HyperNode & node);

    // Generates the features that can be factored over a node
    void AddEdgeFeatures(const std::vector<FeatureDataBase*> & sent,
                         const HyperNode & node,
                         HyperEdge & edge);

    // Add features to the entire hypergraph
    void AddHyperGraphFeatures(const std::vector<FeatureDataBase*> & sent,
                               HyperGraph & graph);
    
    // Change an integer-indexed feature vector into a string-indexed vector
    FeatureVectorString StringifyFeatureIndices(const FeatureVectorInt & vec);

    // Parse a multi-factor input separated by tabs
    std::vector<FeatureDataBase*> ParseInput(const std::string & line) const;

    // Parse the configuration
    void ParseConfiguration(const std::string & str);

    // Accessors
    const FeatureBase* GetGenerator(int id) const { return feature_gens_[id]; }
    const std::string & GetFeatureName(int id) const {
        return feature_ids_.GetSymbol(id);
    }

private:

    std::vector<FeatureBase*> feature_gens_; // Feature generators
    SymbolSet<std::string,int> feature_ids_; // Feature names and IDs
    bool add_; // Whether to allow the adding of new features

};

}

#endif

