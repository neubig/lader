#include <kyldr/loss-bracket.h>
#include <boost/foreach.hpp>

using namespace kyldr;
using namespace std;
using namespace boost;

double LossBracket::AddLossToProduction(
        int src_left, int src_mid, int src_right,
        int trg_left, int trg_midleft, int trg_midright, int trg_right,
        HyperEdge::Type type,
        const Ranks * ranks, const FeatureDataParse * parse) {
    if(!parse) THROW_ERROR("Bracketing loss requires parse input");
    int count = 0;
    // For the root, return the number of valid spans
    if(type == HyperEdge::EDGE_ROOT) {
        typedef std::pair<std::pair<int,int>, std::string> SpanPair;
        BOOST_FOREACH(const SpanPair & val, parse->GetSpans()) {
            if(val.second.length() == 1)
                count++;
        }
    } else {
        const string & label = parse->GetSpanLabel(src_left, src_right);
        if(label.length() == 1) {
            if(label[0] == 'X')
                count++;
            else if(label[0] == (char)type)
                count--;
        }
    }
    return weight_*count;
}


// Calculate the accuracy of a single sentence
std::pair<double,double> LossBracket::CalculateSentenceLoss(
        const std::vector<int> order,
        const Ranks * ranks, const FeatureDataParse * parse) {
    THROW_ERROR("CalculateSentenceLoss not available for parse");
    return MakePair(0,0);
}
