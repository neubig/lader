#ifndef REORDERER_RUNNER_H__ 
#define REORDERER_RUNNER_H__

#include <iostream>
#include <fstream>
#include <kyldr/config-runner.h>
#include <kyldr/reorderer-model.h>
#include <kyldr/feature-set.h>
#include <kyldr/loss-base.h>

namespace kyldr {

// A class to train the reordering model
class ReordererRunner {
public:

    // Types of output to produce
    typedef enum {
        OUTPUT_STRING,
        OUTPUT_PARSE,
        OUTPUT_ORDER
    } OutputType;

    ReordererRunner() { }
    ~ReordererRunner() {
        if(model_) delete model_;
        if(features_) delete features_;
    }

    // Initialize the model
    void InitializeModel(const ConfigRunner & config);
    
    // Run the model
    void Run(const ConfigRunner & config);

    // Write the model to a file
    void ReadModel(const std::string & str) {
        std::ifstream in(str.c_str());
        if(!in) THROW_ERROR("Couldn't read model from file (-model_in '"
                            << str << "')");
        features_ = FeatureSet::FromStream(in);
        model_ = ReordererModel::FromStream(in);
    }

private:

    ReordererModel * model_; // The model
    FeatureSet * features_;  // The mapping on feature ids and which to use
    std::vector<OutputType> outputs_;

};

}

#endif

