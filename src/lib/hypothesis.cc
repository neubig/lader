/*
 * hypothesis.cc
 *
 *  Created on: Dec 10, 2013
 *      Author: leona
 */


#include <lader/hypothesis.h>
#include <lader/target-span.h>
#include <sstream>

using namespace std;
using namespace lader;

inline string GetTokenWord(const string & str) {
    ostringstream oss;
    for(int i = 0; i < (int)str.length(); i++) {
        switch (str[i]) {
            case '(': oss << "-LRB-"; break;
            case ')': oss << "-RRB-"; break;
            case '[': oss << "-LSB-"; break;
            case ']': oss << "-RSB-"; break;
            case '{': oss << "-LCB-"; break;
            case '}': oss << "-RCB-"; break;
            default: oss << str[i]; break;
        }
    }
    return oss.str();
}

void Hypothesis::PrintChildren( std::ostream& out ) const{
	if (left_child_ && left_child_->size() > 0){
		out << endl << "\t" << left_rank_ << "th LChild ";
		out << *GetLeftHyp();
	}
	if (right_child_ && right_child_->size() > 0){
		out << endl << "\t" << right_rank_ << "th RChild ";
		out << *GetRightHyp();
	}
	out << endl;
}

void Hypothesis::PrintParse(ostream & out) const {
    HyperEdge::Type type = GetEdgeType();
    if(IsTerminal()) {
        out << "(" << (char)type;
        for(int i = GetLeft(); i <= GetRight(); i++)
            out << " ("<<(char)type<< "W " << i <<")";
        out << ")";
    } else if(type == HyperEdge::EDGE_ROOT) {
        GetLeftHyp()->PrintParse(out);
    } else {
        out << "("<<(char)type<<" ";
        GetLeftHyp()->PrintParse(out);
        out << " ";
        GetRightHyp()->PrintParse(out);
        out << ")";
    }
}

void Hypothesis::PrintParse(const vector<string> & strs, ostream & out) const {
    HyperEdge::Type type = GetEdgeType();
    if(IsTerminal()) {
        out << "(" << (char)type;
        for(int i = GetLeft(); i <= GetRight(); i++)
            out << " ("<<(char)type<< "W " << GetTokenWord(strs[i]) <<")";
        out << ")";
    } else if(type == HyperEdge::EDGE_ROOT) {
        GetLeftHyp()->PrintParse(strs, out);
    } else {
        out << "("<<(char)type<<" ";
        GetLeftHyp()->PrintParse(strs, out);
        out << " ";
        GetRightHyp()->PrintParse(strs, out);
        out << ")";
    }
}

Hypothesis * Hypothesis::GetLeftHyp() const{
	if (!left_child_ || left_child_->size() == 0)
		return NULL;
	return left_child_->GetHypothesis(0);
}

Hypothesis * Hypothesis::GetRightHyp() const{
	if (!right_child_ || right_child_->size() == 0)
		return NULL;
	return right_child_->GetHypothesis(0);
}

void Hypothesis::GetReordering(std::vector<int> & reord, bool verbose) const{
	HyperEdge::Type type = this->GetEdgeType();
	if (verbose && !this->IsTerminal()){
		cerr << "Loss:" << this->GetLoss();
		cerr << *this;
		this->PrintChildren(cerr);
	}
	if(type == HyperEdge::EDGE_FOR) {
		for(int i = this->GetLeft(); i <= this->GetRight(); i++)
			reord.push_back(i);
	} else if(type == HyperEdge::EDGE_BAC) {
		for(int i = this->GetRight(); i >= this->GetLeft(); i--)
			reord.push_back(i);
	} else if(type == HyperEdge::EDGE_ROOT) {
		GetLeftHyp()->GetReordering(reord, verbose);
	} else if(type == HyperEdge::EDGE_STR) {
		GetLeftHyp()->GetReordering(reord, verbose);
		GetRightHyp()->GetReordering(reord, verbose);
	} else if(type == HyperEdge::EDGE_INV) {
		GetRightHyp()->GetReordering(reord, verbose);
		GetLeftHyp()->GetReordering(reord, verbose);
	}
}

// Clone a hypothesis with a hyper edge
Hypothesis *Hypothesis::Clone() const
{
    Hypothesis *new_hyp;
    HyperEdge *edge = this->GetEdge();
    new_hyp = new Hypothesis(*this);
    new_hyp->SetEdge(edge->Clone());
    return new_hyp;
}
