#ifndef CONFIG_EVALUATOR_H__
#define CONFIG_EVALUATOR_H__

#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <lader/util.h>
#include <lader/config-base.h>

namespace lader {

class ConfigEvaluator : public ConfigBase {

public:

    ConfigEvaluator() : ConfigBase() {
        minArgs_ = 3;
        maxArgs_ = 4;

        SetUsage(
"~~~ evaluate-lader ~~~\n"
"  by Graham Neubig\n"
"\n"
"Evaluates the reordering accuracy of a particular reordering for a sentence.\n"
"  Usage: evaluate-lader GOLDEN_ALIGNMENT DATA SRC [TRG]\n"
"\n"
"  GOLDEN_ALIGNMENT: [SRC_LEN]-[TRG_LEN] ||| f1-e1 f2-e2 f3-e3\n"
"  DATA: lader output \"a1 a2 ...\" where a1 is the reordered position of source word 1\n"
"  SRC: Sentence in the original source order\n"
"  TRG: Target sentence\n"
);

        AddConfigEntry("attach_null", "right", "Whether to attach null alignments to the left or right");
        AddConfigEntry("combine_blocks", "true", "Whether to attach the blocks together");

    }
	
};

}

#endif
