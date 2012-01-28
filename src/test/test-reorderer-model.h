#ifndef TEST_REORDERER_MODEL_H__
#define TEST_REORDERER_MODEL_H__

#include "test-base.h"
#include <kyldr/reorderer-model.h>

namespace kyldr {

class TestReordererModel : public TestBase {

public:

    TestReordererModel() {
        // Create a model with weights
        weights.insert(MakePair(string("0"),1));
        weights.insert(MakePair(string("1"),2));
        model.SetWeights(weights);
        // Create a feature vector
        feats.push_back(FeatureTuple("0", 0, 1));
        feats.push_back(FeatureTuple("1", 1, 2));
        feats.push_back(FeatureTuple("2", 2, 3));
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
        edge.SetLoss(5);
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

    int TestAdjustWeights() {
        // Make the input feature vector
        FeatureVector fvi;
        fvi.push_back(FeatureTuple("1", 1, 1));
        fvi.push_back(FeatureTuple("2", 2, 2));
        fvi.push_back(FeatureTuple("4", 4, -4));
        // Make the reordering model
        ReordererModel mod;
        mod.AdjustWeights(fvi, 1);
        // Make the expected model
        ReordererModel::WeightMap exp;
        exp.insert(MakePair(string("1"), 1));
        exp.insert(MakePair(string("2"), 2));
        exp.insert(MakePair(string("4"), -4));
        // Check to make sure it's ok
        int ret = 1;
        ret *= CheckMap(exp, mod.GetWeights());
        // Adjust again
        mod.AdjustWeights(fvi, -2);
        exp.clear();
        exp.insert(MakePair(string("1"), -1));
        exp.insert(MakePair(string("2"), -2));
        exp.insert(MakePair(string("4"), 4));
        ret *= CheckMap(exp, mod.GetWeights());
        return ret;
    }

    int TestModelIO() {
        ReordererModel exp;
        ReordererModel::WeightMap exp_weight;
        exp_weight.insert(MakePair(string("1"), 1));
        exp_weight.insert(MakePair(string("4"), 5));
        exp.SetWeights(exp_weight);
        ostringstream oss;
        exp.ToStream(oss);
        istringstream iss(oss.str());
        ReordererModel * act = ReordererModel::FromStream(iss);
        int ret = (exp == *act) ? 1 : 0;
        delete act;
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestScoreNode()" << endl; if(TestScoreNode()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestScoreEdge()" << endl; if(TestScoreEdge()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAdjustWeights()" << endl; if(TestAdjustWeights()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestModelIO()" << endl; if(TestModelIO()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestReordererModel Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    ReordererModel model;
    ReordererModel::WeightMap weights;
    FeatureVector feats;

};

}

#endif
