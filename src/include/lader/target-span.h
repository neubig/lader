#ifndef TARGET_SPAN_H__ 
#define TARGET_SPAN_H__

#include <boost/foreach.hpp>
#include <lader/hypothesis.h>
#include <lader/symbol-set.h>
#include <lader/util.h>
#include <vector>
#include <cfloat>
#include <set>
#include <sstream>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace lader;

namespace lader {

class TargetSpan {
public:
    TargetSpan(int left, int right, int trg_left, int trg_right)
                    : left_(left), right_(right), 
                      trg_left_(trg_left), trg_right_(trg_right),
                      id_(-1) { }
    
    void Clear()
    {
        BOOST_FOREACH(Hypothesis * hyp, hyps_)
            delete hyp;
        hyps_.clear();
    }

    virtual ~TargetSpan()
    {
        Clear();
    }
    // Print a parse tree in Penn Treebank format
    void PrintParse(const std::vector<std::string> & strs,
                    std::ostream & out) const;

    void GetReordering(std::vector<int> & reord) const {
        HyperEdge::Type type = hyps_[0]->GetEdgeType();
        if(type == HyperEdge::EDGE_FOR) {
            for(int i = left_; i <= right_; i++)
                reord.push_back(i);
        } else if(type == HyperEdge::EDGE_BAC) {
            for(int i = right_; i >= left_; i--)
                reord.push_back(i);
        } else if(type == HyperEdge::EDGE_ROOT) {
            hyps_[0]->GetLeftChild()->GetReordering(reord);
        } else if(type == HyperEdge::EDGE_STR) {
            hyps_[0]->GetLeftChild()->GetReordering(reord);
            hyps_[0]->GetRightChild()->GetReordering(reord);
        } else if(type == HyperEdge::EDGE_INV) {
            hyps_[0]->GetRightChild()->GetReordering(reord);
            hyps_[0]->GetLeftChild()->GetReordering(reord);
        }
    }

    void LabelWithIds(int & curr_id) {
        if(id_ != -1) return;
        BOOST_FOREACH(Hypothesis * hyp, hyps_) {
            if(hyp->GetLeftChild())
                hyp->GetLeftChild()->LabelWithIds(curr_id);
            if(hyp->GetRightChild())
                hyp->GetRightChild()->LabelWithIds(curr_id);
        }
        id_ = curr_id++;
    }

    double GetScore() const {
        double ret = !hyps_.size() ? -DBL_MAX : hyps_[0]->GetScore();
        return ret;
    }
    int GetId() const { return id_; }
    int GetLeft() const { return left_; }
    int GetRight() const { return right_; }
    int GetTrgLeft() const { return trg_left_; }
    int GetTrgRight() const { return trg_right_; }
    // Add a hypothesis with a new hyper-edge
    void AddHypothesis(Hypothesis * hyp) { hyps_.push_back(hyp); }
    // Add an unique hypothesis with a new hyper-edge
//    bool AddUniqueHypothesis(Hypothesis * hyp) {
//    	// TODO: hashtable for checking the uniqueness
//    	BOOST_FOREACH(Hypothesis * prev, hyps_)
//			if (*prev == *hyp)
//				return false;
//    	hyps_.push_back(hyp);
//    	return true;
//    }
    const std::set<char> & GetHasTypes() { return has_types_; }
    const std::vector<Hypothesis*> & GetHypotheses() const { return hyps_; }
    std::vector<Hypothesis*> & GetHypotheses() { return hyps_; }
    const Hypothesis* GetHypothesis(int i) const { return SafeAccess(hyps_,i); }
    Hypothesis* GetHypothesis(int i) { return SafeAccess(hyps_,i); }
    size_t size() const { return hyps_.size(); }
    const Hypothesis* operator[] (size_t val) const { return hyps_[val]; }
    Hypothesis* operator[] (size_t val) { return hyps_[val]; }
    void ResetId() { id_ = -1; has_types_.clear(); }
    void SetId(int id) { id_ = id; }
    void SetHasType(char c) { has_types_.insert(c); }

protected:
    int left_, right_, trg_left_, trg_right_;
    std::vector<Hypothesis*> hyps_;

private:
    int id_;
    std::set<char> has_types_;
};

}

namespace std {
// Output function for pairs
inline std::ostream& operator << ( std::ostream& out,
                                   const lader::TargetSpan & rhs )
{
    out << "[" << rhs.GetLeft() << ", " << rhs.GetRight() << "]";
    return out;
}
}

#endif

