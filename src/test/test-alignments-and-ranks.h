#ifndef TEST_ALIGNMENTS_AND_RANKS_H__
#define TEST_ALIGNMENTS_AND_RANKS_H__

#include "test-base.h"
#include <kyldr/alignment.h>
#include <kyldr/combined-alignment.h>
#include <kyldr/ranks.h>

namespace kyldr {

class TestAlignmentsAndRanks : public TestBase {

public:

    TestAlignmentsAndRanks() { }

    ~TestAlignmentsAndRanks() {
    }

    // Test whether simple combination of alignments works
    // 0-0 0-1 -> 0-[0-1]
    // 1-1 1-3 -> 1-[1-3]
    int TestSimpleCombination() {
        vector<string> words;
        words.push_back("a");
        words.push_back("b");
        Alignment al(MakePair(2,4));
        al.AddAlignment(MakePair(0,0));
        al.AddAlignment(MakePair(0,1));
        al.AddAlignment(MakePair(1,1));
        al.AddAlignment(MakePair(1,3));
        CombinedAlign cal;
        cal.BuildFromAlignment(words, al);
        int ret = 1;
        if(cal[0] != MakePair(0.0,1.0)) {
            cout << " @ 0 != 0-1 (" <<cal[0] << ")" << endl; ret = 0;
        }
        if(cal[1] != MakePair(1.0,3.0)) {
            cout << " @ 1 != 1-3 (" <<cal[1] << ")" << endl; ret = 0;
        }
        if(al.GetSrcLen() != cal.GetSrcLen()) {
            cout << " al.GetSrcLen() " << al.GetSrcLen()
                 << " != cal.GetSrcLen() " << cal.GetSrcLen() << endl; ret = 0;
        }
        return ret;
    }
    
    // Test whether blocking works
    // x......
    // xx.....
    // .x.....
    // .x.....
    // ...x.x.
    // ....x.x
    int TestBlockingCombination() {
        vector<string> words(7, "x");
        Alignment al(MakePair(7,6));
        al.AddAlignment(MakePair(0,0));
        al.AddAlignment(MakePair(0,1));
        al.AddAlignment(MakePair(1,1));
        al.AddAlignment(MakePair(1,3));
        al.AddAlignment(MakePair(3,4));
        al.AddAlignment(MakePair(4,5));
        al.AddAlignment(MakePair(5,4));
        al.AddAlignment(MakePair(6,5));
        CombinedAlign cal;
        cal.BuildFromAlignment(words,
                               al, 
                               CombinedAlign::LEAVE_NULL_AS_IS,
                               CombinedAlign::COMBINE_BLOCKS); 
        // Create the expected alignment, which should have two blocks and one
        // null
        vector<pair<double,double> > exp(7), act = cal.GetSpans();
        exp[0] = MakePair(0,3);
        exp[1] = MakePair(0,3);
        exp[2] = MakePair(-1,-1);
        exp[3] = MakePair(4,5);
        exp[4] = MakePair(4,5);
        exp[5] = MakePair(4,5);
        exp[6] = MakePair(4,5);
        return CheckVector(exp, act);
    }

    // Test whether brackets work
    int TestBracketCombination() {
        vector<string> words(6, "x");
        Alignment al(MakePair(6, 6));
        al.AddAlignment(MakePair(0,3));
        al.AddAlignment(MakePair(2,2));
        al.AddAlignment(MakePair(3,1));
        al.AddAlignment(MakePair(5,0));
        int ret = 1;
        // Create a value with brackets
        words[1] = "("; words[4] = ")";
        CombinedAlign cal;
        cal.BuildFromAlignment(words,
                               al, 
                               CombinedAlign::LEAVE_NULL_AS_IS,
                               CombinedAlign::COMBINE_BLOCKS,
                               CombinedAlign::ALIGN_BRACKET_SPANS);
        if(cal[4].first <= 2 || cal[4].first >= 3) {
            cerr << "Bad brackets: " << cal[4].first << endl; ret = 0;
        }
        // Create a value with quotes
        words[1] = "\""; words[4] = "\"";
        CombinedAlign cal1;
        cal1.BuildFromAlignment(words,
                               al, 
                               CombinedAlign::LEAVE_NULL_AS_IS,
                               CombinedAlign::COMBINE_BLOCKS,
                               CombinedAlign::ALIGN_BRACKET_SPANS);
        if(cal1[4].first <= 2 || cal1[4].first >= 3) {
            cerr << "Bad brackets: " << cal1[4].first << endl; ret = 0;
        }
        return ret;
    }

    // Test whether left and right combination work
    // 0-0 2-2 ->
    //   LEAVE_NULL_AS_IS  -1/-1
    //   ATTACH_NULL_LEFT  1 / 0
    //   ATTACH_NULL_RIGHT 1 / 2
    int TestAttachNull() {
        vector<string> words(5, "x");
        Alignment al(MakePair(5,5));
        al.AddAlignment(MakePair(1,1));
        al.AddAlignment(MakePair(3,3));
        CombinedAlign ca_asis, ca_left, ca_right;
        int ret = 1;
        // Test asis
        ca_asis.BuildFromAlignment(words, al, CombinedAlign::LEAVE_NULL_AS_IS);
        if(ca_asis[2] != MakePair(-1.0,-1.0)) {
            cout << " @ asis != -1/-1 (" <<ca_asis[2]<< ")" << endl;
            ret = 0;
        }
        // Test left, 2 and 4 should be attached to the left, 0 attached to the right
        ca_left.BuildFromAlignment(words, al, CombinedAlign::ATTACH_NULL_LEFT);
        if(ca_left[0] != MakePair(1.0,1.0) ||
           ca_left[2] != MakePair(1.0,1.0) ||
           ca_left[4] != MakePair(3.0,3.0)) {
            cout << " @ left0 = 1/1 == " <<ca_left[0] << endl;
            cout << " @ left2 = 1/1 == " <<ca_left[2] << endl;
            cout << " @ left4 = 3/3 == " <<ca_left[4] << endl;
            ret = 0;
        }
        // Test right, 0 and 2 should be attached to the right, 
        //   4 attached to the left
        ca_right.BuildFromAlignment(words, al, CombinedAlign::ATTACH_NULL_RIGHT);
        if(ca_right[0] != MakePair(1.0,1.0) ||
           ca_right[2] != MakePair(3.0,3.0) ||
           ca_right[4] != MakePair(3.0,3.0)) {
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

    int TestAlignmentIsLesser() {
        vector<bool> act, exp;
        AlignmentIsLesser alt;
        // Normal previous and next should work
        act.push_back(alt(MakePair(0,0), MakePair(1,1)));
        exp.push_back(true);
        act.push_back(alt(MakePair(1,1), MakePair(0,0)));
        exp.push_back(false);
        // Equal is not lesser
        act.push_back(alt(MakePair(0,1), MakePair(0,1)));
        exp.push_back(false);
        // Values where one is the same and one is lesser are lesser
        act.push_back(alt(MakePair(0,1), MakePair(1,1)));
        exp.push_back(true);
        act.push_back(alt(MakePair(0,0), MakePair(0,1)));
        exp.push_back(true);
        // Values where one is the same and one is greater are not lesser
        act.push_back(alt(MakePair(1,1), MakePair(0,1)));
        exp.push_back(false);
        act.push_back(alt(MakePair(0,1), MakePair(0,0)));
        exp.push_back(false);
        // Values where one is greater and one is lesser are not lesser
        act.push_back(alt(MakePair(1,1), MakePair(0,2)));
        exp.push_back(false);
        act.push_back(alt(MakePair(0,2), MakePair(1,1)));
        exp.push_back(false);
        return CheckVector(exp,act);
    }

    // Test to make sure that sorting of the ranks works properly
    int TestRanks() {
        // Create an aligment that looks like this
        // xx....
        // ...x..
        // ..x...
        // ....x.
        // .....x
        // ....x.
        vector<string> words(6, "x");
        Alignment al(MakePair(6,6));
        al.AddAlignment(MakePair(0,0));
        al.AddAlignment(MakePair(1,0));
        al.AddAlignment(MakePair(2,2));
        al.AddAlignment(MakePair(3,1));
        al.AddAlignment(MakePair(4,3));
        al.AddAlignment(MakePair(4,5));
        al.AddAlignment(MakePair(5,4));
        CombinedAlign my_cal(words, al);
        Ranks my_ranks(my_cal);
        // Which should have ranks
        // 0 0 2 1 3 3
        int ret = 1;
        std::vector<int> act = my_ranks.GetRanks(), exp(6);
        exp[0] = 0; exp[1] = 0; exp[2] = 2;
        exp[3] = 1; exp[4] = 3; exp[5] = 3;
        ret = CheckVector(exp, act);
        // And a maximum rank of 3 
        if(my_ranks.GetMaxRank() != 3) {
            cerr << "GetMaxRank() != 3: " << my_ranks.GetMaxRank() << endl;
            ret = 0;
        }
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestSimpleCombination()" << endl; if(TestSimpleCombination()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestBlockingCombination()" << endl; if(TestBlockingCombination()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestBracketCombination()" << endl; if(TestBracketCombination()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAttachNull()" << endl; if(TestAttachNull()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAlignmentReadWrite()" << endl; if(TestAlignmentReadWrite()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAlignmentIsLesser()" << endl; if(TestAlignmentIsLesser()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestRanks()" << endl; if(TestRanks()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestAlignmentsAndRanks Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

};

}

#endif
