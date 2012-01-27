#include <kyldr/reorderer-trainer.h>

using namespace kyldr;
using namespace boost;
using namespace std;

void ReordererTrainer::TrainIncremental(const ConfigTrainer & config) {
    InitializeModel(config);
    ReadData(config.GetString("source_in"));
    ReadAlignments(config.GetString("align_in"));
    // Temporary values
    double model_score, model_loss, oracle_score, oracle_loss;
    FeatureVectorInt model_features, oracle_features;
    // Perform an iterations
    for(int iter = 0; iter < config.GetInt("iterations"); iter++) {
        double iter_loss = 0;
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
            // Parse the hypergraph, slightly boosting loss by 1.0
            model_.ScoreGraph(hyper_graph, 1.0);
            model_score = hyper_graph.Parse();
            model_loss = hyper_graph.GetRoot()->AccumulateLoss();
            model_score -= model_loss * 1;
            model_features = hyper_graph.GetRoot()->AccumulateFeatures();
            // Parse the hypergraph, penalizing loss heavily (oracle)
            model_.ScoreGraph(hyper_graph, -1e6);
            oracle_score = hyper_graph.Parse();
            oracle_loss = hyper_graph.GetRoot()->AccumulateLoss();
            oracle_score -= oracle_loss * -1e6;
            oracle_features = hyper_graph.GetRoot()->AccumulateFeatures();
            // Add the difference between the vectors
            model_.AdjustWeights(
                VectorSubtract(oracle_features, model_features),
                learning_rate_);
            // Add the statistics for this iteration
            iter_loss += model_loss;
            cout << "oracle_score=" << oracle_score << " model_score="<<model_score<< " oracle_loss="<<oracle_loss<<" model_loss="<<model_loss << endl;
        }
        cout << endl << "Finished iteration " << iter << " with loss " << iter_loss << endl;
    }
}
