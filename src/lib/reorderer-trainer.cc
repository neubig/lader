#include <kyldr/reorderer-trainer.h>
#include <boost/algorithm/string.hpp>

using namespace kyldr;
using namespace boost;
using namespace std;

void ReordererTrainer::TrainIncremental(const ConfigTrainer & config) {
    InitializeModel(config);
    ReadData(config.GetString("source_in"));
    if(config.GetString("align_in").length())
        ReadAlignments(config.GetString("align_in"));
    if(config.GetString("parse_in").length())
        ReadParses(config.GetString("parse_in"));
    int verbose = config.GetInt("verbose");
    // Temporary values
    double model_score = 0, model_loss = 0, oracle_score = 0, oracle_loss = 0;
    FeatureVectorInt model_features, oracle_features;
    vector<int> sent_order(data_.size());
    for(int i = 0 ; i < (int)sent_order.size(); i++)
        sent_order[i] = i;
    // Perform an iteration
    for(int iter = 0; iter < config.GetInt("iterations"); iter++) {
        double iter_model_loss = 0, iter_oracle_loss = 0;
        // Shuffle
        if(config.GetBool("shuffle"))
            random_shuffle(sent_order.begin(), sent_order.end());
        // Over all values in the corpus
        int done = 0;
        BOOST_FOREACH(int sent, sent_order) {
            if(++done % 1000 == 0) { cout << "."; cout.flush(); }
            HyperGraph hyper_graph;
            // If we are saving features for efficiency, recover the saved
            // features and replace them in the hypergraph
            if(config.GetBool("save_features") && iter != 0)
                hyper_graph.SetFeatures(SafeAccess(saved_feats_, sent));
            // Make the hypergraph using cube pruning
            hyper_graph.BuildHyperGraph(model_,
                                        features_,
                                        data_[sent],
                                        config.GetInt("beam"),
                                        true);
            // Add losses to the hypotheses in thehypergraph
            BOOST_FOREACH(LossBase * loss, losses_)
                loss->AddLossToHyperGraph(
                    sent < (int)ranks_.size() ? &ranks_[sent] : NULL,
                    sent < (int)parses_.size() ? &parses_[sent] : NULL,
                    hyper_graph);
            // Parse the hypergraph, penalizing loss heavily (oracle)
            oracle_score = hyper_graph.Rescore(model_, -1e6);
            oracle_features = hyper_graph.AccumulateFeatures(
                                                    hyper_graph.GetRoot());
            oracle_loss     = hyper_graph.AccumulateLoss(
                                                    hyper_graph.GetRoot());
            oracle_score   -= oracle_loss * -1e6;
            // Parse the hypergraph, slightly boosting loss by 1.0
            model_score = hyper_graph.Rescore(model_, 1.0);
            model_loss  = hyper_graph.AccumulateLoss(
                                                hyper_graph.GetRoot());
            model_score -= model_loss * 1;
            // Add the statistics for this iteration
            iter_model_loss += model_loss;
            iter_oracle_loss += oracle_loss;
            // // --- DEBUG: Get the reordering ---
            // vector<int> order;
            // hyper_graph.GetRoot()->GetReordering(order);
            // for(int i = 0; i < (int)order.size(); i++) {
            //     if(i != 0) cout << " "; cout << order[i];
            // }
            // cout << endl;
            // // --- DEBUG: check both losses match ---
            // pair<double,double> sent_loss =
            //    losses_[0]->CalculateSentenceLoss(order, ranks_[sent]);
            // if(sent_loss.first != model_loss)
            //     THROW_ERROR("sent_loss="<<sent_loss
            //                 <<", model_loss="<<model_loss);
            // // --- END DEBUG ---
            if(verbose > 0) {
                cout << "sent=" <<sent <<
                        " oracle_score=" << oracle_score << 
                        " model_score=" << model_score << 
                        " oracle_loss=" << oracle_loss <<
                        " model_loss=" << model_loss << endl;
            }
            model_features = hyper_graph.AccumulateFeatures(
                                                hyper_graph.GetRoot());
            // Add the difference between the vectors if there is at least
            //  some loss
            model_.AdjustWeights(
                model_loss == oracle_loss ?
                FeatureVectorInt() :
                VectorSubtract(oracle_features, model_features));
            // If we are saving features
            if(config.GetBool("save_features")) {
                if((int)saved_feats_.size() <= sent)
                    saved_feats_.resize(sent+1);
                saved_feats_[sent] = hyper_graph.GetFeatures();
                hyper_graph.ClearFeatures();
            }
        }
        cout << "Finished iteration " << iter << " with loss " << iter_model_loss << " (oracle: " << iter_oracle_loss << ")" << endl;
        WriteModel(config.GetString("model_out"));
        if(iter_model_loss == iter_oracle_loss) 
            break;
    }
}

void ReordererTrainer::InitializeModel(const ConfigTrainer & config) {
    srand(time(NULL));
    ofstream model_out(config.GetString("model_out").c_str());
    if(!model_out)
        THROW_ERROR("Must specify a valid model output with -model_out ('"
                        <<config.GetString("model_out")<<"')");
    attach_ = config.GetString("attach_null") == "left" ? 
                CombinedAlign::ATTACH_NULL_LEFT :
                CombinedAlign::ATTACH_NULL_RIGHT;
    combine_ = config.GetBool("combine_blocks") ? 
                CombinedAlign::COMBINE_BLOCKS :
                CombinedAlign::LEAVE_BLOCKS_AS_IS;
    features_.ParseConfiguration(config.GetString("feature_profile"));
    features_.SetMaxTerm(config.GetInt("max_term"));
    features_.SetUseReverse(config.GetBool("use_reverse"));
    model_.SetCost(config.GetDouble("cost"));
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

void ReordererTrainer::ReadAlignments(const std::string & align_in) {
    std::ifstream in(align_in.c_str());
    if(!in) THROW_ERROR("Could not open alignment file (-align_in): "
                            <<align_in);
    std::string line;
    int i = 0;
    while(getline(in, line))
        ranks_.push_back(
            Ranks(CombinedAlign(SafeAccess(data_,i++)[0]->GetSequence(),
                                Alignment::FromString(line),
                                attach_, combine_, bracket_)));
}

void ReordererTrainer::ReadParses(const std::string & parse_in) {
    std::ifstream in(parse_in.c_str());
    if(!in) THROW_ERROR("Could not open parse file (-parse_in): " << parse_in);
    std::string line;
    while(getline(in, line)) {
        parses_.push_back(FeatureDataParse());
        parses_.rbegin()->FromString(line);
    }
}
