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
#include <lader/hyper-graph.h>
using namespace std;

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

    ReordererRunner(): model_(NULL), features_(NULL){ }
    ~ReordererRunner() {
        if(model_) delete model_;
        if(features_) delete features_;
    }

    // Initialize the model
    void InitializeModel(const ConfigRunner & config);
    
    // Run the model
    void Run(const ConfigRunner & config);

    // Write the model to a file
    void ReadModel(const string & str) {
        ifstream in(str.c_str());
        if(!in) THROW_ERROR("Couldn't read model from file (-model_in '"
                            << str << "')");
        features_ = FeatureSet::FromStream(in);
        model_ = ReordererModel::FromStream(in);
    }

private:

    ReordererModel * model_; // The model
    FeatureSet * features_;  // The mapping on feature ids and which to use
    vector<OutputType> outputs_;

};

// A task
class ReordererTask : public Task {
public:
    ReordererTask(int id, const string & line,
                  ReordererModel * model, FeatureSet * features,
                  vector<ReordererRunner::OutputType> * outputs,
                  const ConfigRunner& config, HyperGraph * hyper_graph,
                  OutputCollector * collector) :
        id_(id), line_(line), model_(model), features_(features), 
        outputs_(outputs), collector_(collector), config_(config), graph_(hyper_graph) { }
    void Run();
protected:
    int id_;
    string line_;
    ReordererModel * model_; // The model
    FeatureSet * features_;  // The mapping on feature ids and which to use
    vector<ReordererRunner::OutputType> * outputs_;
    OutputCollector * collector_;
    const ConfigRunner& config_;
    HyperGraph * graph_;
};

}

#endif

