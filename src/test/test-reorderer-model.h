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
        // Create a feature vector
        feats.push_back(MakePair(0,1));
        feats.push_back(MakePair(1,2));
        feats.push_back(MakePair(2,3));
    }

    // int TestScoreEdge() {
    //     int ret = 1;
    //     HyperEdge edge;
    //     edge.SetFeatureVector(feats);
    //     edge.SetLoss(5);
    //     // Weight should be 1*1 + 2*2 (no loss)
    //     model.ScoreEdge(edge);
    //     if(edge.GetScore() != 5) {
    //         cout << "edge.GetScore() " << edge.GetScore() << " != 5" << endl;
    //         ret = 0;
    //     }
    //     // Weight should be 1*1 + 2*2 + (1*1 + 2*2) * 10
    //     model.ScoreEdge(edge, 10);
    //     if(edge.GetScore() != 55) {
    //         cout << "edge.GetScore() " << edge.GetScore() << " != 5" << endl;
    //         ret = 0;
    //     }
    //     return ret;
    // }

    int TestAdjustWeights() {
        // Make the reordering model
        ReordererModel mod;
        // lambda = 0.01
        //  1/sqrt(lambda) = 10
        mod.SetCost(0.01);

        // Make the input feature vector
        FeatureVectorInt fvi1;
        fvi1.push_back(MakePair(1,1));
        fvi1.push_back(MakePair(2,2));
        fvi1.push_back(MakePair(4,-2));
        mod.AdjustWeights(fvi1);
        // Make the expected model
        // This is the first iteration:
        //  nu = 1/lambda*t = 1/0.01 = 100
        //  w += nu/1*x
        //  w_{t+1/2} = 1:100 2:200 4:-200
        //  norm = sqrt(10000+40000+40000) = 300
        //  w_{t+1} = 10/300 = 1/30
        vector<double> exp(5,0);
        exp[1] = 100.0/30.0; exp[2] = 200.0/30.0; exp[4] = -200.0/30.0;
        // Check to make sure it's ok
        int ret = 1;
        ret *= CheckAlmostVector(exp, mod.GetWeights());

        // Try one more iteration
        FeatureVectorInt fvi2;
        fvi2.push_back(MakePair(0,2));
        fvi2.push_back(MakePair(3,1));
        fvi2.push_back(MakePair(5,-2));
        mod.AdjustWeights(fvi2);
        //  nu = 1/lambda*t = 1/0.01/2 = 50
        //  w += nu*x
        //  w_{t+1/2} = 0:100 1:50/30 2:100/30 3:50 4:-100 5:-100/30
        //  val = 100^2+100^2+50^2+(100^2+100^2+50^2)/(30^2) = 22525
        //  w_{t+1} = 10/sqrt(22525)*w_{t+1/2}
        exp = vector<double>(6,0);
        double norm = 10/sqrt(22525);
        exp[0] = 100.0    *norm;
        exp[1] = 50.0  /30*norm;
        exp[2] = 100.0 /30*norm;
        exp[3] = 50.0     *norm;
        exp[4] = -100.0/30*norm;
        exp[5] = -100.0   *norm;
        ret *= CheckAlmostVector(exp, mod.GetWeights());
        return ret;
    }

    int TestModelIO() {
        ReordererModel exp;
        vector<double> weights(6,0);
        weights[1] = 1; weights[4] = 5;
        exp.SetWeights(weights);
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
        // done++; cout << "TestScoreEdge()" << endl; if(TestScoreEdge()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAdjustWeights()" << endl; if(TestAdjustWeights()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestModelIO()" << endl; if(TestModelIO()) succeeded++; else cout << "FAILED!!!" << endl;
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
