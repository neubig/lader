#ifndef FEATURE_BASE_H__ 
#define FEATURE_BASE_H__

#include <vector>
#include <string>
#include <kyldr/feature-data-base.h>
#include <kyldr/hyper-edge.h>
#include <kyldr/hyper-node.h>

namespace kyldr {

// Real-valued feature vectors that are either indexed with std::strings or ints
typedef std::vector<std::pair<std::string, double> > FeatureVectorString;
typedef std::vector<std::pair<int, double> > FeatureVectorInt;

// A virtual class for feature generators
// Four functions must be implemented to create a feature generator
//   ParseConfiguration, ParseData, GenerateNodeFeatures, GenerateEdgeFeatures
class FeatureBase {
public:
    // Types of features, whether they are factored over nodes, all edges,
    // or only nonterminal edges
    typedef enum {
        NODE_FACTORED,
        EDGE_FACTORED,
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

