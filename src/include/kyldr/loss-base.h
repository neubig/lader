#ifndef LOSS_BASE_H__
#define LOSS_BASE_H__

#include <kyldr/ranks.h>
#include <kyldr/hyper-graph.h>
#include <kyldr/hyper-edge.h>

namespace kyldr {

// A base class that adds losses to all of the cells and edges in a hypergraph
// To create new loss functions, it is necessary to re-implement 
//   AddLossToEdge (and also optionally Initialize)
class LossBase {
public:

    // Constructor and destructor
    LossBase() : weight_(1.0) { }
    virtual ~LossBase() { }
    
    // Add a loss value to a production
    //  src_left: The starting position of the left span
    //  src_mid: The starting position of the right span (or nonterm -1)
    //  src_right: The ending position of the right span
    //  trg_left: The starting target word of the span that will be reordered
    //            to the left in the target if this production is used
    //  trg_midleft: Ditto, for ending target word of the left span
    //  trg_midright: Ditto, for starting target word of the right span
    //  trg_right: Ditto, for ending target word of the right span
    //  type: The type of production
    //  ranks: The correct ranks of the words
    virtual double AddLossToProduction(
        int src_left, int src_mid, int src_right,
        int trg_left, int trg_midleft, int trg_midright, int trg_right,
        HyperEdge::Type type, const Ranks & ranks) = 0;
    
    // Calculate the accuracy of a single sentence
    virtual std::pair<double,double> CalculateSentenceLoss(
            const std::vector<int> order, const Ranks & ranks) = 0;

    // Initializes the loss calculator with a ranks 
    virtual void Initialize(const Ranks & ranks) { }

    // Get the name of the subclass
    virtual std::string GetName() const = 0;

    void AddLossToHyperGraph(const Ranks & ranks,
                             HyperGraph & hyper_graph);

    // Create a new sub-class of a particular type
    //  type=fuzzy --> LossFuzzy
    static LossBase* CreateNew(const std::string & type);

    // Accessors
    void SetWeight(double weight) { weight_ = weight; }

protected:
    double weight_;

};

}

#endif

