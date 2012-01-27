
#include <kyldr/reorderer-trainer.h>
#include <kyldr/config-trainer.h>

using namespace kyldr;
using namespace std;

int main(int argc, char** argv) {
    // load the arguments
    ConfigTrainer conf;
    vector<string> args = conf.loadConfig(argc,argv);
    // train the reorderer
    ReordererTrainer trainer;
    trainer.TrainIncremental(conf);
}
