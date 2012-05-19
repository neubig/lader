
#include <lader/config-trainer.h>
#include <lader/reorderer-trainer.h>

using namespace lader;
using namespace std;

int main(int argc, char** argv) {
    // load the arguments
    ConfigTrainer conf;
    vector<string> args = conf.loadConfig(argc,argv);
    // train the reorderer
    ReordererTrainer trainer;
    trainer.TrainIncremental(conf);
}
