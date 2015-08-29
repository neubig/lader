#include <lader/feature-vector.h>
#include <boost/foreach.hpp>

using namespace lader;

// Vector subtract
FeatureVectorInt lader::VectorSubtract(const FeatureVectorInt & a, 
                                const FeatureVectorInt & b) {
    FeatureVectorInt ret;
    int i = 0, j = 0, 
        x = (a.size()?a[i].first:INT_MAX), 
        y = (b.size()?b[j].first:INT_MAX);
    while(x != INT_MAX || y != INT_MAX) {
        if(x == y) {
            if(a[i].second != b[j].second)
                ret.push_back(MakePair(x, a[i].second - b[j].second));
            x = (++i == (int)a.size()?INT_MAX:a[i].first);
            y = (++j == (int)b.size()?INT_MAX:b[j].first);
        } else if(x < y) {
            ret.push_back(MakePair(x, a[i].second));
            x = (++i == (int)a.size()?INT_MAX:a[i].first);
        } else {
            ret.push_back(MakePair(y, -b[j].second));
            y = (++j == (int)b.size()?INT_MAX:b[j].first);
        }
    }
    return ret;
}

void lader::ClearAndSet(FeatureVectorInt & fvi,
		const IntMap<double> & feat_map)
{
    fvi.clear();
    BOOST_FOREACH(FeaturePairInt feat_pair, feat_map)
    	fvi.push_back(feat_pair);
}
