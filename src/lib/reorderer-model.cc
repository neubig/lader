
#include <kyldr/reorderer-model.h>

using namespace std;
using namespace kyldr;

// If the norm gets larger than this, explicitly scale to prevent overflow
#define OVERFLOW_BOUNDARY 1e100

// Adjust the v according to the algorithm
//   Pegasos: primal estimated sub gradient solver for SVM
//   (Figure 1)
void ReordererModel::AdjustWeights(const FeatureVectorInt & feats) {
    // Set the learning rate nu given the cost lambda and the iteration t
    double nu = 1.0/lambda_/t_;
    // w_t -> w_{t+1/2}:
    //  1) Adjust the scaling factor alpha for the weight vector
    alpha_ = (t_ == 1) ? 1.0 : (1.0 - nu*lambda_)*alpha_;
    //  2) Adjust v with the new values
    BOOST_FOREACH(FeaturePairInt feat, feats) {
        if((int)v_.size() <= feat.first)
            v_.resize(feat.first+1, 0);
        // Save the old weight and update
        double & v = v_[feat.first], old_v = v;
        v += feat.second * nu / alpha_;
        // Update the squared norm
        v_squared_norm_ += v*v - old_v*old_v;
    }
    // w_{t+1/2} -> w_{t+1}
    //  Adjust the weights to ensure that they stay within the ball 1/lambda
    double w_squared_norm = v_squared_norm_ * alpha_ * alpha_;
    if(w_squared_norm > 1.0/lambda_)
        alpha_ *= sqrt(1.0/lambda_/w_squared_norm);
    // If we are in danger of overflow, re-normalize
    if(v_squared_norm_ > OVERFLOW_BOUNDARY)
        GetWeights();
    // cerr << "alpha_==" << alpha_<<", v_squared_norm_="<<v_squared_norm_<< endl;
    // Increase the iteration
    t_++;
}

// IO Functions
void ReordererModel::ToStream(std::ostream & out) {
    out << "reorderer_model" << std::endl;
    BOOST_FOREACH(double weight, GetWeights())
        out << weight << std::endl;
    out << std::endl;
}
ReordererModel * ReordererModel::FromStream(std::istream & in) {
    std::string line;
    GetlineEquals(in, "reorderer_model");
    std::vector<double> v;
    ReordererModel * ret = new ReordererModel;
    while(std::getline(in, line) && line.length()) {
        double dub;
        std::istringstream iss(line);
        iss >> dub;
        ret->v_.push_back(dub);
    }
    return ret;
}
