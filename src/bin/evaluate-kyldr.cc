
#include <kyldr/config-evaluator.h>
#include <kyldr/reorderer-evaluator.h>

using namespace kyldr;
using namespace std;

int main(int argc, char** argv) {
    // load the arguments
    ConfigTrainer conf;
    vector<string> args = conf.loadConfig(argc,argv);
    // train the reorderer
    ReordererEvaluator evaluator;
    evaluator.Evaluate(conf);
}
