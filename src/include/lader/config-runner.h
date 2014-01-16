#ifndef CONFIG_RUNNER_H__
#define CONFIG_RUNNER_H__

#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <lader/util.h>
#include <lader/config-base.h>

namespace lader {

class ConfigRunner : public ConfigBase {

public:

    ConfigRunner() : ConfigBase() {
        minArgs_ = 0;
        maxArgs_ = 2;

        SetUsage(
"~~~ lader ~~~\n"
"  by Graham Neubig\n"
"  by Hwidong Na for cube growing\n"
"\n"
"Reorders a text according to a discriminative model.\n"
);

        AddConfigEntry("out_format", "string", "A comma seperated list of outputs (string/parse/order)");
        AddConfigEntry("model", "", "A model to be used in reordering");
        AddConfigEntry("beam", "1", "The maximum beam size");
        AddConfigEntry("pop_limit", "10000", "The maximum pop count (for cube growing)");
        AddConfigEntry("cube_growing", "true", "Use Cube Growing for construction of hyper graph");
        AddConfigEntry("threads", "1", "The number of threads to use");
        AddConfigEntry("verbose", "0", "The level of debugging output to print 2: detail for building");
        AddConfigEntry("source_in", "", "The input file for the source sentences");
    }
	
};

}

#endif
