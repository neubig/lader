#include <kyldr/config-runner.h>
#include <kyldr/reorderer-runner.h>

using namespace kyldr;
using namespace std;

int main(int argc, char** argv) {
    // load the arguments
    ConfigRunner conf;
    vector<string> args = conf.loadConfig(argc,argv);
    // train the reorderer
    ReordererRunner runner;
    runner.Run(conf);
}
