#ifndef FEATURE_SET_H__ 
#define FEATURE_SET_H__

#include <lader/feature-data-base.h>
#include <lader/feature-base.h>
#include <lader/symbol-set.h>
#include <boost/foreach.hpp>

namespace lader {

// A class containing a set of features defined over various data types
// Can assign features to the nodes and edges of a hypergraph
class FeatureSet {
public:

    FeatureSet() { }
    ~FeatureSet() {
        BOOST_FOREACH(FeatureBase * gen, feature_gens_)
            if(gen)
                delete gen;
        // if(feature_ids_)
        //     delete feature_ids_;
    }

    // Add a feature generator, and take control of it
    void AddFeatureGenerator(FeatureBase * gen) {
        feature_gens_.push_back(gen);
    }

    // Generates the features that can be factored over a node
    FeatureVectorInt * MakeEdgeFeatures(
        const Sentence & sent,
        const HyperEdge & edge,
        SymbolSet<int> & feature_ids,
        bool add_features) const;
    
    // Change an integer-indexed feature vector into a string-indexed vector
    FeatureVectorString StringifyFeatureIndices(const FeatureVectorInt & vec);

    // Parse a multi-factor input separated by tabs
    Sentence ParseInput(const std::string & line) const;

    // Parse the configuration
    void ParseConfiguration(const std::string & str);

    // IO Functions
    void ToStream(std::ostream & out);
    static FeatureSet * FromStream(std::istream & in);

    // Comparators
    bool operator== (const FeatureSet & rhs) {
        return (config_str_ == rhs.config_str_ &&
                feature_gens_.size() == rhs.feature_gens_.size());
    }

    // Accessors
    const FeatureBase* GetGenerator(int id) const { return feature_gens_[id]; }

private:

    std::string config_str_; // The configuration string
    std::vector<FeatureBase*> feature_gens_; // Feature generators
};

}

#endif

