#ifndef REORDERER_MODEL_H__ 
#define REORDERER_MODEL_H__

#include <vector>
#include <kyldr/feature-set.h>
#include <kyldr/hyper-graph.h>

namespace kyldr {

// A reorderer model that contains the weights and the feature set
class ReordererModel {
public:
    
    // Calculate the scores of each single edge and node in a hypergraph
    //  The loss_factor indicates the multiplier of the loss term compared
    //  to the weight term (0 by default = don't consider loss in scoring)
    void ScoreGraph(HyperGraph & graph, double loss_factor = 0);

    // Calculate the score of one edge
    void ScoreEdge(HyperEdge & edge, double loss_factor = 0);

    // Calculate the score of one node
    void ScoreNode(HyperNode & node);

    // Adjust the weights
    void AdjustWeights(const FeatureVectorInt & feats, double weight);

    // IO Functions
    void ToStream(std::ostream & out);
    static ReordererModel * FromStream(std::istream & in);

    // Comparators
    bool operator==(const ReordererModel & rhs) {
        return weights_ == rhs.weights_;
    }

    // Accessors
    double GetWeight(int id) const {
        return id < (int)weights_.size() ? weights_[id] : 0;
    }
    const std::vector<double> & GetWeights() const { return weights_; }
    void SetWeights(const std::vector<double> & weights) { weights_ = weights; }

private:
    // Weights over features and weights over losses
    std::vector<double> weights_;

};

}

#endif

