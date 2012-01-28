#ifndef REORDERER_MODEL_H__ 
#define REORDERER_MODEL_H__

#include <vector>
#include <kyldr/feature-set.h>
#include <kyldr/hyper-graph.h>

namespace kyldr {

// A reorderer model that contains the weights and the feature set
class ReordererModel {
public:
    
    typedef std::pair<std::string,double> WeightPair;
    typedef std::tr1::unordered_map<std::string,double> WeightMap;
    
    // Calculate the scores of each single edge and node in a hypergraph
    //  The loss_factor indicates the multiplier of the loss term compared
    //  to the weight term (0 by default = don't consider loss in scoring)
    void ScoreGraph(HyperGraph & graph, double loss_factor = 0);

    // Calculate the score of one edge
    void ScoreEdge(HyperEdge & edge, double loss_factor = 0);

    // Calculate the score of one node
    void ScoreNode(HyperNode & node);

    // Adjust the weights
    void AdjustWeights(const FeatureVector & feats, double weight);

    // IO Functions
    void ToStream(std::ostream & out);
    static ReordererModel * FromStream(std::istream & in);

    // Comparators
    bool operator==(const ReordererModel & rhs) {
        return weights_ == rhs.weights_;
    }

    // Accessors
    double GetWeight(std::string id) const {
        WeightMap::const_iterator wit = weights_.find(id);        
        return wit != weights_.end() ? wit->second : 0;
    }
    const WeightMap & GetWeights() const { return weights_; }
    void SetWeights(const WeightMap & weights) { weights_ = weights; }

private:
    // Weights over features and weights over losses
    WeightMap weights_;

};

}

#endif

