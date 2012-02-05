            }
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
    ofstream model_out(config.GetString("model_out").c_str());
    if(!model_out)
        THROW_ERROR("Must specify a valid model output with -model_out ('"
                        <<config.GetString("model_out")<<"')");
    features_.ParseConfiguration(config.GetString("feature_profile"));
    features_.SetMaxTerm(config.GetInt("max_term"));
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
