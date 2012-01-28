#include <kyldr/loss-fuzzy.h>
#include <kyldr/loss-base.h>

using namespace kyldr;
using namespace std;

LossBase * LossBase::CreateNew(const string & type) {
    // Create a new feature base based on the type
    if(type == "fuzzy")
        return new LossFuzzy;
    else
        THROW_ERROR("Bad feature type " << type << " (must be 'seq')");
    return NULL;
}
