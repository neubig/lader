#include <lader/reorderer-runner.h>
#include <lader/feature-data-sequence.h>
#include <lader/thread-pool.h>
#include <lader/output-collector.h>
#include <boost/tokenizer.hpp>
#include <lader/hyper-graph.h>

using namespace lader;
using namespace std;
using namespace boost;

void ReordererTask::Run() {
    int verbose = config_.GetInt("verbose");
    // Load the data
    Sentence datas = features_->ParseInput(line_);
    // Save the original string
    vector<string> words = ((FeatureDataSequence*)datas[0])->GetSequence();
    // Build the hypergraph
    graph_->SetAllStacks(datas[0]->GetNumWords());
    graph_->BuildHyperGraph(*model_, *features_, datas);
    // Reorder
    std::vector<int> reordering;
    graph_->GetBest()->GetReordering(reordering);
    datas[0]->Reorder(reordering);
    // Print the string
    ostringstream oss;
    for(int i = 0; i < (int)outputs_->size(); i++) {
        if(i != 0) oss << "\t";
        if(outputs_->at(i) == ReordererRunner::OUTPUT_STRING) {
            oss << datas[0]->ToString();
        } else if(outputs_->at(i) == ReordererRunner::OUTPUT_PARSE) {
            graph_->GetBest()->PrintParse(words, oss);
        } else if(outputs_->at(i) == ReordererRunner::OUTPUT_HYPERGRAPH) {
            graph_->PrintHyperGraph(words, oss);
        } else if(outputs_->at(i) == ReordererRunner::OUTPUT_ORDER) {
            for(int j = 0; j < (int)reordering.size(); j++) {
                if(j != 0) oss << " ";
                oss << reordering[j];
            }
        } else {
            THROW_ERROR("Unimplemented output format");
        }
    }
    oss << endl;
    collector_->Write(id_, oss.str(), "");
    // Clean up the data
    BOOST_FOREACH(FeatureDataBase* data, datas)
        delete data;
    delete graph_;
}

// Run the model
void ReordererRunner::Run(const ConfigRunner & config) {
    InitializeModel(config);
    // Create the thread pool
    ThreadPool pool(config.GetInt("threads"), 1000);
    OutputCollector collector;

    std::string line;
    std::string source_in = config.GetString("source_in");
    std::ifstream sin(source_in.c_str());
    if (!sin)
    	cerr << "use stdin for source_in" << endl;
    int id = 0;
    HyperGraph graph(config.GetBool("cube_growing")) ;
    graph.SetBeamSize(config.GetInt("beam"));
	graph.SetPopLimit(config.GetInt("pop_limit"));
	// do not need to set threads because it runs in parallel at sentence-level
    while(std::getline(sin != NULL? sin : std::cin, line)) {
    	ReordererTask *task = new ReordererTask(id++, line, model_, features_,
    		 	&outputs_, config, graph.Clone(),
				&collector);
		pool.Submit(task);
    }
    if (sin) sin.close();
    pool.Stop(true); 
}

// Initialize the model
void ReordererRunner::InitializeModel(const ConfigRunner & config) {
	std::ifstream in(config.GetString("model").c_str());
	if(!in) THROW_ERROR("Couldn't read model from file (-model '"
						<< config.GetString("model") << "')");
	features_ = FeatureSet::FromStream(in);
	model_ = ReordererModel::FromStream(in);
    model_->SetAdd(false);
    tokenizer<char_separator<char> > outs(config.GetString("out_format"),
                                          char_separator<char>(","));
    BOOST_FOREACH(const string & str, outs) {
        if(str == "string")
            outputs_.push_back(OUTPUT_STRING);
        else if(str == "parse")
            outputs_.push_back(OUTPUT_PARSE);
        else if(str == "hypergraph")
            outputs_.push_back(OUTPUT_HYPERGRAPH);
        else if(str == "order")
            outputs_.push_back(OUTPUT_ORDER);
        else
            THROW_ERROR("Bad output format '" << str <<"'");
    }
}
