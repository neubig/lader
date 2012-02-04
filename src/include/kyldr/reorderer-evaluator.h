#ifndef REORDERER_EVALUATOR_H__ 
#define REORDERER_EVALUATOR_H__

#include <iostream>
#include <fstream>
#include <kyldr/config-evaluator.h>

namespace kyldr {

// A class to train the reordering model
class ReordererEvaluator {
public:

    ReordererEvaluator() { }
    ~ReordererEvaluator() { }
    
    // Run the evaluator
    void Evaluate(const ConfigEvaluator & config);

private:

};

}

#endif

