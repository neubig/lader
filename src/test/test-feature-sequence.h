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

    TestFeatureSequence() : 
            edge00(0, -1, 0, HyperEdge::EDGE_FOR), 
            edge11(1, -1, 1, HyperEdge::EDGE_FOR), 
            edge22(2, -1, 2, HyperEdge::EDGE_FOR),
            edge12t(1, -1, 2, HyperEdge::EDGE_BAC),
            edge12nt(1, 2, 2, HyperEdge::EDGE_INV),
            edge02(0, 1, 2, HyperEdge::EDGE_STR) {
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

    int TestLeftRightFeatures() {
        FeatureSequence featl, featr, feata;
        featl.ParseConfiguration("L%LL,R%LR,S%LS,N%LN");
        featr.ParseConfiguration("L%RL,R%RR,S%RS,N%RN");
        feata.ParseConfiguration("A%SS%LS%RS");
        // These features apply to only non-terminals
        FeatureVectorString edge00l, edge02l;
        edge02l.push_back(MakePair(string("L||he"), 1));
        edge02l.push_back(MakePair(string("R||he"), 1));
        edge02l.push_back(MakePair(string("S||he"), 1));
        edge02l.push_back(MakePair(string("N||1"), 1));
        FeatureVectorString edge00r, edge02r;
        edge02r.push_back(MakePair(string("L||ate"), 1));
        edge02r.push_back(MakePair(string("R||rice"), 1));
        edge02r.push_back(MakePair(string("S||ate rice"), 1));
        edge02r.push_back(MakePair(string("N||2"), 1));
        FeatureVectorString edge00a, edge02a;
        edge02a.push_back(
            MakePair(string("A||he ate rice||he||ate rice"), 1));
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(edge00l,
                           featl.GenerateEdgeFeatures(sent, edge00));
        ret *= CheckVector(edge02l,
                           featl.GenerateEdgeFeatures(sent, edge02));
        ret *= CheckVector(edge00r,
                           featr.GenerateEdgeFeatures(sent, edge00));
        ret *= CheckVector(edge02r,
                           featr.GenerateEdgeFeatures(sent, edge02));
        ret *= CheckVector(edge00a,
                           feata.GenerateEdgeFeatures(sent, edge00));
        ret *= CheckVector(edge02a,
                           feata.GenerateEdgeFeatures(sent, edge02));
        return ret;
    }

    int TestEdgeFeatures() {
        FeatureSequence feat;
        feat.ParseConfiguration("D%CD,T%ET");
        // These features apply to non-terminals
        FeatureVectorString edge00exp, edge12exp, edge02exp;
        edge00exp.push_back(MakePair(string("T||F"), 1));
        edge12exp.push_back(MakePair(string("D||0"), 1));
        edge12exp.push_back(MakePair(string("T||I"), 1));
        edge02exp.push_back(MakePair(string("D||1"), 1));
        edge02exp.push_back(MakePair(string("T||S"), 1));
        // Test the features
        int ret = 1;
        ret *= CheckVector(edge00exp,
                           feat.GenerateEdgeFeatures(sent, edge00));
        ret *= CheckVector(edge12exp,
                           feat.GenerateEdgeFeatures(sent, edge12nt));
        ret *= CheckVector(edge02exp,
                           feat.GenerateEdgeFeatures(sent, edge02));
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestFeatureDataSequenceParse()" << endl; if(TestFeatureDataSequenceParse()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestFeatureTemplateIsLegal()" << endl; if(TestFeatureTemplateIsLegal()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestLeftRightFeatures()" << endl; if(TestLeftRightFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestEdgeFeatures()" << endl; if(TestEdgeFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestFeatureSequence Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    HyperEdge edge00, edge11, edge22, edge12t, edge12nt, edge02;
    CombinedAlignment cal;
    FeatureDataSequence sent, sent_pos;

};

}

#endif
