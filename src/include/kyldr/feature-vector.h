#ifndef FEATURE_VECTOR_H__ 
#define FEATURE_VECTOR_H__

#include <vector>
#include <string>

namespace kyldr {

// Real-valued feature vectors that are either indexed with std::strings or ints
typedef std::vector<std::pair<std::string, double> > FeatureVectorString;
typedef std::pair<int, double> FeaturePairInt;
typedef std::vector<std::pair<int, double> > FeatureVectorInt;

}

#endif

