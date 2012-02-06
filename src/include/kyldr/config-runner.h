#ifndef CONFIG_RUNNER_H__
#define CONFIG_RUNNER_H__

#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <tr1/unordered_map>
#include <kyldr/util.h>
#include <kyldr/config-base.h>

namespace kyldr {

class ConfigRunner : public ConfigBase {

public:

    ConfigRunner() : ConfigBase() {
        minArgs_ = 0;
        maxArgs_ = 2;

        SetUsage(
"~~~ kyldr ~~~\n"
"  by Graham Neubig\n"
"\n"
"Reorders a text according to a discriminative model.\n"
);

        AddConfigEntry("out_format", "string", "A comma seperated list of outputs (string/parse/order)");
        AddConfigEntry("model", "", "A model to be used in reordering");
        AddConfigEntry("beam", "0", "The maximum beam size");

    }
	
};

}

#endif
