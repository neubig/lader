#include <kyldr/reorderer-runner.h>
#include <kyldr/feature-data-sequence.h>

using namespace kyldr;
using namespace std;

// Run the model
void ReordererRunner::Run(const ConfigRunner & config) {
    InitializeModel(config);
    std::string line;
    while(std::getline(std::cin, line)) {
        // Load the data
        std::vector<FeatureDataBase*> datas = features_->ParseInput(line);
        // Build the hypergraph
        HyperGraph hyper_graph;
        hyper_graph.BuildHyperGraph(*model_, *features_, datas, 
                                    config.GetInt("beam"));
        // Reorder, get a string representation, and clean up
        if(config.GetString("out_format") == "string") {
            // Get the reordering
            std::vector<int> reordering;
            hyper_graph.GetRoot()->GetReordering(reordering);
            datas[0]->Reorder(reordering);
            cout << datas[0]->ToString() << endl;
        } else if(config.GetString("out_format") == "parse") {
            const vector<string> & words = 
                ((FeatureDataSequence*)datas[0])->GetSequence();
            hyper_graph.GetRoot()->PrintParse(words, cout);
            cout << endl;
        } else {
            THROW_ERROR("Unknown output format "
                            <<config.GetString("out_format"));
        }
        // Clean up the data
        BOOST_FOREACH(FeatureDataBase* data, datas)
            delete data;
    } 
}
