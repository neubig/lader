#ifndef FEATURE_BASE_H__ 
#define FEATURE_BASE_H__

#include <vector>
#include <string>
#include <kyldr/feature-data-base.h>

namespace kyldr {

// Real-valued feature vectors that are either indexed with strings or ints
typedef std::vector<str::string, double> FeatureVectorString;
typedef std::vector<int, double> FeatureVectorInt;

// A virtual class for feature generators
// Four functions must be implemented to create a feature generator
//   ParseConfiguration, ParseData, GenerateNodeFeatures, GenerateEdgeFeatures
class FeatureBase {
public:
    FeatureBase() { }
    virtual ~FeatureBase() { }
 
    // Parses an arbitrary configuration string to decide the type of features
    // that the generator will construct
    virtual void ParseConfiguration(const string & str) = 0;

    // Parses an input string of data (that is in the appropriate format for
    // the feature generator) into the generator's internal representation
    virtual FeatureDataBase * ParseData(const string & str) = 0;

    // Generates the features that can be factored over a node
    virtual FeatureVectorString GenerateNodeFeatures(
                                const FeatureDataBase & sentence,
                                const HyperNode & node) = 0;

    // Generates the features that can be factored over an edge
    virtual FeatureVectorString GenerateEdgeFeatures(
                                const FeatureDataBase & sentence,
                                const HyperNode & node,
                                const HyperEdge & edge) = 0;

private:

};

}

#endif

