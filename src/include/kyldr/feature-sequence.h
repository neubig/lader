#ifndef FEATURE_SEQUENCE_H__ 
#define FEATURE_SEQUENCE_H__

#include <vector>
#include <string>
#include <kyldr/feature-base.h>

namespace kyldr {

// A class to calculate features that concern sequences of words, tags, etc.
class FeatureSequence : public FeatureBase {
public:
    FeatureSequence() { }
    virtual ~FeatureSequence() { }
 
    // Parse the configuration, a comma separated string of feature to calculate
    virtual void ParseConfiguration(const string & str) = 0;

    // Parses a space-separated input string of data
    virtual FeatureDataBase * ParseData(const string & str) = 0;

    // Generates the features that can be factored over a node
    virtual FeatureVectorString GenerateNodeFeatures(
                                const FeatureDataSequence & sentence,
                                const HyperNode & node) = 0;

    // Generates the features that can be factored over an edge
    virtual FeatureVectorString GenerateEdgeFeatures(
                                const FeatureDataSequence & sentence,
                                const HyperNode & node,
                                const HyperEdge & edge) = 0;

private:

};

}

#endif

