#ifndef FEATURE_ALIGN_H__ 
#define FEATURE_ALIGN_H__

#include <vector>
#include <string>
#include <lader/feature-data-align.h>
#include <lader/feature-base.h>
#include <lader/dictionary.h>

namespace lader {

// A class to calculate features that concern alignments
class FeatureAlign : public FeatureBase {
public:
    // A template for the features given the type, and a vector containing
    // the feature name and the values to be replaced
    typedef std::pair<FeatureType, std::vector<std::string> > FeatureTemplate;

    FeatureAlign() { }
    virtual ~FeatureAlign() { }
 
    // Parse the configuration
    virtual void ParseConfiguration(const std::string & str);

    // Parses alignment data
    virtual FeatureDataBase * ParseData(const std::string & str);

    // Generates the features that can be factored over an edge
    virtual void GenerateEdgeFeatures(
                                const FeatureDataBase & sentence,
                                const HyperEdge & edge,
                                FeatureVectorString & feats);

    // Get the type of this feature generator
    virtual std::string GetType() const { return "align"; }

    // Check whether this is equal
    virtual bool CheckEqual(const FeatureBase & rhs) const {
        return rhs.GetType() == this->GetType();
    }

    // Check to make sure that the feature template can be interpreted
    static bool FeatureTemplateIsLegal(const std::string & name);

private:
    CombinedAlign::NullHandler attach_;
    LossTau loss_;

};

}

#endif

