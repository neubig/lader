#ifndef FEATURE_BASE_H__ 
#define FEATURE_BASE_H__

#include <vector>
#include <string>
#include <kyldr/feature-vector.h>
#include <kyldr/feature-data-base.h>
#include <kyldr/hyper-edge.h>

namespace kyldr {

// A virtual class for feature generators
// Four functions must be implemented to create a feature generator
//   ParseConfiguration, ParseData, GenerateNodeFeatures, GenerateEdgeFeatures
class FeatureBase {
public:
    // Types of features, whether they are factored over nodes, all edges,
    // or only nonterminal edges
    typedef enum {
        ALL_FACTORED,
        NONTERM_FACTORED
    } FeatureType;

    // Constructors
    FeatureBase() { }
    virtual ~FeatureBase() { }
 
    // Parses an arbitrary configuration std::string to decide the type of features
    // that the generator will construct
    virtual void ParseConfiguration(const std::string & str) = 0;

    // Parses an input std::string of data (that is in the appropriate format for
    // the feature generator) into the generator's internal representation
    virtual FeatureDataBase * ParseData(const std::string & str) = 0;

    // Generates the features that can be factored over an edge
    virtual FeatureVectorString GenerateEdgeFeatures(
                                const FeatureDataBase & sentence,
                                const HyperEdge & edge) = 0;

    // Get the type string of this particular value
    virtual std::string GetType() const = 0;

    // Check to make sure this is equal to the right side
    virtual bool CheckEqual(const FeatureBase & rhs) const = 0;

    // Create a new sub-class of a particular type
    //  type=seq --> FeatureSequence
    static FeatureBase* CreateNew(const std::string & type);

private:

};

}

#endif

