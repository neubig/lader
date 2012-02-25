#include <kyldr/feature-align.h>

#include <sstream>
#include <cfloat>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace kyldr;
using namespace std;

// Align the comma-separated list of configuration options
void FeatureAlign::ParseConfiguration(const string & str) {
    if(str == "left")
        attach_ = CombinedAlign::ATTACH_NULL_LEFT;
    else if(str == "right")
        attach_ = CombinedAlign::ATTACH_NULL_RIGHT;
    else
        THROW_ERROR("FeatureAlign only takes left or right: "<<str);
}

// Parses an input string of data (that is in the appropriate format for
// the feature generator) into the generator's internal representation
FeatureDataBase * FeatureAlign::ParseData(const string & str) {
    FeatureDataAlign * data = new FeatureDataAlign;
    data->SetAttach(attach_);
    data->FromString(str);
    loss_.Initialize(data->GetRanks(), NULL);
    return data;
}

// Generates the features that can be factored over an edge
void FeatureAlign::GenerateEdgeFeatures(
                            const FeatureDataBase & sent,
                            const HyperEdge & edge,
                            FeatureVectorString & feat) {
    const FeatureDataAlign & align_data = (const FeatureDataAlign &)sent;
    feat.push_back(MakePair(string("TAU"),
        loss_.AddLossToProduction(
            edge.GetLeft(), edge.GetCenter(), edge.GetRight(),
            -1, -1, -1 ,-1, edge.GetType(), align_data.GetRanks(), NULL)));
}
