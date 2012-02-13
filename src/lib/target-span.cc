#include <kyldr/target-span.h>

using namespace kyldr;
using namespace std;

void TargetSpan::PrintParse(const vector<string> & strs, ostream & out) const {
    HyperEdge::Type type = hyps_[0]->GetType();
    if(type == HyperEdge::EDGE_FOR || type == HyperEdge::EDGE_BAC) {
        out << "(" << (char)type;
        for(int i = left_; i <= right_; i++)
            out << " ("<<(char)type<<"W " << strs[i]<<")";
        out << ")";
    } else if(type == HyperEdge::EDGE_ROOT) {
        hyps_[0]->GetLeftChild()->PrintParse(strs, out);
    } else {
        out << "("<<(char)type<<" ";
        hyps_[0]->GetLeftChild()->PrintParse(strs, out);
        out << " ";
        hyps_[0]->GetRightChild()->PrintParse(strs, out);
        out << ")";
    }
}
