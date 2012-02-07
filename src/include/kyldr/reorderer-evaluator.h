#ifndef REORDERER_EVALUATOR_H__ 
#define REORDERER_EVALUATOR_H__

#include <iostream>
#include <fstream>
#include <kyldr/config-evaluator.h>
#include <kyldr/combined-alignment.h>

namespace kyldr {

// A class to train the reordering model
class ReordererEvaluator {
public:

    ReordererEvaluator() : attach_(CombinedAlign::ATTACH_NULL_LEFT) { }
    ~ReordererEvaluator() { }
    
    // Run the evaluator
    void Evaluate(const ConfigEvaluator & config);

private:
    CombinedAlign::NullHandler attach_;

};

}

#endif

