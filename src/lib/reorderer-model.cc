
#include <kyldr/reorderer-model.h>

using namespace std;
using namespace kyldr;

// Calculate the scores of each single edge and node in a hypergraph
//  The loss_factor indicates the multiplier of the loss term compared
//  to the weight term (0 by default = don't consider loss in scoring)
void ReordererModel::ScoreGraph(HyperGraph & graph, double loss_factor) {
    BOOST_FOREACH(HyperEdge * edge, graph.GetEdges())
        ScoreEdge(*edge, loss_factor);
    BOOST_FOREACH(HyperNode * node, graph.GetNodes())
        ScoreNode(*node);
}

// Calculate the score of one edge
void ReordererModel::ScoreEdge(HyperEdge & edge, double loss_factor) {
    double score = loss_factor * edge.GetLoss();
    // Add the weights
    BOOST_FOREACH(FeaturePairInt val, edge.GetFeatureVector()) {
        if(val.first < (int)weights_.size())
            score += val.second * weights_[val.first];
    }
    edge.SetScore(score);
}

// Calculate the score of one node
void ReordererModel::ScoreNode(HyperNode & node) {
    double score = 0;
    // Add the weights
    BOOST_FOREACH(FeaturePairInt val, node.GetFeatureVector()) {
        if(val.first < (int)weights_.size())
            score += val.second * weights_[val.first];
    }
    node.SetScore(score);
}

// Adjust the weights
void ReordererModel::AdjustWeights(const FeatureVectorInt & feats, double weight) {
    BOOST_FOREACH(FeaturePairInt feat, feats) {
        if((int)weights_.size() <= feat.first)
            weights_.resize(feat.first+1, 0);
        weights_[feat.first] += feat.second * weight;
    }
}

// IO Functions
void ReordererModel::ToStream(std::ostream & out) {
    out << "reorderer_model" << std::endl;
    BOOST_FOREACH(double weight, weights_)
        out << weight << std::endl;
    out << std::endl;
}
ReordererModel * ReordererModel::FromStream(std::istream & in) {
    std::string line;
    GetlineEquals(in, "reorderer_model");
    std::vector<double> weights;
    ReordererModel * ret = new ReordererModel;
    while(std::getline(in, line) && line.length()) {
        double dub;
        std::istringstream iss(line);
        iss >> dub;
        ret->weights_.push_back(dub);
    }
    return ret;
}
