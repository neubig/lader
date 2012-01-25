#ifndef TEST_FEATURE_SEQUENCE_H__
#define TEST_FEATURE_SEQUENCE_H__

#include "test-base.h"
#include <kyldr/combined-alignment.h>
#include <kyldr/feature-sequence.h>
#include <kyldr/feature-data-sequence.h>

namespace kyldr {

class TestFeatureSequence : public TestBase {

public:

    TestFeatureSequence() {
        // Create a combined alignment
        //  x..
        //  ..x
        //  .x.
        Alignment al(3,3);
        al.AddAlignment(0,0);
        al.AddAlignment(1,2);
        al.AddAlignment(2,1);
        cal = CombinedAlignment(al);
        // Create a sentence
        string str = "he ate rice";
        sent.ParseInput(str);
        // Create edges that reproduce the alignment in two ways
        node00 = HyperNode(0, HyperSpan(0,0,MakePair(0,0),MakePair(0,0)));
        edge00 = HyperEdge(0, HyperEdge::EDGE_TERMSTR);
        node00.AddEdge(&edge00);
        node11 = HyperNode(1, HyperSpan(1,1,MakePair(2,2),MakePair(2,2)));
        edge11 = HyperEdge(1, HyperEdge::EDGE_TERMSTR);
        node11.AddEdge(&edge11);
        node22 = HyperNode(2, HyperSpan(2,2,MakePair(1,1),MakePair(1,1)));
        edge22 = HyperEdge(2, HyperEdge::EDGE_TERMSTR);
        node22.AddEdge(&edge22);
        node12 = HyperNode(3, HyperSpan(1,2,MakePair(1,1),MakePair(2,2)));
        edge12nt = HyperEdge(3, HyperEdge::EDGE_INV, &node11, &node22);
        node12.AddEdge(&edge12nt);
        edge12t = HyperEdge(4, HyperEdge::EDGE_TERMINV);
        node12.AddEdge(&edge12t);
        node02 = HyperNode(4, HyperSpan(0,2,MakePair(0,0),MakePair(2,2)));
        edge02 = HyperEdge(5, HyperEdge::EDGE_STR, &node00, &node12);
        node02.AddEdge(&edge02);
    }
    ~TestFeatureSequence() { }

    int TestFeatureDataSequenceParse() {
        // Create the expected value
        vector<string> exp;
        exp.push_back("this");
        exp.push_back("is");
        exp.push_back("a");
        exp.push_back("test");
        // Create the real value
        string str = "this is a test";
        FeatureDataSequence fds;
        fds.ParseInput(str);
        vector<string> act = fds.GetSequence();
        return CheckVector(exp, act);
    }
    
    int TestFeatureTemplateIsLegal() {
        const int num = 8;
        const char* templ[num] = { "SS", "LN", "RS", "CD", "ET",
                                   "XY", "SD", "CR" };
        const bool exp[num] = {true, true, true, true, true,
                               false, false, false};
        int ret = 1;
        for(int i = 0; i < num; i++) {
            if(FeatureSequence::FeatureTemplateIsLegal(templ[i]) != exp[i]) {
                cout << "FeatureTemplateIsLegal failed on " << templ[i] << endl;
                ret = 0;
            }
        }
        return ret;
    }

    int TestNodeFeatures() {
        FeatureSequence feat;
        feat.ParseConfiguration("L%SL,R%SR,S%SS,N%SN");
        // These are both node-factored features, so they should exist for all
        // nodes, but no edges
        FeatureVectorString node00exp, node12exp, edge00exp, edge12ntexp;
        node00exp.push_back(MakePair(string("L||he"), 1));
        node00exp.push_back(MakePair(string("R||he"), 1));
        node00exp.push_back(MakePair(string("S||he"), 1));
        node00exp.push_back(MakePair(string("N||1"), 1));
        node12exp.push_back(MakePair(string("L||ate"), 1));
        node12exp.push_back(MakePair(string("R||rice"), 1));
        node12exp.push_back(MakePair(string("S||ate rice"), 1));
        node12exp.push_back(MakePair(string("N||2"), 1));
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(node00exp, feat.GenerateNodeFeatures(sent, node00));
        ret *= CheckVector(node12exp, feat.GenerateNodeFeatures(sent, node12));
        ret *= CheckVector(edge00exp, 
                           feat.GenerateEdgeFeatures(sent, node00, edge00));
        ret *= CheckVector(edge12ntexp, 
                           feat.GenerateEdgeFeatures(sent, node12, edge12nt));
        return ret;
    }

    int TestLeftRightFeatures() {
        FeatureSequence featl, featr, feata;
        featl.ParseConfiguration("L%LL,R%LR,S%LS,N%LN");
        featr.ParseConfiguration("L%RL,R%RR,S%RS,N%RN");
        feata.ParseConfiguration("A%SS%LS%RS");
        // These features apply to only non-terminals
        FeatureVectorString node00expl, node02expl, edge00expl, edge02expl;
        edge02expl.push_back(MakePair(string("L||he"), 1));
        edge02expl.push_back(MakePair(string("R||he"), 1));
        edge02expl.push_back(MakePair(string("S||he"), 1));
        edge02expl.push_back(MakePair(string("N||1"), 1));
        FeatureVectorString node00expr, node02expr, edge00expr, edge02expr;
        edge02expr.push_back(MakePair(string("L||ate"), 1));
        edge02expr.push_back(MakePair(string("R||rice"), 1));
        edge02expr.push_back(MakePair(string("S||ate rice"), 1));
        edge02expr.push_back(MakePair(string("N||2"), 1));
        FeatureVectorString node00expa, node02expa, edge00expa, edge02expa;
        edge02expa.push_back(
            MakePair(string("A||he ate rice||he||ate rice"), 1));
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(node00expl,featl.GenerateNodeFeatures(sent, node00));
        ret *= CheckVector(node02expl,featl.GenerateNodeFeatures(sent, node02));
        ret *= CheckVector(edge00expl,
                           featl.GenerateEdgeFeatures(sent, node00, edge00));
        ret *= CheckVector(edge02expl,
                           featl.GenerateEdgeFeatures(sent, node02, edge02));
        ret *= CheckVector(node00expr,featr.GenerateNodeFeatures(sent, node00));
        ret *= CheckVector(node02expr,featr.GenerateNodeFeatures(sent, node02));
        ret *= CheckVector(edge00expr,
                           featr.GenerateEdgeFeatures(sent, node00, edge00));
        ret *= CheckVector(edge02expr,
                           featr.GenerateEdgeFeatures(sent, node02, edge02));
        ret *= CheckVector(node00expa,feata.GenerateNodeFeatures(sent, node00));
        ret *= CheckVector(node02expa,feata.GenerateNodeFeatures(sent, node02));
        ret *= CheckVector(edge00expa,
                           feata.GenerateEdgeFeatures(sent, node00, edge00));
        ret *= CheckVector(edge02expa,
                           feata.GenerateEdgeFeatures(sent, node02, edge02));
        return ret;
    }

    int TestEdgeFeatures() {
        FeatureSequence feat;
        feat.ParseConfiguration("D%CD,T%ET");
        // These features apply to non-terminals
        FeatureVectorString node00exp, edge00exp, edge12exp, edge02exp;
        edge00exp.push_back(MakePair(string("T||F"), 1));
        edge12exp.push_back(MakePair(string("D||0"), 1));
        edge12exp.push_back(MakePair(string("T||I"), 1));
        edge02exp.push_back(MakePair(string("D||1"), 1));
        edge02exp.push_back(MakePair(string("T||S"), 1));
        // Test the features
        int ret = 1;
        ret *= CheckVector(node00exp,feat.GenerateNodeFeatures(sent, node00));
        ret *= CheckVector(edge00exp,
                           feat.GenerateEdgeFeatures(sent, node00, edge00));
        ret *= CheckVector(edge12exp,
                           feat.GenerateEdgeFeatures(sent, node12, edge12nt));
        ret *= CheckVector(edge02exp,
                           feat.GenerateEdgeFeatures(sent, node02, edge02));
        return ret;
    }


    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestFeatureDataSequenceParse()" << endl; if(TestFeatureDataSequenceParse()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestFeatureTemplateIsLegal()" << endl; if(TestFeatureTemplateIsLegal()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestNodeFeatures()" << endl; if(TestNodeFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestLeftRightFeatures()" << endl; if(TestLeftRightFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestEdgeFeatures()" << endl; if(TestEdgeFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestFeatureSequence Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    HyperEdge edge00, edge11, edge22, edge02, edge12t, edge12nt;
    HyperNode node00, node11, node22, node12, node02;
    CombinedAlignment cal;
    FeatureDataSequence sent;

};

}

#endif
