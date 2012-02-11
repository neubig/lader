#ifndef CONFIG_TRAINER_H__
#define CONFIG_TRAINER_H__

#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <kyldr/util.h>
#include <kyldr/config-base.h>

namespace kyldr {

class ConfigTrainer : public ConfigBase {

public:

    ConfigTrainer() : ConfigBase() {
        minArgs_ = 0;
        maxArgs_ = 0;

        SetUsage(
"~~~ train-kyldr ~~~\n"
"  by Graham Neubig\n"
"\n"
"Trains a model discriminative model for machine translation reordering.\n"
);

        // AddConfigEntry("first_step", "1", "The first step (1=hypergraph, 2=loss, 3=features, 4=train)");
        // AddConfigEntry("last_step", "4", "The last step to perform");
        AddConfigEntry("align_in", "", "The input file for the alignments");
        AddConfigEntry("source_in", "", "The input file for the source sentences");
        AddConfigEntry("max_term", "0", "The maximum length of a terminal ");
        AddConfigEntry("beam", "0", "The maximum beam size");
        AddConfigEntry("save_features", "true", "Save the features in memory after generating them once (more memory, more speed)");
        AddConfigEntry("use_reverse", "true", "Whether to use reverse terminals ");
        AddConfigEntry("attach_null", "left", "Whether to attach null alignments to the left or right");
        AddConfigEntry("combine_blocks", "true", "Whether to combine alignments into blocks");
        AddConfigEntry("loss_profile", "fuzzy=1", "Which loss functions to use");
        AddConfigEntry("feature_profile", "seq=T%ET,S%ET%SS", "Which loss functions to use ");
        AddConfigEntry("iterations", "10", "The number of iterations of training to perform.");
        AddConfigEntry("verbose", "0", "The level of debugging output to print");
        AddConfigEntry("cost", "1e-5", "The rate at which to learn\n");
        AddConfigEntry("shuffle", "true", "Whether to shuffle the input");
        AddConfigEntry("model_out", "", "An output file for the model");

    }
	
};

}

#endif
