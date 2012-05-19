#ifndef TARGET_SPAN_H__ 
#define TARGET_SPAN_H__

#include <boost/foreach.hpp>
#include <lader/hypothesis.h>
#include <lader/symbol-set.h>
#include <lader/util.h>
#include <vector>
#include <cfloat>
#include <set>

namespace lader {

class TargetSpan {
public:
    TargetSpan(int left, int right, int trg_left, int trg_right)
                    : left_(left), right_(right), 
                      trg_left_(trg_left), trg_right_(trg_right),
                      id_(-1) { }
    ~TargetSpan() {
        BOOST_FOREACH(Hypothesis * hyp, hyps_)
            delete hyp;
    }

    // Print a parse tree in Penn Treebank format
    void PrintParse(const std::vector<std::string> & strs,
                    std::ostream & out) const;

    void GetReordering(std::vector<int> & reord) const {
        HyperEdge::Type type = hyps_[0]->GetType();
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
    void AddHypothesis(const Hypothesis & hyp) {
        hyps_.push_back(new Hypothesis(hyp));
    }
    const std::set<char> & GetHasTypes() { return has_types_; }
    const std::vector<Hypothesis*> & GetHypotheses() const { return hyps_; }
    std::vector<Hypothesis*> & GetHypotheses() { return hyps_; }
    const Hypothesis* GetHypothesis(int i) const { return SafeAccess(hyps_,i); }
    Hypothesis* GetHypothesis(int i) { return SafeAccess(hyps_,i); }
    void ResetId() { id_ = -1; has_types_.clear(); }
    void SetId(int id) { id_ = id; }
    void SetHasType(char c) { has_types_.insert(c); }
    

private:

    int left_, right_, trg_left_, trg_right_;
    std::vector<Hypothesis*> hyps_;
    int id_;
    std::set<char> has_types_;
};

}

#endif

