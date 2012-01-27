#ifndef TEST_COMBINED_ALIGNMENT_H__
#define TEST_COMBINED_ALIGNMENT_H__

#include "test-base.h"
#include <kyldr/alignment.h>
#include <kyldr/combined-alignment.h>

namespace kyldr {

class TestCombinedAlignment : public TestBase {

public:

    TestCombinedAlignment() { }

    ~TestCombinedAlignment() {
    }

    // Test whether simple combination of alignments works
    // 0-0 0-1 -> 0-[0-1]
    // 1-1 1-3 -> 1-[1-3]
    int TestSimpleCombination() {
        Alignment al(MakePair(2,4));
        al.AddAlignment(MakePair(0,0));
        al.AddAlignment(MakePair(0,1));
        al.AddAlignment(MakePair(1,1));
        al.AddAlignment(MakePair(1,3));
        CombinedAlignment cal;
        cal.BuildFromAlignment(al);
        int ret = 1;
        if(cal[0] != MakePair(0,1)) {
            cout << " @ 0 != 0-1 (" <<cal[0] << ")" << endl; ret = 0;
        }
        if(cal[1] != MakePair(1,3)) {
            cout << " @ 1 != 1-3 (" <<cal[1] << ")" << endl; ret = 0;
        }
        if(al.GetSrcLen() != cal.GetSrcLen()) {
            cout << " al.GetSrcLen() " << al.GetSrcLen()
                 << " != cal.GetSrcLen() " << cal.GetSrcLen() << endl; ret = 0;
        }
        if(al.GetTrgLen() != cal.GetTrgLen()) {
            cout << " al.GetTrgLen() " << al.GetTrgLen()
                 << " != cal.GetTrgLen() " << cal.GetTrgLen() << endl; ret = 0;
        }
        return ret;
    }

    // Test whether left and right combination work
    // 0-0 2-2 ->
    //   LEAVE_NULL_AS_IS  -1/-1
    //   ATTACH_NULL_LEFT  1 / 0
    //   ATTACH_NULL_RIGHT 1 / 2
    int TestAttachNull() {
        Alignment al(MakePair(5,5));
        al.AddAlignment(MakePair(1,1));
        al.AddAlignment(MakePair(3,3));
        CombinedAlignment ca_asis, ca_left, ca_right;
        int ret = 1;
        // Test asis
        ca_asis.BuildFromAlignment(al, CombinedAlignment::LEAVE_NULL_AS_IS);
        if(ca_asis[2] != MakePair(-1,-1)) {
            cout << " @ asis != -1/-1 (" <<ca_asis[2]<< ")" << endl;
            ret = 0;
        }
        // Test left, 2 and 4 should be attached to the left, 0 attached to the right
        ca_left.BuildFromAlignment(al, CombinedAlignment::ATTACH_NULL_LEFT);
        if(ca_left[0] != MakePair(1,1) ||
           ca_left[2] != MakePair(1,1) ||
           ca_left[4] != MakePair(3,3)) {
            cout << " @ left0 = 1/1 == " <<ca_left[0] << endl;
            cout << " @ left2 = 1/1 == " <<ca_left[2] << endl;
            cout << " @ left4 = 3/3 == " <<ca_left[4] << endl;
            ret = 0;
        }
        // Test right, 0 and 2 should be attached to the right, 
        //   4 attached to the left
        ca_right.BuildFromAlignment(al, CombinedAlignment::ATTACH_NULL_RIGHT);
        if(ca_right[0] != MakePair(1,1) ||
           ca_right[2] != MakePair(3,3) ||
           ca_right[4] != MakePair(3,3)) {
            cout << " @ right0 = 1/1 == " <<ca_right[0] << endl;
            cout << " @ right2 = 3/3 == " <<ca_right[2] << endl;
            cout << " @ right4 = 3/3 == " <<ca_right[4] << endl;
            ret = 0;
        }
        return ret;
    }

    int TestAlignmentReadWrite() {
        Alignment exp(MakePair(2,4));
        exp.AddAlignment(MakePair(0,0));
        exp.AddAlignment(MakePair(0,1));
        exp.AddAlignment(MakePair(1,1));
        exp.AddAlignment(MakePair(1,3));
        string str = exp.ToString();
        Alignment act = Alignment::FromString(str);
        int ret = 1;
        if(exp != act) {
            cerr << "exp '" << exp.ToString() << 
                    "' != act '"<<act.ToString()<<"'" <<endl;
            ret = 0;
        }
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestSimpleCombination()" << endl; if(TestSimpleCombination()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAttachNull()" << endl; if(TestAttachNull()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAlignmentReadWrite()" << endl; if(TestAlignmentReadWrite()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestCombinedAlignment Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

};

}

#endif
