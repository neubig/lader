#ifndef TEST_LOSS_BRACKET_H__
#define TEST_LOSS_BRACKET_H__

#include "test-base.h"
#include <kyldr/combined-alignment.h>
#include <kyldr/loss-bracket.h>

namespace kyldr {

class TestLossBracket : public TestBase {

public:

    TestLossBracket() {
        // Create a parse tree with a terminal and non-terminal
        fdp.FromString("(S (I (F (FW a)) (B (BW b))) (F (FW c)))");
        lf.Initialize(NULL, &fdp);
        lf.SetWeight(0.5);
    }
    ~TestLossBracket() { }

    int TestBrackets() {
        int ret = 1;
        // The head matches
        double loss02s = lf.AddLossToProduction(0,2,2,0,0,1,1,
                                               HyperEdge::EDGE_STR,NULL,&fdp);
        if(loss02s != 0) {
            cerr << "loss02s "<<loss02s<<" != 0"<<endl; ret = 0;
        }
        // The head matches span but not type
        double loss02i = lf.AddLossToProduction(0,2,2,0,0,1,1,
                                               HyperEdge::EDGE_INV,NULL,&fdp);
        if(loss02i != 0.5) {
            cerr << "loss02i "<<loss02i<<" != 0.5"<<endl; ret = 0;
        }
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestBrackets()" << endl; if(TestBrackets()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestLossBracket Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    FeatureDataParse fdp;
    LossBracket lf;

};

}

#endif
