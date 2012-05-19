#ifndef TEST_FEATURE_PARSE_H__
#define TEST_FEATURE_PARSE_H__

#include "test-base.h"
#include <lader/combined-alignment.h>
#include <lader/feature-data-parse.h>
#include <lader/feature-set.h>
#include <lader/feature-parse.h>
#include <fstream>

namespace lader {

class TestFeatureParse : public TestBase {

public:

    TestFeatureParse() :
            edge00(0, -1, 0, HyperEdge::EDGE_FOR), 
            edge02(0, 1, 2, HyperEdge::EDGE_INV),
            edge13(1, 2, 3, HyperEdge::EDGE_STR) {
        // Create the real value
        string str = "(S (N this) (VP (V is) (NP (DET a) (N test))))";
        fdp.FromString(str);
    }
    ~TestFeatureParse() { }

    int TestFeatureDataParse() {
        int ret = 1;
        // Create the expected/actual value of the sequence
        vector<string> act_seq = fdp.GetSequence(), exp_seq;
        exp_seq.push_back("this");
        exp_seq.push_back("is");
        exp_seq.push_back("a");
        exp_seq.push_back("test");
        ret *= CheckVector(exp_seq, act_seq);
        // Check the spans
        vector<string> act_span(10), exp_span(10);
        act_span[0] = fdp.GetSpanLabel(0,0); exp_span[0] = "N";
        act_span[1] = fdp.GetSpanLabel(1,1); exp_span[1] = "V";
        act_span[2] = fdp.GetSpanLabel(2,2); exp_span[2] = "DET";
        act_span[3] = fdp.GetSpanLabel(3,3); exp_span[3] = "N";
        act_span[4] = fdp.GetSpanLabel(0,1); exp_span[4] = "X";
        act_span[5] = fdp.GetSpanLabel(1,2); exp_span[5] = "X";
        act_span[6] = fdp.GetSpanLabel(2,3); exp_span[6] = "NP";
        act_span[7] = fdp.GetSpanLabel(0,2); exp_span[7] = "X";
        act_span[8] = fdp.GetSpanLabel(1,3); exp_span[8] = "VP";
        act_span[9] = fdp.GetSpanLabel(0,3); exp_span[9] = "S";
        ret *= CheckVector(exp_span, act_span);
        return ret;
    }

    // Feature templates include
    // ST, LT, RT for the tags of each span
    int TestFeatureTemplateIsLegal() {
        const int num = 12;
        const char* templ[num] = { "SP", "LP", "RP", "CD", "ET",
                                   "XY", "SD", "CR", "SQE", "LQ1",
                                   "LQE1", "SQ#04" };
        const bool exp[num] = {true, true, true, false, true,
                               false, false, false, false, false,
                               false, false};
        int ret = 1;
        for(int i = 0; i < num; i++) {
            if(FeatureParse::FeatureTemplateIsLegal(templ[i]) != exp[i]) {
                cout << "FeatureTemplateIsLegal failed on " << templ[i] << endl;
                ret = 0;
            }
        }
        return ret;
    }

    int TestLeftRightFeatures() {
        ReordererModel mod;
        FeatureParse feat;
        feat.ParseConfiguration("L%LP,R%RP,S%SP");
        // These features apply to only non-terminals
        FeatureVectorString edge00exp, edge02exp, edge13exp;
        edge00exp.push_back(MakePair(string("S||N"), 1));
        edge02exp.push_back(MakePair(string("L||N"), 1));
        edge02exp.push_back(MakePair(string("R||X"), 1));
        edge02exp.push_back(MakePair(string("S||X"), 1));
        edge13exp.push_back(MakePair(string("L||V"), 1));
        edge13exp.push_back(MakePair(string("R||NP"), 1));
        edge13exp.push_back(MakePair(string("S||VP"), 1));
        // Create vectors
        FeatureVectorString edge00act, edge02act, edge13act; 
        feat.GenerateEdgeFeatures(fdp, edge00, edge00act);
        feat.GenerateEdgeFeatures(fdp, edge02, edge02act);
        feat.GenerateEdgeFeatures(fdp, edge13, edge13act);
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(edge00exp, edge00act);
        ret *= CheckVector(edge02exp, edge02act);
        ret *= CheckVector(edge13exp, edge13act);
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestFeatureDataParse()" << endl; if(TestFeatureDataParse()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestFeatureTemplateIsLegal()" << endl; if(TestFeatureTemplateIsLegal()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestLeftRightFeatures()" << endl; if(TestLeftRightFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestFeatureParse Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    FeatureDataParse fdp;
    HyperEdge edge00, edge02, edge13;

};

}

#endif
