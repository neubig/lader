
#include <lader/reorderer-model.h>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace lader;
using namespace boost;

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

#define MINIMUM_WEIGHT 1e-10

// IO Functions
void ReordererModel::ToStream(std::ostream & out) {
    out << "reorderer_model" << std::endl;
    const vector<double> & weights = GetWeights();
    for(int i = 0; i < (int)weights.size(); i++)
        if(abs(weights[i]) > MINIMUM_WEIGHT)
            out << feature_ids_.GetSymbol(i) << "\t" << weights[i] << endl;
    out << endl;
}
ReordererModel * ReordererModel::FromStream(std::istream & in) {
    std::string line;
    GetlineEquals(in, "reorderer_model");
    ReordererModel * ret = new ReordererModel;
    while(std::getline(in, line) && line.length()) {
        vector<string> columns;
        algorithm::split(columns, line, is_any_of("\t"));
        if(columns.size() != 2)
            THROW_ERROR("Bad line in reordered model: " << line);
        double dub;
        std::istringstream iss(columns[1]);
        iss >> dub;
        ret->SetWeight(columns[0], dub);
    }
    return ret;
}
