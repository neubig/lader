#ifndef TEST_FEATURE_SET_H__
#define TEST_FEATURE_SET_H__

#include "test-base.h"
#include <kyldr/feature-sequence.h>
#include <kyldr/feature-set.h>

namespace kyldr {

class TestFeatureSet : public TestBase {

public:

    TestFeatureSet() : 
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

    int TestMakeEdgeFeatures() {
        // Set up the feature generators
        FeatureSequence *featw = new FeatureSequence,
                        *featp = new FeatureSequence;
        featw->ParseConfiguration("SW%LS%RS");
        featp->ParseConfiguration("SP%LS%RS");
        // Set up the feature set
        FeatureSet set;
        set.AddFeatureGenerator(featw);
        set.AddFeatureGenerator(featp);
        // Set up the data
        vector<FeatureDataBase*> datas;
        datas.push_back(&sent);
        datas.push_back(&sent_pos);
        // These are both node-factored features, so they should exist for all
        // nodes, but no edges
        FeatureVectorString edge02exp;
        edge02exp.push_back(MakePair(string("SW||he||ate rice"), 1));
        edge02exp.push_back(MakePair(string("SP||PRP||VBD NN"), 1));
        // Generate the features
        FeatureVectorInt * edge02int = set.MakeEdgeFeatures(datas, edge02);
        FeatureVectorString edge02act =
                                    set.StringifyFeatureIndices(*edge02int);
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(edge02exp, edge02act);
        delete edge02int;
        return ret;
    }

    int TestFeatureSetFromConfiguration() {
        FeatureSequence seq0, seq1;
        seq0.ParseConfiguration("S%ET%SS,T%ET");
        seq1.ParseConfiguration("B%LL%RR");
        FeatureSet set;
        set.ParseConfiguration("seq=S%ET%SS,T%ET|seq=B%LL%RR");
        return ((seq0.CheckEqual(*set.GetGenerator(0))) && 
                (seq1.CheckEqual(*set.GetGenerator(1)))) ? 1 : 0;
    }

    int TestFeatureSetIO() {
        // Set up the data
        vector<FeatureDataBase*> datas;
        datas.push_back(&sent);
        datas.push_back(&sent_pos);
        // Create and generate some features
        FeatureSet exp;
        exp.ParseConfiguration("seq=S%SS|seq=T%ET,L%ET%LL");
        FeatureVectorInt * feat = exp.MakeEdgeFeatures(datas, edge02);
        // Read and write toa  stream
        ostringstream oss;
        exp.ToStream(oss);
        istringstream iss(oss.str());
        FeatureSet * act = FeatureSet::FromStream(iss);
        int ret = (exp == *act) ? 1 : 0;
        delete feat;
        delete act;
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestMakeEdgeFeatures()" << endl; if(TestMakeEdgeFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestFeatureSetFromConfiguration()" << endl; if(TestFeatureSetFromConfiguration()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestFeatureSetIO()" << endl; if(TestFeatureSetIO()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestFeatureSet Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    HyperEdge edge00, edge11, edge22, edge12t, edge12nt, edge02;
    CombinedAlignment cal;
    FeatureDataSequence sent, sent_pos;

};

}

#endif
