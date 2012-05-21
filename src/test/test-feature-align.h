#ifndef TEST_FEATURE_ALIGN_H__
#define TEST_FEATURE_ALIGN_H__

#include "test-base.h"
#include <lader/combined-alignment.h>
#include <lader/feature-data-align.h>
#include <lader/feature-set.h>
#include <lader/feature-align.h>
#include <fstream>

namespace lader {

class TestFeatureAlign : public TestBase {

public:

    TestFeatureAlign() :
            edge00(0, -1, 0, HyperEdge::EDGE_FOR), 
            edge02(0, 1, 2, HyperEdge::EDGE_INV),
            edge13(1, 2, 3, HyperEdge::EDGE_STR) {
    }
    ~TestFeatureAlign() { }

    int TestTauFeatures() {
        FeatureAlign feat;
        feat.ParseConfiguration("left");
        // Create the real value
        string str = "this is a test ||| 4-3 ||| 0-0 1-2 2-1 3-1";
        FeatureDataBase * fda = feat.ParseData(str);
        // These features apply to only non-terminals
        SymbolSet<int> syms;
        FeatureVectorInt edge00exp, edge02exp, edge13exp;
        edge00exp.push_back(MakePair(syms.GetId("TAU", true), 0));
        edge02exp.push_back(MakePair(syms.GetId("TAU", true), 2));
        edge13exp.push_back(MakePair(syms.GetId("TAU", true), 2));
        // Create vectors
        FeatureVectorInt edge00act, edge02act, edge13act; 
        feat.GenerateEdgeFeatures(*fda, edge00, syms, true, edge00act);
        feat.GenerateEdgeFeatures(*fda, edge02, syms, true, edge02act);
        feat.GenerateEdgeFeatures(*fda, edge13, syms, true, edge13act);
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(edge00exp, edge00act);
        ret *= CheckVector(edge02exp, edge02act);
        ret *= CheckVector(edge13exp, edge13act);
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestTauFeatures()" << endl; if(TestTauFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestFeatureAlign Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    FeatureDataAlign fda;
    HyperEdge edge00, edge02, edge13;

};

}

#endif
