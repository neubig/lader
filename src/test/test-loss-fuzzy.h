#ifndef TEST_LOSS_FUZZY_H__
#define TEST_LOSS_FUZZY_H__

#include "test-base.h"
#include <kyldr/combined-alignment.h>
#include <kyldr/loss-fuzzy.h>

namespace kyldr {

class TestLossFuzzy : public TestBase {

public:

    TestLossFuzzy() {
        // Create an alignment that looks like this
        // xx..
        // ...x
        // ..x.
        Alignment al(MakePair(4,3));
        al.AddAlignment(MakePair(0,0));
        al.AddAlignment(MakePair(1,0));
        al.AddAlignment(MakePair(2,2));
        al.AddAlignment(MakePair(3,1));
        cal = CombinedAlignment(al);
    }
    ~TestLossFuzzy() { }

    int TestStraightNonterminal() {
        int ret = 1;
        // Create an equal node, loss==0
        HyperNode node00(0, HyperSpan(0,0,MakePair(0,0),MakePair(0,0)));
        HyperNode node11(1, HyperSpan(1,1,MakePair(0,0),MakePair(0,0)));
        HyperNode node01(2, HyperSpan(0,1,MakePair(0,0),MakePair(0,0)));
        HyperEdge edge01(0, HyperEdge::EDGE_STR, &node00, &node11);
        double loss01 = lf.AddLossToEdge(cal,&node01,&edge01);
        if(loss01 != 0) {
            cerr << "loss01 "<<loss01<<" != 0"<<endl; ret = 0;
        }
        // Create a skipped node, loss==1
        HyperNode node22(3, HyperSpan(2,2,MakePair(2,2),MakePair(2,2)));
        HyperNode node12(4, HyperSpan(1,2,MakePair(0,0),MakePair(2,2)));
        HyperEdge edge12(0, HyperEdge::EDGE_STR, &node11, &node22);
        double loss12 = lf.AddLossToEdge(cal,&node12,&edge12);
        if(loss12 != 1) {
            cerr << "loss12 "<<loss12<<" != 1"<<endl; ret = 0;
        }
        // Create a reversed node, loss==1
        HyperNode node33(5, HyperSpan(3,3,MakePair(1,1),MakePair(1,1)));
        HyperNode node23(6, HyperSpan(1,2,MakePair(2,2),MakePair(1,1)));
        HyperEdge edge23(0, HyperEdge::EDGE_STR, &node22, &node33);
        double loss23 = lf.AddLossToEdge(cal,&node23,&edge23);
        if(loss23 != 1) {
            cerr << "loss23 "<<loss23<<" != 1"<<endl; ret = 0;
        }
        return ret;
    }

    int TestInvertedNonterminal() {
        int ret = 1;
        // Create an equal node, loss==0
        HyperNode node00(0, HyperSpan(0,0,MakePair(0,0),MakePair(0,0)));
        HyperNode node11(1, HyperSpan(1,1,MakePair(0,0),MakePair(0,0)));
        HyperNode node01(2, HyperSpan(0,1,MakePair(0,0),MakePair(0,0)));
        HyperEdge edge01(0, HyperEdge::EDGE_INV, &node00, &node11);
        double loss01 = lf.AddLossToEdge(cal,&node01,&edge01);
        if(loss01 != 0) {
            cerr << "loss01 "<<loss01<<" != 0"<<endl; ret = 0;
        }
        // Create a skipped node, loss==1
        HyperNode node22(3, HyperSpan(2,2,MakePair(2,2),MakePair(2,2)));
        HyperNode node12(4, HyperSpan(1,2,MakePair(2,2),MakePair(0,0)));
        HyperEdge edge12(0, HyperEdge::EDGE_INV, &node11, &node22);
        double loss12 = lf.AddLossToEdge(cal,&node12,&edge12);
        if(loss12 != 1) {
            cerr << "loss12 "<<loss12<<" != 1"<<endl; ret = 0;
        }
        // Create a reversed node, loss==0
        HyperNode node33(5, HyperSpan(3,3,MakePair(1,1),MakePair(1,1)));
        HyperNode node23(6, HyperSpan(1,2,MakePair(1,1),MakePair(2,2)));
        HyperEdge edge23(0, HyperEdge::EDGE_INV, &node22, &node33);
        double loss23 = lf.AddLossToEdge(cal,&node23,&edge23);
        if(loss23 != 0) {
            cerr << "loss23 "<<loss23<<" != 0"<<endl; ret = 0;
        }
        return ret;
    }

    int TestStraightTerminal() {
        int ret = 1;
        // Create an equal node, loss==0
        HyperNode node01(0, HyperSpan(0,1,MakePair(0,0),MakePair(0,0)));
        HyperEdge edge01(0, HyperEdge::EDGE_TERMSTR);
        double loss01 = lf.AddLossToEdge(cal,&node01,&edge01);
        if(loss01 != 0) {
            cerr << "loss01 "<<loss01<<" != 0"<<endl; ret = 0;
        }
        // Create a skipped node, loss==0
        HyperNode node12(1, HyperSpan(1,2,MakePair(0,0),MakePair(2,2)));
        HyperEdge edge12(0, HyperEdge::EDGE_TERMSTR);
        double loss12 = lf.AddLossToEdge(cal,&node12,&edge12);
        if(loss12 != 1) {
            cerr << "loss12 "<<loss12<<" != 1"<<endl; ret = 0;
        }
        // Create a three-word, loss==2
        HyperNode node13(2, HyperSpan(1,3,MakePair(0,0),MakePair(1,1)));
        HyperEdge edge13(0, HyperEdge::EDGE_TERMSTR);
        double loss13 = lf.AddLossToEdge(cal,&node13,&edge13);
        if(loss13 != 2) {
            cerr << "loss13 "<<loss13<<" != 2"<<endl; ret = 0;
        }
        return ret;
    }

    int TestInvertedTerminal() {
        int ret = 1;
        // Create an equal node, loss==0
        HyperNode node01(0, HyperSpan(0,1,MakePair(0,0),MakePair(0,0)));
        HyperEdge edge01(0, HyperEdge::EDGE_TERMINV);
        double loss01 = lf.AddLossToEdge(cal,&node01,&edge01);
        if(loss01 != 0) {
            cerr << "loss01 "<<loss01<<" != 0"<<endl; ret = 0;
        }
        // Create a skipped node, loss==1
        HyperNode node12(1, HyperSpan(1,2,MakePair(0,0),MakePair(2,2)));
        HyperEdge edge12(0, HyperEdge::EDGE_TERMINV);
        double loss12 = lf.AddLossToEdge(cal,&node12,&edge12);
        if(loss12 != 1) {
            cerr << "loss12 "<<loss12<<" != 1"<<endl; ret = 0;
        }
        // Create a three-word node, loss==1
        HyperNode node13(2, HyperSpan(1,3,MakePair(0,0),MakePair(1,1)));
        HyperEdge edge13(0, HyperEdge::EDGE_TERMINV);
        double loss13 = lf.AddLossToEdge(cal,&node13,&edge13);
        if(loss13 != 1) {
            cerr << "loss13 "<<loss13<<" != 1"<<endl; ret = 0;
        }
        return ret;
    }

    int TestRoot() {
        int ret = 1;
        // Create an equal node, loss==0
        HyperNode node1r(0, HyperSpan(-1,2,MakePair(-1,-1),MakePair(2,2)));
        HyperNode node1c(1, HyperSpan(0,1,MakePair(0,0),MakePair(1,1)));
        HyperEdge edge1(0, HyperEdge::EDGE_ROOT, &node1c);
        double loss1 = lf.AddLossToEdge(cal,&node1r,&edge1);
        if(loss1 != 0) {
            cerr << "loss1 "<<loss1<<" != 0"<<endl; ret = 0;
        }
        // Create a reversed node, loss==2
        HyperNode node2r(0, HyperSpan(-1,2,MakePair(-1,-1),MakePair(2,2)));
        HyperNode node2c(1, HyperSpan(0,1,MakePair(1,1),MakePair(0,0)));
        HyperEdge edge2(0, HyperEdge::EDGE_ROOT, &node2c);
        double loss2 = lf.AddLossToEdge(cal,&node2r,&edge2);
        if(loss2 != 2) {
            cerr << "loss2 "<<loss2<<" != 2"<<endl; ret = 2;
        }

        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestStraightNonterminal()" << endl; if(TestStraightNonterminal()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestInvertedNonterminal()" << endl; if(TestInvertedNonterminal()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestStraightTerminal()" << endl; if(TestStraightTerminal()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestInvertedTerminal()" << endl; if(TestInvertedTerminal()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestRoot()" << endl; if(TestRoot()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestLossFuzzy Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    CombinedAlignment cal;
    LossFuzzy lf;

};

}

#endif
