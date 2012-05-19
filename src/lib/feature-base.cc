
#include <lader/feature-base.h>
#include <lader/feature-sequence.h>
#include <lader/feature-parse.h>
#include <lader/feature-align.h>

using namespace lader;
using namespace std;

FeatureBase * FeatureBase::CreateNew(const string & type) {
    // Create a new feature base based on the type
    if(type == "seq")
        return new FeatureSequence;
    else if(type == "cfg")
        return new FeatureParse;
    else if(type == "align")
        return new FeatureAlign;
    else
        THROW_ERROR("Bad feature type " << type << " (must be seq/cfg/align)");
    return NULL;
}
