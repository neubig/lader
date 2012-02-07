#ifndef CONFIG_EVALUATOR_H__
#define CONFIG_EVALUATOR_H__

#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <tr1/unordered_map>
#include <kyldr/util.h>
#include <kyldr/config-base.h>

namespace kyldr {

class ConfigEvaluator : public ConfigBase {

public:

    ConfigEvaluator() : ConfigBase() {
        minArgs_ = 2;
        maxArgs_ = 4;

        SetUsage(
"~~~ kyldr-evaluate ~~~\n"
"  by Graham Neubig\n"
"\n"
"Evaluates the reordering accuracy of a particluar reordering for a sentence.\n"
"  Usage: kyldr-evaluate GOLDEN_ALIGNMENT DATA [SRC] [TRG]\n"
"\n"
"  GOLDEN_ALIGNMENT: [SRC_LEN]-[TRG_LEN] ||| f1-e1 f2-e2 f3-e3\n"
"  INPUT: a1 a2 a3 a4 a5 where a1 is the reordered position of source word 1\n"
"  TRG: Sentence in the original source order\n"
"  TRG: Target sentence\n"
);

        AddConfigEntry("attach_null", "left", "Whether to attach null alignments to the left or right");

    }
	
};

}

#endif
