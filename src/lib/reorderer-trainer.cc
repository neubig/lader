#include <lader/reorderer-trainer.h>
#include <boost/algorithm/string.hpp>
#include <lader/feature-data-sequence.h>
#include <time.h>
#include <cstdlib>
using namespace lader;
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
    bool loss_aug = config.GetBool("loss_augmented_inference");
    // Temporary values
    double model_score = 0, model_loss = 0, oracle_score = 0, oracle_loss = 0;
    FeatureVectorInt model_features, oracle_features;
    std::tr1::unordered_map<int,double> feat_map;
    vector<int> sent_order(data_.size());
    for(int i = 0 ; i < (int)sent_order.size(); i++)
        sent_order[i] = i;

	struct timespec build={0,0}, oracle={0,0}, model={0,0}, adjust={0,0};
	struct timespec tstart={0,0}, tend={0,0};
	HyperGraph graph(config.GetBool("cube_growing"));
	graph.SetThreads(config.GetInt("threads"));
	graph.SetBeamSize(config.GetInt("beam"));
	graph.SetPopLimit(config.GetInt("pop_limit"));
	graph.SetSaveFeatures(config.GetBool("save_features"));
	HyperGraph * ptr_graph = &graph;
    // Perform an iteration
    cerr << "(\".\" == 100 sentences)" << endl;
    for(int iter = 0; iter < config.GetInt("iterations"); iter++) {
        double iter_model_loss = 0, iter_oracle_loss = 0;
        // Shuffle
        if(config.GetBool("shuffle"))
            random_shuffle(sent_order.begin(), sent_order.end());
        // Over all values in the corpus
        int done = 0;
        BOOST_FOREACH(int sent, sent_order) {
//            // If we are saving features for efficiency, recover the saved
//            // features and replace them in the hypergraph
//            if(config.GetBool("save_features")){
//            	// reuse the saved graph
//				ptr_graph = saved_graphs_[sent];
//				// stack was cleared if the features are stored in a file
//            	if (!config.GetString("features_dir").empty()){
//            		ptr_graph->SetNumWords(data_[sent][0]->GetNumWords());
//            		ptr_graph->SetAllStacks();
//            		clock_gettime(CLOCK_MONOTONIC, &tstart);
//            		ifstream in(GetFeaturePath(config, sent).c_str());
//            		ptr_graph->FeaturesFromStream(in);
//            		in.close();
//            		clock_gettime(CLOCK_MONOTONIC, &tend);
//            		build.tv_sec += tend.tv_sec - tstart.tv_sec;
//            		build.tv_nsec += tend.tv_nsec - tstart.tv_nsec;
//            		if(verbose > 0)
//            			printf("FeaturesFromStream took about %.5f seconds\n",
//            					((double) ((tend.tv_sec))+ 1.0e-9 * tend.tv_nsec)
//            					- ((double) ((tstart.tv_sec)) + 1.0e-9 * tstart.tv_nsec));
//            	}
//            }
            clock_gettime(CLOCK_MONOTONIC, &tstart);
            // We want to find a derivation that minimize loss and maximize model score
            // Make the hypergraph using cube pruning/growing
			ptr_graph->BuildHyperGraph(*model_,
                                        *features_,
                                        data_[sent]);
			clock_gettime(CLOCK_MONOTONIC, &tend);
			build.tv_sec += tend.tv_sec - tstart.tv_sec;
			build.tv_nsec += tend.tv_nsec - tstart.tv_nsec;
			// Add losses to the hypotheses in the hypergraph
			BOOST_FOREACH(LossBase * loss, losses_)
				ptr_graph->AddLoss(loss,
					sent < (int)ranks_.size() ? &ranks_[sent] : NULL,
					sent < (int)parses_.size() ? &parses_[sent] : NULL);
			// Parse the hypergraph, penalizing loss heavily (oracle)
			clock_gettime(CLOCK_MONOTONIC, &tstart);
			oracle_score = ptr_graph->Rescore(*model_, -1e6);
			feat_map.clear();
			ptr_graph->AccumulateFeatures(feat_map, *model_, *features_,
							data_[sent],
							ptr_graph->GetRoot());
			oracle_features.clear();
			ClearAndSet(oracle_features, feat_map);
			oracle_loss = ptr_graph->AccumulateLoss(ptr_graph->GetRoot());
			oracle_score -= oracle_loss * -1e6;
			clock_gettime(CLOCK_MONOTONIC, &tend);
			oracle.tv_sec += tend.tv_sec - tstart.tv_sec;
			oracle.tv_nsec += tend.tv_nsec - tstart.tv_nsec;
			// Parse the hypergraph, slightly boosting loss by 1.0 if we are
			// using loss-augmented inference
			clock_gettime(CLOCK_MONOTONIC, &tstart);
			model_score = ptr_graph->Rescore(*model_, loss_aug ? 1.0 : 0.0);
			model_loss = ptr_graph->AccumulateLoss(ptr_graph->GetRoot());
			model_score -= model_loss * 1;
			feat_map.clear();
			ptr_graph->AccumulateFeatures(feat_map, *model_, *features_,
							data_[sent],
							ptr_graph->GetRoot());
			ClearAndSet(model_features, feat_map);
			clock_gettime(CLOCK_MONOTONIC, &tend);
			model.tv_sec += tend.tv_sec - tstart.tv_sec;
			model.tv_nsec += tend.tv_nsec - tstart.tv_nsec;
			// Add the statistics for this iteration
			iter_model_loss += model_loss;
			iter_oracle_loss += oracle_loss;
			if (verbose > 0){
				vector<int> order;
				ptr_graph->GetBest()->GetReordering(order, verbose > 1);
				for(int i = 0; i < (int)order.size(); i++) {
					if(i != 0) cout << " "; cout << order[i];
				}
				cout << endl;
				for(int i = 0; i < (int)order.size(); i++) {
					if(i != 0) cout << " "; cout << data_[sent][0]->GetElement(order[i]);
				}
				cout << endl;
				for(int i = 0; i < (int)order.size(); i++) {
					if(i != 0) cout << " "; cout << ranks_[sent][i];
				}
				cout << endl;
				// --- DEBUG: check both losses match ---
				double sent_loss = 0;
				BOOST_FOREACH(LossBase * loss, losses_)
					sent_loss += loss->CalculateSentenceLoss(order,
									sent < (int)ranks_.size() ? &ranks_[sent] : NULL,
									sent < (int)parses_.size() ? &parses_[sent] : NULL).first;
				if(sent_loss != model_loss){
					ostringstream out;
					vector<string> words = ((FeatureDataSequence*)data_[sent][0])->GetSequence();
		            ptr_graph->GetBest()->PrintParse(words, out);
					cerr << "sent=" << sent << " sent_loss="<< sent_loss <<" != model_loss="<<model_loss << endl;
		            cerr << out.str() << endl;
				}
				cout << "sent=" << sent << " oracle_score=" << oracle_score << " model_score=" << model_score << endl
					 << "sent_loss = "<< sent_loss << " oracle_loss=" << oracle_loss << " model_loss=" << model_loss << endl;
			}
			// Add the difference between the vectors if there is at least
			//  some loss
			clock_gettime(CLOCK_MONOTONIC, &tstart);
			if(config.GetString("learner") == "pegasos") {
				model_->AdjustWeightsPegasos(
						model_loss == oracle_loss ?
								FeatureVectorInt() :
								VectorSubtract(oracle_features, model_features));
			} else if(config.GetString("learner") == "perceptron") {
				if(model_loss != oracle_loss)
					model_->AdjustWeightsPerceptron(
							VectorSubtract(oracle_features, model_features));
			} else {
				THROW_ERROR("Bad learner: " << config.GetString("learner"));
			}

			clock_gettime(CLOCK_MONOTONIC, &tend);
			adjust.tv_sec += tend.tv_sec - tstart.tv_sec;
			adjust.tv_nsec += tend.tv_nsec - tstart.tv_nsec;
			// because features are stored in stack, clear stack if not saved in memory
			if(!config.GetBool("save_features") || !config.GetString("features_dir").empty())
            	ptr_graph->ClearStacks();
			done++;
            if(done % 100 == 0) cerr << ".";
            if(done % (100*10) == 0) cerr << done << endl;
        }
        cerr << "Finished iteration " << iter << endl;
        cout << "Finished iteration " << iter << " with loss " << iter_model_loss << " (oracle: " << iter_oracle_loss << ")" << endl;
        cout << "Running time on average: "
        		<< "build " << ((double)build.tv_sec + 1.0e-9*build.tv_nsec) / (iter+1) << "s"
        		<< ", oracle " << ((double)oracle.tv_sec + 1.0e-9*oracle.tv_nsec) / (iter+1) << "s"
        		<< ", model " << ((double)model.tv_sec + 1.0e-9*model.tv_nsec)  / (iter+1) << "s"
        		<< ", adjust " << ((double)adjust.tv_sec + 1.0e-9*adjust.tv_nsec)  / (iter+1) << "s" << endl;
        cout.flush();
        bool last_iter = (iter_model_loss == iter_oracle_loss ||
                          iter == config.GetInt("iterations") - 1);
        // write every iteration to a different file
        if(config.GetBool("write_every_iter")){
        	stringstream ss;
        	ss << ".it" << iter;
        	WriteModel(config.GetString("model_out") + ss.str());
        }
        if(last_iter) {
            WriteModel(config.GetString("model_out"));
            break;
        }
    }
}

void ReordererTrainer::InitializeModel(const ConfigTrainer & config) {
    srand(time(NULL));
    ofstream model_out(config.GetString("model_out").c_str());
    if(!model_out)
        THROW_ERROR("Must specify a valid model output with -model_out ('"
                        <<config.GetString("model_out")<<"')");
    // Load the model from  a file if it exists, otherwise note features
    if(config.GetString("model_in").length() != 0) {
        std::ifstream in(config.GetString("model_in").c_str());
        if(!in) THROW_ERROR("Couldn't read model from file (-model_in '"
                            << config.GetString("model_in") << "')");
        features_ = FeatureSet::FromStream(in);
        model_ = ReordererModel::FromStream(in);
    } else {
        model_ = new ReordererModel;
        model_->SetMaxTerm(config.GetInt("max_term"));
        model_->SetUseReverse(config.GetBool("use_reverse"));
        features_ = new FeatureSet;
        features_->ParseConfiguration(config.GetString("feature_profile"));
    }
    // Load the other config
    attach_ = config.GetString("attach_null") == "left" ? 
                CombinedAlign::ATTACH_NULL_LEFT :
                CombinedAlign::ATTACH_NULL_RIGHT;
    combine_ = config.GetBool("combine_blocks") ? 
                CombinedAlign::COMBINE_BLOCKS :
                CombinedAlign::LEAVE_BLOCKS_AS_IS;
    model_->SetCost(config.GetDouble("cost"));
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
