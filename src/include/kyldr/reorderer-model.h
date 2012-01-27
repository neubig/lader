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
    void ScoreGraph(HyperGraph & graph, double loss_factor = 0) {
        BOOST_FOREACH(HyperEdge * edge, graph.GetEdges())
            ScoreEdge(*edge, loss_factor);
        BOOST_FOREACH(HyperNode * node, graph.GetNodes())
            ScoreNode(*node);
    }

    // Calculate the score of one edge
    void ScoreEdge(HyperEdge & edge, double loss_factor = 0) {
        double score = loss_factor * edge.GetLoss();
        // Add the weights
        BOOST_FOREACH(FeaturePairInt val, edge.GetFeatureVector()) {
            if(val.first < (int)weights_.size())
                score += val.second * weights_[val.first];
        }
        edge.SetScore(score);
    }

    // Calculate the score of one node
    void ScoreNode(HyperNode & node) {
        double score = 0;
        // Add the weights
        BOOST_FOREACH(FeaturePairInt val, node.GetFeatureVector()) {
            if(val.first < (int)weights_.size())
                score += val.second * weights_[val.first];
        }
        node.SetScore(score);
    }

    // Adjust the weights
    void AdjustWeights(const FeatureVectorInt & feats, double weight) {
        BOOST_FOREACH(FeaturePairInt feat, feats) {
            if((int)weights_.size() <= feat.first)
                weights_.resize(feat.first+1, 0);
            weights_[feat.first] += feat.second * weight;
        }
    }

    // Accessors
    const std::vector<double> & GetWeights() { return weights_; }
    void SetWeights(const std::vector<double> & weights) { weights_ = weights; }

private:
    // Weights over features and weights over losses
    std::vector<double> weights_;

};

}

#endif

