
#include <lader/config-evaluator.h>
#include <lader/reorderer-evaluator.h>

using namespace lader;
using namespace std;

int main(int argc, char** argv) {
    // load the arguments
    ConfigEvaluator conf;
    vector<string> args = conf.loadConfig(argc,argv);
    // train the reorderer
    ReordererEvaluator evaluator;
    evaluator.Evaluate(conf);
}
