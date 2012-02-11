#ifndef TEST_FEATURE_SEQUENCE_H__
#define TEST_FEATURE_SEQUENCE_H__

#include "test-base.h"
#include <kyldr/combined-alignment.h>
#include <kyldr/feature-sequence.h>
#include <kyldr/feature-data-sequence.h>
#include <kyldr/feature-set.h>
#include <fstream>

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
        vector<string> words(3, "x");
        Alignment al(MakePair(3,3));
        al.AddAlignment(MakePair(0,0));
        al.AddAlignment(MakePair(1,2));
        al.AddAlignment(MakePair(2,1));
        cal = Ranks(CombinedAlign(words,al));
        // Create a sentence
        string str = "he ate rice";
        sent.FromString(str);
        string str_pos = "PRP VBD NN";
        sent_pos.FromString(str_pos);
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
        fds.FromString(str);
        vector<string> act = fds.GetSequence();
        return CheckVector(exp, act);
    }
    
    int TestFeatureTemplateIsLegal() {
        const int num = 12;
        const char* templ[num] = { "SS", "LN", "RS", "CD", "ET",
                                   "XY", "SD", "CR", "SQE", "LQ1",
                                   "LQE1", "SQ#04" };
        const bool exp[num] = {true, true, true, true, true,
                               false, false, false, false, false,
                               true, true};
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
        ReordererModel mod;
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
        // Create vectors
        FeatureVectorString edge00lact, edge02lact, edge00ract, 
                            edge02ract, edge00aact, edge02aact;
        featl.GenerateEdgeFeatures(sent, edge00, edge00lact);
        featl.GenerateEdgeFeatures(sent, edge02, edge02lact);
        featr.GenerateEdgeFeatures(sent, edge00, edge00ract);
        featr.GenerateEdgeFeatures(sent, edge02, edge02ract);
        feata.GenerateEdgeFeatures(sent, edge00, edge00aact);
        feata.GenerateEdgeFeatures(sent, edge02, edge02aact);
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(edge00l, edge00lact);
        ret *= CheckVector(edge02l, edge02lact);
        ret *= CheckVector(edge00r, edge00ract);
        ret *= CheckVector(edge02r, edge02ract);
        ret *= CheckVector(edge00a, edge00aact);
        ret *= CheckVector(edge02a, edge02aact);
        return ret;
    }

    int TestSequenceFeatures() {
        // Create a dictionary and write it to /tmp/dict.txt
        std::vector<double> hvec(2,0), arvec(2,0);
        hvec[0] = 0.1; hvec[1] = 0.2;
        arvec[0] = 0.3; arvec[1] = 0.4;
        Dictionary dict;
        dict.AddEntry("he", hvec); dict.AddEntry("ate rice", arvec);
        ofstream out("/tmp/dict.txt");
        dict.ToStream(out);
        out.close();
        // Parse the feature sequence
        FeatureSequence feat;
        feat.ParseConfiguration("dict=/tmp/dict.txt,QE%SN%SQE0,Q1%SN%SQ#01");
        // Test for the span
        FeatureVectorString feat00, feat02;
        feat00.push_back(MakePair(string("QE||1||+"), 1));
        feat00.push_back(MakePair(string("Q1||1"), 0.2));
        feat02.push_back(MakePair(string("QE||3||-"), 1));
        // Create the actual features
        FeatureVectorString edge00act, edge02act;
        feat.GenerateEdgeFeatures(sent, edge00, edge00act);
        feat.GenerateEdgeFeatures(sent, edge02, edge02act);
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(feat00, edge00act);
        ret *= CheckVector(feat02, edge02act);
        return ret;
    }

    int TestEdgeFeatures() {
        FeatureSequence feat;
        feat.ParseConfiguration("D%CD,B%CB,D#%CD#,B#%CB#,T%ET");
        // These features apply to non-terminals
        FeatureVectorString edge00exp, edge12exp, edge02exp;
        edge00exp.push_back(MakePair(string("T||F"), 1));
        edge12exp.push_back(MakePair(string("D||0"), 1));
        edge12exp.push_back(MakePair(string("B||0"), 1));
        edge12exp.push_back(MakePair(string("T||I"), 1));
        edge02exp.push_back(MakePair(string("D||1"), 1));
        edge02exp.push_back(MakePair(string("B||1"), 1));
        edge02exp.push_back(MakePair(string("D#"), 1));
        edge02exp.push_back(MakePair(string("B#"), 1));
        edge02exp.push_back(MakePair(string("T||S"), 1));
        // Create the actual features
        FeatureVectorString edge00act, edge12ntact, edge02act;
        feat.GenerateEdgeFeatures(sent, edge00, edge00act);
        feat.GenerateEdgeFeatures(sent, edge12nt, edge12ntact);
        feat.GenerateEdgeFeatures(sent, edge02, edge02act);
        // Test the features
        int ret = 1;
        ret *= CheckVector(edge00exp, edge00act);
        ret *= CheckVector(edge12exp, edge12ntact);
        ret *= CheckVector(edge02exp, edge02act);
        return ret;
    }

    int TestReorderData() {
        FeatureDataSequence data;
        data.FromString("a b c d");
        vector<int> order(4,0);
        order[0] = 2; order[1] = 0; order[2] = 3; order[3] = 1;
        data.Reorder(order);
        string act = data.ToString();
        string exp = "c a d b";
        int ret = 1;
        if(exp != act) {
            cerr << "Reordering failed: act (" << act << ") != " 
                 << " exp (" << exp << ")" << endl;
            ret = 0;
        }
        return ret; 
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestFeatureDataSequenceParse()" << endl; if(TestFeatureDataSequenceParse()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestFeatureTemplateIsLegal()" << endl; if(TestFeatureTemplateIsLegal()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestLeftRightFeatures()" << endl; if(TestLeftRightFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestEdgeFeatures()" << endl; if(TestEdgeFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestReorderData()" << endl; if(TestReorderData()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestSequenceFeatures()" << endl; if(TestSequenceFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestFeatureSequence Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    HyperEdge edge00, edge11, edge22, edge12t, edge12nt, edge02;
    Ranks cal;
    FeatureDataSequence sent, sent_pos;

};

}

#endif
