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
        double score = 0;
        // Add the losses
        if(loss_factor) {
            const std::vector<double> & loss = edge.GetLossVector();
            for(int i = 0; i < (int)min(loss.size(),loss_weights_.size()); i++)
                score += loss[i] * loss_weights_[i];
            score *= loss_factor;
        }
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

    // Accessors
    void SetWeights(const std::vector<double> & weights) { weights_ = weights; }
    void SetLossWeights(const std::vector<double> & weights) { 
        loss_weights_ = weights;
    }

private:
    // The set of feature functions
    FeatureSet feature_set_;
    // Weights over features and weights over losses
    std::vector<double> weights_, loss_weights_;

};

}

#endif

