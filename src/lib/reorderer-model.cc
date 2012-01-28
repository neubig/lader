
#include <kyldr/reorderer-model.h>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace kyldr;
using namespace boost;

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
    BOOST_FOREACH(FeatureTuple val, edge.GetFeatureVector()) {
        WeightMap::const_iterator wit = weights_.find(val.name);
        if(wit != weights_.end())
            score += val.value * wit->second;
    }
    edge.SetScore(score);
}

// Calculate the score of one node
void ReordererModel::ScoreNode(HyperNode & node) {
    double score = 0;
    // Add the weights
    BOOST_FOREACH(FeatureTuple val, node.GetFeatureVector()) {
        WeightMap::const_iterator wit = weights_.find(val.name);
        if(wit != weights_.end())
            score += val.value * wit->second;
    }
    node.SetScore(score);
}

// Adjust the weights
void ReordererModel::AdjustWeights(const FeatureVector & feats, double weight) {
    BOOST_FOREACH(FeatureTuple feat, feats)
        weights_[feat.name] += feat.value * weight;
}

// IO Functions
void ReordererModel::ToStream(std::ostream & out) {
    out << "reorderer_model" << std::endl;
    BOOST_FOREACH(WeightPair weight, weights_)
        out << weight.first << "\t" << weight.second << std::endl;
    out << std::endl;
}
ReordererModel * ReordererModel::FromStream(std::istream & in) {
    std::string line;
    GetlineEquals(in, "reorderer_model");
    WeightMap weights;
    ReordererModel * ret = new ReordererModel;
    while(std::getline(in, line) && line.length()) {
        vector<string> name_weight;
        algorithm::split(name_weight, line, is_any_of("\t"));
        if(name_weight.size() != 2)
            THROW_ERROR("Bad line in reorderer model '" << line << "'");
        double dub;
        std::istringstream iss(name_weight[1]);
        iss >> dub;
        ret->weights_.insert(MakePair(name_weight[0], dub));
    }
    return ret;
}
