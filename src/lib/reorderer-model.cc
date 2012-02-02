
#include <kyldr/reorderer-model.h>

using namespace std;
using namespace kyldr;

// Adjust the weights
void ReordererModel::AdjustWeights(const FeatureVectorInt & feats,
                                   double weight) {
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
