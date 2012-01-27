#include <kyldr/feature-vector.h>

using namespace kyldr;

// Vector subtract
FeatureVectorInt kyldr::VectorSubtract(const FeatureVectorInt & a, 
                                const FeatureVectorInt & b) {
    FeatureVectorInt ret;
    int i = 0, j = 0, 
        x = (a.size()?a[i].first:INT_MAX), 
        y = (b.size()?b[j].first:INT_MAX);
    while(x != INT_MAX || y != INT_MAX) {
        if(x == y) {
            if(a[i].second != b[j].second)
                ret.push_back(MakePair(x, a[i].second - b[j].second));
            x = (++i == (int)a.size()?a[i].first:INT_MAX);
            y = (++j == (int)b.size()?b[j].first:INT_MAX);
        } else if(x < y) {
            ret.push_back(MakePair(x, a[i].second));
            x = (++i == (int)a.size()?a[i].first:INT_MAX);
        } else {
            ret.push_back(MakePair(y, -b[j].second));
            y = (++j == (int)b.size()?b[j].first:INT_MAX);
        }
    }
    return ret;
}
