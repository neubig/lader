#include <kyldr/loss-bracket.h>

using namespace kyldr;
using namespace std;

double LossBracket::AddLossToProduction(
        int src_left, int src_mid, int src_right,
        int trg_left, int trg_midleft, int trg_midright, int trg_right,
        HyperEdge::Type type,
        const Ranks * ranks, const FeatureDataParse * parse) {
    if(!parse) THROW_ERROR("Bracketing loss requires parse input");
    const string & label = parse->GetSpanLabel(src_left, src_right);
    return label.length() != 1 || label[0] != (char)type ? weight_ : 0;
}


// Calculate the accuracy of a single sentence
std::pair<double,double> LossBracket::CalculateSentenceLoss(
        const std::vector<int> order,
        const Ranks * ranks, const FeatureDataParse * parse) {
    THROW_ERROR("CalculateSentenceLoss not available for parse");
    return MakePair(0,0);
}
