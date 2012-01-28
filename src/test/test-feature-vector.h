#ifndef TEST_VECTOR_SUBTRACT_H__
#define TEST_VECTOR_SUBTRACT_H__

#include "test-base.h"
#include <kyldr/feature-vector.h>

namespace kyldr {

class TestFeatureVector : public TestBase {

public:

    TestFeatureVector() { }

    ~TestFeatureVector() { }

    // Test whether vector subtraction works
    int TestVectorSubtract() {
        FeatureVectorInt v1, v2, exp, act;
        v1.push_back(MakePair(1,1));
        v1.push_back(MakePair(3,3));
        v1.push_back(MakePair(5,5));
        v2.push_back(MakePair(2,2));
        v2.push_back(MakePair(3,3));
        v2.push_back(MakePair(4,4));
        exp.push_back(MakePair(1,1));
        exp.push_back(MakePair(2,-2));
        exp.push_back(MakePair(4,-4));
        exp.push_back(MakePair(5,5));
        act = VectorSubtract(v1,v2);
        return CheckVector(exp,act);
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestVectorSubtract()" << endl; if(TestVectorSubtract()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestFeatureVector Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

};

}

#endif
