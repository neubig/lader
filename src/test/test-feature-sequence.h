#ifndef TEST_FEATURE_SEQUENCE_H__
#define TEST_FEATURE_SEQUENCE_H__

#include "test-base.h"
#include <kyldr/combined-alignment.h>
#include <kyldr/feature-sequence.h>
#include <kyldr/feature-data-sequence.h>
#include <kyldr/feature-set.h>

namespace kyldr {

class TestFeatureSequence : public TestBase {

public:

    TestFeatureSequence() {
        // Create a combined alignment
        //  x..
        //  ..x
        //  .x.
        Alignment al(MakePair(3,3));
        al.AddAlignment(MakePair(0,0));
        al.AddAlignment(MakePair(1,2));
        al.AddAlignment(MakePair(2,1));
        cal = CombinedAlignment(al);
        // Create a sentence
        string str = "he ate rice";
        sent.ParseInput(str);
        string str_pos = "PRP VBD NN";
        sent_pos.ParseInput(str_pos);
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
        FeatureVector node00exp, node12exp, edge00exp, edge12ntexp;
        node00exp.push_back(FeatureTuple(string("L||he"), -1, 1));
        node00exp.push_back(FeatureTuple(string("R||he"), -1, 1));
        node00exp.push_back(FeatureTuple(string("S||he"), -1, 1));
        node00exp.push_back(FeatureTuple(string("N||1"), -1, 1));
        node12exp.push_back(FeatureTuple(string("L||ate"), -1, 1));
        node12exp.push_back(FeatureTuple(string("R||rice"), -1, 1));
        node12exp.push_back(FeatureTuple(string("S||ate rice"), -1, 1));
        node12exp.push_back(FeatureTuple(string("N||2"), -1, 1));
        // Do the parsing and checking
        int ret = 1;
        FeatureVector node00act, node12act, edge00act, edge12ntact;
        feat.GenerateNodeFeatures(sent,node00,node00act);
        feat.GenerateNodeFeatures(sent,node12,node12act);
        feat.GenerateEdgeFeatures(sent, node00, edge00, edge00act);
        feat.GenerateEdgeFeatures(sent, node12, edge12nt, edge12ntact);
        ret *= CheckVector(node00exp, node00act);
        ret *= CheckVector(node12exp, node12act);
        ret *= CheckVector(edge00exp, edge00act);
        ret *= CheckVector(edge12ntexp, edge12ntact);
        return ret;
    }

    int TestLeftRightFeatures() {
        FeatureSequence featl, featr, feata;
        featl.ParseConfiguration("L%LL,R%LR,S%LS,N%LN");
        featr.ParseConfiguration("L%RL,R%RR,S%RS,N%RN");
        feata.ParseConfiguration("A%SS%LS%RS");
        // These features apply to only non-terminals
        FeatureVector node00l, node02l, edge00l, edge02l;
        edge02l.push_back(FeatureTuple(string("L||he"), -1, 1));
        edge02l.push_back(FeatureTuple(string("R||he"), -1, 1));
        edge02l.push_back(FeatureTuple(string("S||he"), -1, 1));
        edge02l.push_back(FeatureTuple(string("N||1"), -1, 1));
        FeatureVector node00r, node02r, edge00r, edge02r;
        edge02r.push_back(FeatureTuple(string("L||ate"), -1, 1));
        edge02r.push_back(FeatureTuple(string("R||rice"), -1, 1));
        edge02r.push_back(FeatureTuple(string("S||ate rice"), -1, 1));
        edge02r.push_back(FeatureTuple(string("N||2"), -1, 1));
        FeatureVector node00a, node02a, edge00a, edge02a;
        edge02a.push_back(
            FeatureTuple(string("A||he ate rice||he||ate rice"), -1, 1));
        // Do the parsing and checking
        int ret = 1;
        FeatureVector node00actl, node02actl, edge00actl, edge02actl;
        featl.GenerateNodeFeatures(sent,node00,node00actl);
        featl.GenerateNodeFeatures(sent,node02,node02actl);
        featl.GenerateEdgeFeatures(sent, node00, edge00, edge00actl);
        featl.GenerateEdgeFeatures(sent, node02, edge02, edge02actl);
        ret *= CheckVector(node00l, node00actl);
        ret *= CheckVector(node02l, node02actl);
        ret *= CheckVector(edge00l, edge00actl);
        ret *= CheckVector(edge02l, edge02actl);
        FeatureVector node00actr, node02actr, edge00actr, edge02actr;
        featr.GenerateNodeFeatures(sent,node00,node00actr);
        featr.GenerateNodeFeatures(sent,node02,node02actr);
        featr.GenerateEdgeFeatures(sent, node00, edge00, edge00actr);
        featr.GenerateEdgeFeatures(sent, node02, edge02, edge02actr);
        ret *= CheckVector(node00r, node00actr);
        ret *= CheckVector(node02r, node02actr);
        ret *= CheckVector(edge00r, edge00actr);
        ret *= CheckVector(edge02r, edge02actr);
        FeatureVector node00acta, node02acta, edge00acta, edge02acta;
        feata.GenerateNodeFeatures(sent,node00,node00acta);
        feata.GenerateNodeFeatures(sent,node02,node02acta);
        feata.GenerateEdgeFeatures(sent, node00, edge00, edge00acta);
        feata.GenerateEdgeFeatures(sent, node02, edge02, edge02acta);
        ret *= CheckVector(node00a, node00acta);
        ret *= CheckVector(node02a, node02acta);
        ret *= CheckVector(edge00a, edge00acta);
        ret *= CheckVector(edge02a, edge02acta);
        return ret;
    }

    int TestEdgeFeatures() {
        FeatureSequence feat;
        feat.ParseConfiguration("D%CD,T%ET");
        // These features apply to non-terminals
        FeatureVector node00exp, edge00exp, edge12exp, edge02exp;
        edge00exp.push_back(FeatureTuple(string("T||F"), -1, 1));
        edge12exp.push_back(FeatureTuple(string("D||0"), -1, 1));
        edge12exp.push_back(FeatureTuple(string("T||I"), -1, 1));
        edge02exp.push_back(FeatureTuple(string("D||1"), -1, 1));
        edge02exp.push_back(FeatureTuple(string("T||S"), -1, 1));
        // Test the features
        int ret = 1;
        FeatureVector node00act, edge12act, edge00act, edge02act;
        feat.GenerateNodeFeatures(sent,node00,node00act);
        feat.GenerateEdgeFeatures(sent, node00, edge00, edge00act);
        feat.GenerateEdgeFeatures(sent, node12, edge12nt, edge12act);
        feat.GenerateEdgeFeatures(sent, node02, edge02, edge02act);
        ret *= CheckVector(node00exp, node00act);
        ret *= CheckVector(edge12exp, edge12act);
        ret *= CheckVector(edge00exp, edge00act);
        ret *= CheckVector(edge02exp, edge02act);
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
    FeatureDataSequence sent, sent_pos;

};

}

#endif
