#include <kyldr/reorderer-evaluator.h>
#include <kyldr/feature-data-sequence.h>
#include <kyldr/loss-fuzzy.h>
#include <kyldr/loss-tau.h>
#include <kyldr/ranks.h>
#include <boost/algorithm/string.hpp>
#include <sstream>

using namespace kyldr;
using namespace std;
using namespace boost;

// Run the evaluator
void ReordererEvaluator::Evaluate(const ConfigEvaluator & config) {
    // Set the attachment handler
    attach_ = config.GetString("attach_null") == "left"
        ? CombinedAlign::ATTACH_NULL_LEFT : CombinedAlign::ATTACH_NULL_RIGHT;
    combine_ = config.GetBool("combine_blocks") ? 
                CombinedAlign::COMBINE_BLOCKS :
                CombinedAlign::LEAVE_BLOCKS_AS_IS;
    // Set up the losses
    vector<LossBase*> losses;
    losses.push_back(new LossFuzzy());
    losses.push_back(new LossTau());
    // Set up the pairs to store the counts
    vector<pair<double,double> > sums(losses.size(), pair<double,double>(0,0));
    // Open the files
    const vector<string> & args = config.GetMainArgs();
    ifstream aligns_in(SafeAccess(args, 0).c_str());
    if(!aligns_in) THROW_ERROR("Couldn't find alignment file " << args[0]);
    ifstream data_in(SafeAccess(args, 1).c_str());
    if(!data_in) THROW_ERROR("Couldn't find input file " << args[1]);
    ifstream *src_in = NULL, *trg_in = NULL;
    if(args.size() > 2) {
        src_in = new ifstream(SafeAccess(args, 2).c_str());
        if(!(*src_in)) THROW_ERROR("Couldn't find source file " << args[2]);
    }
    if(args.size() > 3) {
        trg_in = new ifstream(SafeAccess(args, 3).c_str());
        if(!*trg_in) THROW_ERROR("Couldn't find source file " << args[3]);
    }
    string data, align, src, trg;
    // Read them one-by-one and run the evaluator
    while(getline(data_in, data) && getline(aligns_in, align)) {
        // Get the input values
        std::vector<string> datas;
        algorithm::split(datas, data, is_any_of("\t"));
        istringstream iss(datas[0]);
        std::vector<int> order; int ival;
        while(iss >> ival) order.push_back(ival);
        // Get the source file
        getline(*src_in, src);
        vector<string> srcs;
        algorithm::split(srcs, src, is_any_of(" "));
        // Get the ranks
        Ranks ranks = Ranks(CombinedAlign(srcs,
                                          Alignment::FromString(align), 
                                          attach_, combine_, bracket_));
        // Print the input values
        cout << "sys_ord:\t" << datas[0] << endl;
        for(int i = 1; i < (int)datas.size(); i++)
            cout << "sys_"<<i<<":\t" << datas[i] << endl;
        // If source and target files exist, print them as well
        cout << "src:\t" << src << endl;
        // Print the reference reordering
        vector<vector<string> > src_order(ranks.GetMaxRank()+1);
        for(int i = 0; i < (int)srcs.size(); i++)
            src_order[ranks[i]].push_back(SafeAccess(srcs,i));
        cout << "ref:\t";
        for(int i = 0; i < (int)src_order.size(); i++) {
            if(i != 0) cout << " ";
            // If there is only one, print the string
            if(src_order[i].size() == 1) {
                cout << src_order[i][0];
            // If there is more than one, print a bracketed group
            } else {
                cout << "{{";
                BOOST_FOREACH(const string & s, src_order[i])
                    cout << " " << s;
                cout << " }}";
            }
        }
        cout << endl;
        if(args.size() > 3) {
            getline(*trg_in, trg);
            cout << "trg:\t" << trg << endl;
        }
        // Score the values
        for(int i = 0; i < (int) losses.size(); i++) {
            pair<double,double> my_loss = 
                                losses[i]->CalculateSentenceLoss(order,ranks);
            sums[i].first += my_loss.first;
            sums[i].second += my_loss.second;
            double acc = my_loss.second == 0 ? 
                                    1 : (1-my_loss.first/my_loss.second);
            if(i != 0) cout << "\t";
            cout << losses[i]->GetName() << "=" << acc 
                 << " (loss "<<my_loss.first<< "/" <<my_loss.second<<")";
        }
        cout << endl << endl;
    }
    cout << "Total:" << endl;
    for(int i = 0; i < (int) sums.size(); i++) {
        if(i != 0) cout << "\t";
        cout << losses[i]->GetName() << "="
             << (1 - sums[i].first/sums[i].second)
             << " (loss "<<sums[i].first << "/" <<sums[i].second<<")";
    }
    cout << endl;
}
