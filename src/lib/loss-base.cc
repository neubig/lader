#include <lader/loss-chunk.h>
#include <lader/loss-tau.h>
#include <lader/loss-bracket.h>
#include <lader/loss-base.h>

using namespace lader;
using namespace std;

LossBase * LossBase::CreateNew(const string & type) {
    // Create a new feature base based on the type
    if(type == "chunk")
        return new LossChunk;
    else if(type == "tau")
        return new LossTau;
    else if(type == "bracket")
        return new LossBracket;
    else
        THROW_ERROR("Bad loss type " << type << " (must be chunk/tau)");
    return NULL;
}
