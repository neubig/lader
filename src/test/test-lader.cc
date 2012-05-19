#include "test-alignments-and-ranks.h"
#include "test-hyper-graph.h"
#include "test-feature-sequence.h"
#include "test-feature-parse.h"
#include "test-feature-align.h"
#include "test-feature-set.h"
#include "test-feature-vector.h"
#include "test-reorderer-model.h"
#include "test-loss-fuzzy.h"
#include "test-loss-tau.h"
#include "test-loss-bracket.h"
#include "test-dictionary.h"
#include <vector>

using namespace std;
using namespace lader;

int main() {
    // Initialize all the tests
    vector<TestBase*> tests;
    tests.push_back(new TestAlignmentsAndRanks());
    tests.push_back(new TestHyperGraph());
    tests.push_back(new TestFeatureSequence());
    tests.push_back(new TestFeatureParse());
    tests.push_back(new TestFeatureAlign());
    tests.push_back(new TestFeatureSet());
    tests.push_back(new TestFeatureVector());
    tests.push_back(new TestReordererModel());
    tests.push_back(new TestLossFuzzy());
    tests.push_back(new TestLossTau());
    tests.push_back(new TestLossBracket());
    tests.push_back(new TestDictionary());
    // Run all the tests
    int number_passed = 0;
    for(int i = 0; i < (int)tests.size(); i++)
        if(tests[i]->RunTest())
            number_passed++;
    // Check whether all were passed or not
    if(number_passed == (int)tests.size()) {
        cout << "**** passed ****" << endl;
    } else {
        cout << "**** FAILED!!! ****" << endl;
    }
    // Deallocate
    for(int i = 0; i < (int)tests.size(); i++)
        delete tests[i]; 
}
