#include <kyldr/reorderer-runner.h>
#include <kyldr/feature-data-sequence.h>
#include <boost/tokenizer.hpp>

using namespace kyldr;
using namespace std;
using namespace boost;

// Run the model
void ReordererRunner::Run(const ConfigRunner & config) {
    InitializeModel(config);
    std::string line;
    while(std::getline(std::cin, line)) {
        // Load the data
        std::vector<FeatureDataBase*> datas = features_->ParseInput(line);
        // Save the original string
        vector<string> words = ((FeatureDataSequence*)datas[0])->GetSequence();
        // Build the hypergraph
        HyperGraph hyper_graph;
        hyper_graph.BuildHyperGraph(*model_, *features_, datas, 
                                    config.GetInt("beam"));
        // Reorder
        std::vector<int> reordering;
        hyper_graph.GetRoot()->GetReordering(reordering);
        datas[0]->Reorder(reordering);
        // Print the string
        for(int i = 0; i < (int)outputs_.size(); i++) {
            if(i != 0) cout << "\t";
            if(outputs_[i] == OUTPUT_STRING) {
                cout << datas[0]->ToString();
            } else if(outputs_[i] == OUTPUT_PARSE) {
                hyper_graph.GetRoot()->PrintParse(words, cout);
            } else if(outputs_[i] == OUTPUT_ORDER) {
                for(int j = 0; j < (int)reordering.size(); j++) {
                    if(j != 0) cout << " ";
                    cout << reordering[j];
                }
            } else {
                THROW_ERROR("Unimplemented output format");
            }
        }
        cout << endl;
        // Clean up the data
        BOOST_FOREACH(FeatureDataBase* data, datas)
            delete data;
    } 
}

// Initialize the model
void ReordererRunner::InitializeModel(const ConfigRunner & config) {
    ReadModel(config.GetString("model"));
    features_->SetAdd(false);
    tokenizer<char_separator<char> > outs(config.GetString("out_format"),
                                          char_separator<char>(","));
    BOOST_FOREACH(const string & str, outs) {
        if(str == "string")
            outputs_.push_back(OUTPUT_STRING);
        else if(str == "parse")
            outputs_.push_back(OUTPUT_PARSE);
        else if(str == "order")
            outputs_.push_back(OUTPUT_ORDER);
        else
            THROW_ERROR("Bad output format '" << str <<"'");
    }
}
