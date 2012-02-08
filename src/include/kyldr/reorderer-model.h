#ifndef REORDERER_MODEL_H__ 
#define REORDERER_MODEL_H__

#include <vector>
#include <kyldr/feature-set.h>
#include <kyldr/feature-vector.h>
#include <kyldr/hyper-graph.h>

namespace kyldr {

// A reorderer model that contains the weights and the feature set
class ReordererModel {
public:
    
    // Initialize the pegasos model
    ReordererModel() 
        : t_(1), alpha_(1), v_squared_norm_(0), lambda_(1e-5) { }

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

    // Perform one round of Pegasos
    void AdjustWeights(const FeatureVectorInt & feats);

    // IO Functions
    void ToStream(std::ostream & out);
    static ReordererModel * FromStream(std::istream & in);

    // Comparators
    bool operator==(const ReordererModel & rhs) {
        return v_ == rhs.v_;
    }

    // Accessors
    double GetWeight(int id) const {
        return id >= 0 && id < (int)v_.size() ? v_[id] * alpha_: 0;
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
    void SetWeights(const std::vector<double> & weights) { v_ = weights; }
    void SetCost(double cost) { lambda_ = cost; }

private:
    // Weights over features and weights over losses
    std::vector<double> v_;
    // Parameters for the pegasos algorithm
    int t_;
    // The scaling factor, squared norm, and cost used in learning Pegasos
    double alpha_, v_squared_norm_, lambda_;

};

}

#endif

