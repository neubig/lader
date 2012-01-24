#ifndef FEATURE_SEQUENCE_H__ 
#define FEATURE_SEQUENCE_H__

#include <vector>
#include <string>
#include <kyldr/feature-data-sequence.h>
#include <kyldr/feature-base.h>

namespace kyldr {

// A class to calculate features that concern sequences of words, tags, etc.
class FeatureSequence : public FeatureBase {
public:
    FeatureSequence() { }
    virtual ~FeatureSequence() { }
 
    // Parse the configuration
    // The configuration takes the following format:
    //  NAME%X%Y where NAME is the overall name and X and Y are template
    // 
    // The templates that can be used will all start with:
    //  S: For features designating an entire span
    //  N: For features defined over an entire non-terminal span
    //  T: For features defined over an entire terminal span
    //  L or R: For features defined over the left or right spans
    //  C: For features that compare the two spans
    //
    // The templates that can be used and combined for binary features are:
    //  %[SNTLR]S :    entire String of a span
    //  %[SNTLR][LR] : Left or Right-most word of a span
    //  %[SNTLR]N :    Number of words in a span
    //  %CD :          Difference (absolute value) in words in two spans
    // 
    // In addition N and D can be used with at the beginning (eg %SN#) to
    // fire non-binary features. These cannot be combined with other features
    virtual void ParseConfiguration(const std::string & str) = 0;

    // Parses a space-separated input string of data
    virtual FeatureDataBase * ParseData(const std::string & str) = 0;

    // Generates the features that can be factored over a node
    virtual FeatureVectorString GenerateNodeFeatures(
                                const FeatureDataSequence & sentence,
                                const HyperNode & node) = 0;

    // Generates the features that can be factored over an edge
    virtual FeatureVectorString GenerateEdgeFeatures(
                                const FeatureDataSequence & sentence,
                                const HyperNode & node,
                                const HyperEdge & edge) = 0;

    // Check to make sure that the feature template can be interpreted
    static bool FeatureTemplateIsLegal(const std::string & name);

private:

    std::vector<std::vector<std::string> > feature_names_;

};

}

#endif

