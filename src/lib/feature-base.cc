
#include <kyldr/feature-base.h>
#include <kyldr/feature-sequence.h>
#include <kyldr/feature-parse.h>
#include <kyldr/feature-align.h>

using namespace kyldr;
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
