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
    // A template for the features given the type, and a vector containing
    // the feature name and the values to be replaced
    typedef std::pair<FeatureType, std::vector<std::string> > FeatureTemplate;

    FeatureSequence() { }
    virtual ~FeatureSequence() { }
 
    // Parse the configuration
    // The configuration takes the following format:
    //  NAME%X%Y where NAME is the overall name and X and Y are template
    // 
    // The templates that can be used will all start with:
    //  S: For features designating an entire span
    //  L or R: For features defined over the left or right spans
    //  C: For features that compare the two spans
    //  E: For features defined over an edge (edge type)
    //
    // The templates that can be used and combined for binary features are:
    //  %[SLR]S :    entire String of a span
    //  %[SLR][LR] : Left or Right-most word of a span
    //  %[SLR]N :    Number of words in a span
    //  %CD :        Difference (absolute value) in words in two spans
    //  %ET :        The type of the edge
    // 
    // In addition N and D can be used with at the beginning (eg %SN#) to
    // fire non-binary features. These cannot be combined with other features
    virtual void ParseConfiguration(const std::string & str);

    // Parses a space-separated input string of data
    virtual FeatureDataBase * ParseData(const std::string & str);

    // Generates the features that can be factored over a node
    virtual void GenerateNodeFeatures(const FeatureDataBase & sentence,
                                      const HyperNode & node,
                                      FeatureVector & vec);

    // Generates the features that can be factored over an edge
    virtual void GenerateEdgeFeatures(const FeatureDataBase & sentence,
                                      const HyperNode & node,
                                      const HyperEdge & edge,
                                      FeatureVector & vec);

    // Get the type of this feature generator
    virtual std::string GetType() const { return "seq"; }

    // Check whether this is equal
    virtual bool CheckEqual(const FeatureBase & rhs) const {
        if(rhs.GetType() != this->GetType())
            return false;
        const FeatureSequence & rhs_seq = (const FeatureSequence &)rhs;
        if(feature_templates_.size() != rhs_seq.feature_templates_.size())
            return false;
        for(int i = 0; i < (int)feature_templates_.size(); i++)
            if(feature_templates_[i] != rhs_seq.feature_templates_[i])
                return false;
        return true;
    }

    // Check to make sure that the feature template can be interpreted
    static bool FeatureTemplateIsLegal(const std::string & name);

private:

    // Get a feature string defined over a span
    std::string GetSpanFeatureString(const FeatureDataSequence & sent,
                                     const HyperSpan & span,
                                     char type);

    std::string GetEdgeFeatureString(const FeatureDataSequence & sent,
                                     const HyperEdge & edge,
                                     char type);

    std::vector<FeatureTemplate> feature_templates_;

};

}

#endif

