#ifndef TARGET_SPAN_H__ 
#define TARGET_SPAN_H__

#include <boost/foreach.hpp>
#include <kyldr/hypothesis.h>
#include <kyldr/util.h>
#include <vector>
#include <cfloat>

namespace kyldr {

class TargetSpan {
public:
    TargetSpan(int left, int right, int trg_left, int trg_right)
                    : left_(left), right_(right), 
                      trg_left_(trg_left), trg_right_(trg_right) { }
    ~TargetSpan() {
        BOOST_FOREACH(Hypothesis * hyp, hyps_)
            delete hyp;
    }

    void PrintParse(const std::vector<std::string> & strs,
                                            std::ostream & out) const {
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

    double GetScore() const {
        double ret = !hyps_.size() ? -DBL_MAX : hyps_[0]->GetScore();
        return ret;
    }
    int GetLeft() const { return left_; }
    int GetRight() const { return right_; }
    int GetTrgLeft() const { return trg_left_; }
    int GetTrgRight() const { return trg_right_; }
    void AddHypothesis(const Hypothesis & hyp) {
        hyps_.push_back(new Hypothesis(hyp));
    }
    const std::vector<Hypothesis*> & GetHypotheses() const { return hyps_; }
    std::vector<Hypothesis*> & GetHypotheses() { return hyps_; }
    const Hypothesis* GetHypothesis(int i) const { return SafeAccess(hyps_,i); }
    Hypothesis* GetHypothesis(int i) { return SafeAccess(hyps_,i); }

private:
    int left_, right_, trg_left_, trg_right_;
    std::vector<Hypothesis*> hyps_;
    int viterbi_hyp_;
};

}

#endif

