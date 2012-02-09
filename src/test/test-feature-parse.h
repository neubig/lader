#ifndef TEST_FEATURE_PARSE_H__
#define TEST_FEATURE_PARSE_H__

#include "test-base.h"
#include <kyldr/combined-alignment.h>
// #include <kyldr/feature-parse.h>
#include <kyldr/feature-data-parse.h>
#include <kyldr/feature-set.h>
#include <fstream>

namespace kyldr {

class TestFeatureParse : public TestBase {

public:

    TestFeatureParse() { }
    ~TestFeatureParse() { }

    int TestFeatureDataParse() {
        int ret = 1;
        // Create the real value
        string str = "(S (N this) (VP (V is) (NP (DET a) (N test))))";
        FeatureDataParse fdp;
        fdp.FromString(str);
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

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestFeatureDataParse()" << endl; if(TestFeatureDataParse()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestFeatureParse Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:

};

}

#endif
