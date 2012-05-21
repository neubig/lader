#ifndef FEATURE_SET_H__ 
#define FEATURE_SET_H__

#include <lader/feature-data-base.h>
#include <lader/feature-base.h>
#include <lader/symbol-set.h>
#include <lader/hyper-graph.h>
#include <boost/foreach.hpp>

namespace lader {

// A class containing a set of features defined over various data types
// Can assign features to the nodes and edges of a hypergraph
class FeatureSet {
public:

    FeatureSet() : max_term_(0), use_reverse_(true) { }
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
    std::vector<FeatureDataBase*> ParseInput(const std::string & line) const;

    // Parse the configuration
    void ParseConfiguration(const std::string & str);

    // IO Functions
    void ToStream(std::ostream & out);
    static FeatureSet * FromStream(std::istream & in);

    // Comparators
    bool operator== (const FeatureSet & rhs) {
        return (config_str_ == rhs.config_str_ &&
                feature_gens_.size() == rhs.feature_gens_.size() &&
                // feature_ids_->size() == rhs.feature_ids_->size() &&
                max_term_ == rhs.max_term_);
    }

    // Accessors
    const FeatureBase* GetGenerator(int id) const { return feature_gens_[id]; }
    // const std::string & GetFeatureName(int id) const {
    //     return feature_ids_->GetSymbol(id);
    // }
    int GetMaxTerm() const { return max_term_; }
    bool GetUseReverse() const { return use_reverse_; }

    // void SetFeatureIds(SymbolSet<std::string,int>* feature_ids) {
    //     if(feature_ids_) delete feature_ids_;
    //     feature_ids_ = feature_ids;
    // }
    void SetMaxTerm(int max_term) { max_term_ = max_term; }
    void SetUseReverse(bool use_reverse) { use_reverse_ = use_reverse; }

private:

    std::string config_str_; // The configuration string
    std::vector<FeatureBase*> feature_gens_; // Feature generators
    int max_term_; // The maximum length of a terminal
    bool use_reverse_; // Reverse

};

}

#endif

