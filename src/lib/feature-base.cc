
#include <kyldr/feature-base.h>
#include <kyldr/feature-sequence.h>

using namespace kyldr;
using namespace std;

FeatureBase * FeatureBase::CreateNew(const string & type) {
    // Create a new feature base based on the type
    if(type == "seq")
        return new FeatureSequence;
    else
        THROW_ERROR("Bad feature type " << type << " (must be 'seq')");
    return NULL;
}
