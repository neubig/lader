#ifndef TEST_REORDERER_MODEL_H__
#define TEST_REORDERER_MODEL_H__

#include "test-base.h"
#include <kyldr/reorderer-model.h>

namespace kyldr {

class TestReordererModel : public TestBase {

public:

    TestReordererModel() {
        // Create a model with weights
        weights.push_back(1);
        weights.push_back(2);
        model.SetWeights(weights);
        model.SetLossWeights(weights);
        // Create a feature vector
        feats.push_back(MakePair(0,1));
        feats.push_back(MakePair(1,2));
        feats.push_back(MakePair(2,3));
    }

    // Score a single node
    int TestScoreNode() {
        int ret = 1;
        HyperNode node;
        node.SetFeatureVector(feats);
        model.ScoreNode(node);
        // Weight should be 1*1 + 2*2
        if(node.GetScore() != 5) {
            cout << "node.GetScore() " << node.GetScore() << " != 5" << endl;
            ret = 0;
        }
        return ret;
    }

    int TestScoreEdge() {
        int ret = 1;
        HyperEdge edge;
        edge.SetFeatureVector(feats);
        edge.SetLossVector(weights);
        // Weight should be 1*1 + 2*2 (no loss)
        model.ScoreEdge(edge);
        if(edge.GetScore() != 5) {
            cout << "edge.GetScore() " << edge.GetScore() << " != 5" << endl;
            ret = 0;
        }
        // Weight should be 1*1 + 2*2 + (1*1 + 2*2) * 10
        model.ScoreEdge(edge, 10);
        if(edge.GetScore() != 55) {
            cout << "edge.GetScore() " << edge.GetScore() << " != 5" << endl;
            ret = 0;
        }
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestScoreNode()" << endl; if(TestScoreNode()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestScoreEdge()" << endl; if(TestScoreEdge()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestReordererModel Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    ReordererModel model;
    std::vector<double> weights;
    FeatureVectorInt feats;

};

}

#endif
