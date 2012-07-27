#ifndef REORDERER_RUNNER_H__ 
#define REORDERER_RUNNER_H__

#include <iostream>
#include <fstream>
#include <lader/config-runner.h>
#include <lader/reorderer-model.h>
#include <lader/feature-set.h>
#include <lader/loss-base.h>
#include <lader/thread-pool.h>
#include <lader/output-collector.h>

namespace lader {

// A class to train the reordering model
class ReordererRunner {
public:

    // Types of output to produce
    typedef enum {
        OUTPUT_STRING,
        OUTPUT_PARSE,
        OUTPUT_HYPERGRAPH,
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

// A task
class ReordererTask : public Task {
public:
    ReordererTask(int id, const std::string & line,
                  ReordererModel * model, FeatureSet * features,
                  std::vector<ReordererRunner::OutputType> * outputs,
                  int beam, OutputCollector * collector) :
        id_(id), line_(line), model_(model), features_(features), 
        outputs_(outputs), beam_(beam), collector_(collector) { }
    void Run();
protected:
    int id_;
    std::string line_;
    ReordererModel * model_; // The model
    FeatureSet * features_;  // The mapping on feature ids and which to use
    std::vector<ReordererRunner::OutputType> * outputs_;
    int beam_;
    OutputCollector * collector_;
};

}

#endif

