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
                         HyperNode & node,
                         bool add = false);

    // Generates the features that can be factored over a node
    void AddEdgeFeatures(const std::vector<FeatureDataBase*> & sent,
                         const HyperNode & node,
                         HyperEdge & edge,
                         bool add = false);

    // Add features to the entire hypergraph
    void AddHyperGraphFeatures(const std::vector<FeatureDataBase*> & sent,
                               HyperGraph & graph);

    // Parse a multi-factor input separated by tabs
    std::vector<FeatureDataBase*> ParseInput(const std::string & line) const;

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
    bool add_; // Whether to allow the adding of new features

};

}

#endif

