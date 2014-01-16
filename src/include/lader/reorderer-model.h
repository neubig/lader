#ifndef REORDERER_MODEL_H__ 
#define REORDERER_MODEL_H__

#include <vector>
#include <lader/feature-set.h>
#include <lader/feature-vector.h>

namespace lader {

// A reorderer model that contains the weights and the feature set
class ReordererModel {
	class HyperGraph;
public:
    
    // Initialize the pegasos model
    ReordererModel() 
        : t_(1), alpha_(1), v_squared_norm_(0), lambda_(1e-5), add_features_(true),
          max_term_(0), use_reverse_(true) { }

    // Calculate the scores of each single edge and node in a hypergraph
    //  The loss_factor indicates the multiplier of the loss term compared
    //  to the weight term (0 by default = don't consider loss in scoring)
    void ScoreGraph(HyperGraph & graph, double loss_factor = 0);

    // Calculate the score of a feature vector
    double ScoreFeatureVector(const FeatureVectorInt & vec) const {
        double ret = 0;
        BOOST_FOREACH(const FeaturePairInt & fpi, vec)
            ret += GetWeight(fpi.first) * fpi.second;
        return ret;
    }

    // Perform one round of Pegasos or the Perceptron
    void AdjustWeightsPegasos(const FeatureVectorInt & feats);
    void AdjustWeightsPerceptron(const FeatureVectorInt & feats);

    // Get the feature IDs
    SymbolSet<int> & GetFeatureIds() {
        return feature_ids_;
    }
    const SymbolSet<int> & GetFeatureIds() const {
        return feature_ids_;
    }

    // Get an indexed feature vector from a string feature vector
    FeatureVectorInt * IndexFeatureVector(const FeatureVectorString & str) {
        FeatureVectorInt * ret = new FeatureVectorInt(str.size());
        for(int i = 0; i < (int)str.size(); i++)
            (*ret)[i] = MakePair(feature_ids_.GetId(str[i].first,add_features_),
                                 str[i].second);
        return ret;
    }
    // Get the strings for a feature vector index
    FeatureVectorString *StringifyFeatureVector(const FeatureVectorInt & str){
        FeatureVectorString * ret = new FeatureVectorString(str.size());
        for(int i = 0; i < (int)str.size(); i++)
            (*ret)[i] = MakePair(feature_ids_.GetSymbol(str[i].first),
                                 str[i].second);
        return ret;
    }

    // IO Functions
    void ToStream(std::ostream & out);
    static ReordererModel * FromStream(std::istream & in);

    // Comparators
    bool operator==(const ReordererModel & rhs) const {
        BOOST_FOREACH(const std::string * str, feature_ids_.GetSymbols())
            if(GetWeight(*str) != rhs.GetWeight(*str))
                return false;
        BOOST_FOREACH(const std::string * str, rhs.feature_ids_.GetSymbols())
            if(GetWeight(*str) != rhs.GetWeight(*str))
                return false;
        return max_term_ == rhs.max_term_ && use_reverse_ == rhs.use_reverse_;
    }
    bool operator!=(const ReordererModel & rhs) const {
        return !(*this == rhs);
    }

    // Accessors
    double GetWeight(int id) const {
        return id >= 0 && id < (int)v_.size() ? v_[id] * alpha_: 0;
    }
    double GetWeight(const std::string & str) const {
        return GetWeight(feature_ids_.GetId(str));
    }
    // Get the weight array
    const std::vector<double> & GetWeights() { 
        // Update the values for the entire array
        if(alpha_ != 1) {
            BOOST_FOREACH(double & val, v_)
                val *= alpha_;
            v_squared_norm_ *= alpha_*alpha_;
            alpha_ = 1;
        }
        return v_;
    }
    // Set the weight appropriately
    void SetWeight(const std::string & name, double w) {
        int idx = feature_ids_.GetId(name, true);
        if((int)v_.size() <= idx)
            v_.resize(idx+1,0.0);
        double old_val = v_[idx];
        v_[idx] = w/alpha_;
        v_squared_norm_ += v_[idx]*v_[idx] - old_val*old_val;
    }
    void SetCost(double cost) { lambda_ = cost; }
    void SetAdd(bool add) { add_features_ = add; }
    bool GetAdd() const { return add_features_; }

    int GetMaxTerm() const { return max_term_; }
    bool GetUseReverse() const { return use_reverse_; }

    void SetMaxTerm(int max_term) { max_term_ = max_term; }
    void SetUseReverse(bool use_reverse) { use_reverse_ = use_reverse; }

private:
    // Weights over features and weights over losses
    std::vector<double> v_;
    // Parameters for the pegasos algorithm
    int t_;
    // The scaling factor, squared norm, and cost used in learning Pegasos
    double alpha_, v_squared_norm_, lambda_;
    // Feature name values
    SymbolSet<int> feature_ids_; // Feature names and IDs
    bool add_features_; // Whether to allow the adding of new features
    int max_term_; // The maximum length of a terminal
    bool use_reverse_; // Reverse
};

}

#endif

