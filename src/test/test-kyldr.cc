#include "test-combined-alignment.h"
#include "test-hyper-graph.h"
#include "test-loss-fuzzy.h"
#include <vector>

using namespace std;
using namespace kyldr;

int main() {
    // Initialize all the tests
    vector<TestBase*> tests;
    tests.push_back(new TestCombinedAlignment());
    tests.push_back(new TestHyperGraph());
    tests.push_back(new TestLossFuzzy());
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
