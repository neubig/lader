#include <kyldr/target-span.h>

using namespace kyldr;
using namespace std;

inline string GetTokenWord(const string & str) {
    if(str == "(")
        return "-LRB-";
    else if(str == ")")
        return "-RRB-";
    else if(str == "[")
        return "-LSB-";
    else if(str == "]")
        return "-RSB-";
    else if(str == "{")
        return "-LCB-";
    else if(str == "}")
        return "-RCB-";
    else
        return str;
}

void TargetSpan::PrintParse(const vector<string> & strs, ostream & out) const {
    HyperEdge::Type type = hyps_[0]->GetType();
    if(type == HyperEdge::EDGE_FOR || type == HyperEdge::EDGE_BAC) {
        out << "(" << (char)type;
        for(int i = left_; i <= right_; i++)
            out << " ("<<(char)type<< "W " << GetTokenWord(strs[i]) <<")";
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
