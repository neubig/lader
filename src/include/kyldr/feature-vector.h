#ifndef FEATURE_VECTOR_H__ 
#define FEATURE_VECTOR_H__

#include <vector>
#include <string>
#include <climits>
#include <kyldr/util.h>

namespace kyldr {


class FeatureTuple {
public:
    FeatureTuple(std::string n, int i, double v) :
        name(n), value(v) { }

    bool operator==(const FeatureTuple & rhs) const {
        return name == rhs.name && value == rhs.value;
    }
    bool operator!=(const FeatureTuple & rhs) const { return !(*this == rhs); }

    std::string name;
    double value;
};
typedef std::vector<FeatureTuple> FeatureVector;

}

namespace std {

// Output for FeatureTumples
inline std::ostream& operator << ( std::ostream& out,
                                   const kyldr::FeatureTuple & rhs ) {
    out << "<"<<rhs.name<<","<<rhs.value<<">";
    return out;
}

}

#endif

