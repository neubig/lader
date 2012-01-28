#include <kyldr/reorderer-trainer.h>
#include <boost/algorithm/string.hpp>

using namespace kyldr;
using namespace boost;
using namespace std;

void ReordererTrainer::TrainIncremental(const ConfigTrainer & config) {
    InitializeModel(config);
    ReadData(config.GetString("source_in"));
    ReadAlignments(config.GetString("align_in"));
    // Temporary values
    double model_score = 0, model_loss = 0, oracle_score = 0, oracle_loss = 0;
    FeatureVector model_features, oracle_features;
    // Perform an iterations
    for(int iter = 0; iter < config.GetInt("iterations"); iter++) {
        double iter_model_loss = 0, iter_oracle_loss = 0;
        // Over all values in the corpus
        for(int sent = 0; sent < (int)data_.size(); sent++) {
            // Make the hypergraph
            HyperGraph hyper_graph(alignments_[sent].GetSrcLen(),
                                   alignments_[sent].GetTrgLen());
            hyper_graph.AddTerminals(alignments_[sent],
                                     config.GetInt("max_term"),
                                     config.GetBool("use_reverse"));
            hyper_graph.AddNonTerminals();
            // Annotate the hypergraph
            BOOST_FOREACH(LossBase * loss, losses_)
                loss->AddLossToHyperGraph(alignments_[sent], hyper_graph);
            features_.AddHyperGraphFeatures(data_[sent], hyper_graph);
            // Parse the hypergraph, penalizing loss heavily (oracle)
            model_.ScoreGraph(hyper_graph, -1e6);
            oracle_score = hyper_graph.Parse();
            oracle_loss = hyper_graph.GetRoot()->AccumulateLoss();
            oracle_score -= oracle_loss * -1e6;
            oracle_features = hyper_graph.GetRoot()->AccumulateFeatures();
            // Inner iterations
            for(int i = 0; i < inner_iters_; i++) {
                // Parse the hypergraph, slightly boosting loss by 1.0
                model_.ScoreGraph(hyper_graph, 1.0);
                model_score = hyper_graph.Parse();
                model_loss = hyper_graph.GetRoot()->AccumulateLoss();
                model_score -= model_loss * 1;
                if(i == 0) {
                    // Add the statistics for this iteration
                    iter_model_loss += model_loss;
                    iter_oracle_loss += oracle_loss;
                    cout << "sent=" <<sent <<
                            " oracle_score=" << oracle_score << 
                            " model_score=" << model_score << 
                            " oracle_loss=" << oracle_loss <<
                            " model_loss=" << model_loss << endl;
                }
                if(model_loss == oracle_loss) break;
                model_features = hyper_graph.GetRoot()->AccumulateFeatures();
                // Add the difference between the vectors
                model_.AdjustWeights(oracle_features, learning_rate_);
                model_.AdjustWeights(model_features, -learning_rate_);
            }
        }
        cout << "Finished iteration " << iter << " with loss " << iter_model_loss << " (oracle: " << iter_oracle_loss << ")" << endl;
        WriteModel(config.GetString("model_out"));
        if(iter_model_loss == iter_oracle_loss) 
            break;
    }
}

void ReordererTrainer::InitializeModel(const ConfigTrainer & config) {
    ofstream model_out(config.GetString("model_out").c_str());
    if(!model_out)
        THROW_ERROR("Must specify a valid model output with -model_out ('"
                        <<config.GetString("model_out")<<"')");
    features_.ParseConfiguration(config.GetString("feature_profile"));
    learning_rate_ = config.GetDouble("learning_rate");
    std::vector<std::string> losses, first_last;
    algorithm::split(
        losses, config.GetString("loss_profile"), is_any_of("|"));
    BOOST_FOREACH(string s, losses) {
        algorithm::split(first_last, s, is_any_of("="));
        if(first_last.size() != 2) THROW_ERROR("Bad loss: " << s);
        LossBase * loss = LossBase::CreateNew(first_last[0]);
        double dub;
        istringstream iss(first_last[1]);
        iss >> dub;
        loss->SetWeight(dub);
        losses_.push_back(loss);
    } 
}
