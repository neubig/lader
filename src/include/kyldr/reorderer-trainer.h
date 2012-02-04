#ifndef REORDERER_TRAINER_H__ 
#define REORDERER_TRAINER_H__

#include <fstream>
#include <kyldr/config-trainer.h>
#include <kyldr/reorderer-model.h>
#include <kyldr/feature-set.h>
#include <kyldr/loss-base.h>

namespace kyldr {

// A class to train the reordering model
class ReordererTrainer {
public:

    ReordererTrainer() : learning_rate_(1), inner_iters_(10) { }
    ~ReordererTrainer() {
        BOOST_FOREACH(std::vector<FeatureDataBase*> vec, data_)
            BOOST_FOREACH(FeatureDataBase* ptr, vec)
                delete ptr;
        BOOST_FOREACH(LossBase * loss, losses_)
            delete loss;
        BOOST_FOREACH(EdgeFeatureMap * feat, saved_feats_) {
            BOOST_FOREACH(EdgeFeaturePair feat_pair, *feat)
                delete feat_pair.second;
            delete feat;
        }
    }

    // Initialize the model
    void InitializeModel(const ConfigTrainer & config);

    // Read in the data
    void ReadData(const std::string & source_in) {
        std::ifstream in(source_in.c_str());
        if(!in) THROW_ERROR("Could not open source file (-source_in): "
                                <<source_in);
        std::string line;
        data_.clear();
        while(getline(in, line))
            data_.push_back(features_.ParseInput(line));
    }

    // Read in the alignments
    void ReadAlignments(const std::string & align_in) {
        std::ifstream in(align_in.c_str());
        if(!in) THROW_ERROR("Could not open alignment file (-align_in): "
                                <<align_in);
        std::string line;
        while(getline(in, line))
            ranks_.push_back(
                Ranks(CombinedAlign(Alignment::FromString(line),
                              CombinedAlign::ATTACH_NULL_LEFT)));
    }

    // Write the model to a file
    void WriteModel(const std::string & str) {
        std::ofstream out(str.c_str());
        if(!out) THROW_ERROR("Couldn't write model to file " << str);
        features_.ToStream(out);
        model_.ToStream(out);
    }

    // Train the reorderer incrementally, building they hypergraph each time
    // we parse
    void TrainIncremental(const ConfigTrainer & config);

private:

    std::vector<Ranks> ranks_; // The alignments
    std::vector<std::vector<FeatureDataBase*> > data_; // The data
    ReordererModel model_; // The model
    FeatureSet features_;  // The mapping on feature ids and which to use
    std::vector<LossBase*> losses_; // The loss functions
    double learning_rate_; // The learning rate
    int inner_iters_; // The number of iterations of online learning to do for
                      // each construction of the hypergraph
    std::vector<EdgeFeatureMap*> saved_feats_; // Features for each hypergraph

};

}

#endif

