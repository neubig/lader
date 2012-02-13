#ifndef HYPOTHESIS_H__ 
#define HYPOTHESIS_H__

#include <kyldr/hyper-edge.h>
#include <kyldr/util.h>
#include <sstream>
#include <iostream>

namespace kyldr {

class TargetSpan;

// A tuple that is used to hold hypotheses during cube pruning
class Hypothesis {
public:
    Hypothesis(double viterbi_score, int left, int right,
               int trg_left, int trg_right,
               HyperEdge::Type type, int center = -1,
               int left_rank = -1, int right_rank = -1,
               TargetSpan* left_child = NULL, TargetSpan* right_child = NULL) :
               viterbi_score_(viterbi_score), loss_(0),
               left_(left), right_(right),
               trg_left_(trg_left), trg_right_(trg_right),
               type_(type), center_(center), 
               left_child_(left_child), right_child_(right_child),
               left_rank_(left_rank), right_rank_(right_rank)
               { }

    // Get a string representing the rule of this hypothesis
    std::string GetRuleString(const std::vector<std::string> & sent,
                              char left_val = 0, char right_val = 0) const {
        std::ostringstream ret;
        ret << "[" << (char)type_ << "] |||";
        if(left_val) {
            ret << " ["<<left_val<<"]";
            if(right_val) ret << " ["<<right_val<<"]";
        } else {
            for(int i = left_; i <= right_; i++) {
                ret << " ";
                for(int j = 0; j < (int)sent[i].length(); j++) {
                    if(sent[i][j] == '\\' || sent[i][j] == '\"') ret << "\\";
                    ret << sent[i][j];
                }
            }
        }
        std::cerr << "ret.str() == " << ret.str() << std::endl;
        return ret.str();
    }

    // Comparators
    bool operator< (const Hypothesis & rhs) const {
        return 
            viterbi_score_ < rhs.viterbi_score_ || 
            (viterbi_score_ == rhs.viterbi_score_ && (
            trg_left_ < rhs.trg_left_ || (trg_left_ == rhs.trg_left_ && (
            trg_right_ < rhs.trg_right_ || (trg_right_ == rhs.trg_right_ && (
            type_ < rhs.type_ || (type_ == rhs.type_ && (
            center_ < rhs.center_ || (center_ == rhs.center_ && (
            left_child_ < rhs.left_child_ || (left_child_ == rhs.left_child_ && (
            right_child_ < rhs.right_child_))))))))))));
    }
    bool operator== (const Hypothesis & rhs) const {
        return
            viterbi_score_ == rhs.viterbi_score_ &&
            trg_left_ == rhs.trg_left_ &&
            trg_right_ == rhs.trg_right_ &&
            type_ == rhs.type_ &&
            center_ == rhs.center_ &&
            left_child_ == rhs.left_child_ &&
            right_child_ == rhs.right_child_;
    }

    // Accessors
    double GetScore() const { return viterbi_score_; }
    double GetLoss() const { return loss_; }
    int GetLeft() const { return left_; }
    int GetRight() const { return right_; }
    int GetTrgLeft() const { return trg_left_; }
    int GetTrgRight() const { return trg_right_; }
    int GetCenter() const { return center_; }
    HyperEdge::Type GetType() const { return type_; }
    TargetSpan* GetLeftChild() const { return left_child_; }
    TargetSpan* GetRightChild() const { return right_child_; }
    int GetLeftRank() const { return left_rank_; }
    int GetRightRank() const { return right_rank_; }

    void SetScore(double dub) { viterbi_score_ = dub; }
    void SetLoss(double dub) {
        // DEBUG std::cerr << "<" << GetLeft() << ", " << GetRight() << ", " << GetTrgLeft() << ", " << GetTrgRight() << ", " << (char)GetType() << ", " << GetCenter() << ">" << std::endl;
        loss_ = dub;
    }
    void SetLeftChild (TargetSpan* dub)  { left_child_ = dub; }
    void SetRightChild(TargetSpan* dub) { right_child_ = dub; }
    void SetLeftRank (int dub)  { left_rank_ = dub; }
    void SetRightRank(int dub) { right_rank_ = dub; }
    void SetTrgLeft(int dub) { trg_left_ = dub; }
    void SetTrgRight(int dub) { trg_right_ = dub; }
    void SetType(HyperEdge::Type type) { type_ = type; }


private:
    double viterbi_score_; // The Viterbi score for the entire subtree that
                           // this hypothesis represents
    double loss_;          // The loss for the single action represented
                           // by this hypothesis
    int left_, right_; // The source words on the left and right of the span
    int trg_left_, trg_right_; // The target words that fall on the far left
                               // and far right of this span
    HyperEdge::Type type_; // The edge type of the production
    int center_;          // The source center word of the hypothesis
    TargetSpan *left_child_, *right_child_; // The child hypothese for nonterm
    int left_rank_, right_rank_; // The ranks of the left and right hypotheses
};

}

namespace std {
// Output function for pairs
inline std::ostream& operator << ( std::ostream& out, 
                                   const kyldr::Hypothesis & rhs )
{
    out << "<" << rhs.GetLeft() << ", " << rhs.GetRight() << ", " << rhs.GetTrgLeft() << ", " << rhs.GetTrgRight() << ", " << (char)rhs.GetType() << ", " << rhs.GetCenter() << ">";
    return out;
}
}


#endif

