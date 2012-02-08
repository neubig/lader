#ifndef TEST_KYLDR_DICTIONARY_H__
#define TEST_KYLDR_DICTIONARY_H__

#include "test-base.h"
#include <kyldr/dictionary.h>

namespace kyldr {

class TestDictionary : public TestBase {

public:

    TestDictionary() { }

    ~TestDictionary() { }

    // Test whether vector subtraction works
    int TestToFromStream() {
        Dictionary exp;
        vector<double> vec1(2,0), vec2(1,0);
        vec1[0] = 0.1; vec1[1] = 0.5; vec2[0] = 0.2;
        exp.AddEntry("a", vec1); exp.AddEntry("a test", vec2);
        ostringstream oss; exp.ToStream(oss);
        istringstream iss(oss.str());
        Dictionary * act = Dictionary::FromStream(iss);
        int ret = 1;
        if(exp != *act) {
            cerr << "Dictionaries don't match" << endl;
            ret = 0;
        }
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestToFromStream()" << endl; if(TestToFromStream()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestDictionary Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

};

}

#endif
