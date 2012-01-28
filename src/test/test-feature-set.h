#ifndef TEST_FEATURE_SET_H__
#define TEST_FEATURE_SET_H__

#include "test-base.h"
#include <kyldr/feature-sequence.h>
#include <kyldr/feature-set.h>

namespace kyldr {

class TestFeatureSet : public TestBase {

public:

    TestFeatureSet() {
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
        // Create edges that reproduce the alignment in two ways
        node00 = HyperNode(0, HyperSpan(0,0,MakePair(0,0),MakePair(0,0)));
        edge00 = HyperEdge(0, HyperEdge::EDGE_TERMSTR);
        node00.AddEdge(&edge00);
        node11 = HyperNode(1, HyperSpan(1,1,MakePair(2,2),MakePair(2,2)));
        edge11 = HyperEdge(1, HyperEdge::EDGE_TERMSTR);
        node11.AddEdge(&edge11);
        node22 = HyperNode(2, HyperSpan(2,2,MakePair(1,1),MakePair(1,1)));
        edge22 = HyperEdge(2, HyperEdge::EDGE_TERMSTR);
        node22.AddEdge(&edge22);
        node12 = HyperNode(3, HyperSpan(1,2,MakePair(1,1),MakePair(2,2)));
        edge12nt = HyperEdge(3, HyperEdge::EDGE_INV, &node11, &node22);
        node12.AddEdge(&edge12nt);
        edge12t = HyperEdge(4, HyperEdge::EDGE_TERMINV);
        node12.AddEdge(&edge12t);
        node02 = HyperNode(4, HyperSpan(0,2,MakePair(0,0),MakePair(2,2)));
        edge02 = HyperEdge(5, HyperEdge::EDGE_STR, &node00, &node12);
        node02.AddEdge(&edge02);
    }

    int TestSetEdgeFeatures() {
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
        set.AddEdgeFeatures(datas, node02, edge02);
        FeatureVectorString edge02act =
                set.StringifyFeatureIndices(edge02.GetFeatureVector());
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(edge02exp, edge02act);
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

    int TestSetNodeFeatures() {
        // Set up the feature generators
        FeatureSequence *featw = new FeatureSequence,
                        *featp = new FeatureSequence;
        featw->ParseConfiguration("SW%SS");
        featp->ParseConfiguration("SP%SS");
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
        FeatureVectorString node00exp;
        node00exp.push_back(MakePair(string("SW||he"), 1));
        node00exp.push_back(MakePair(string("SP||PRP"), 1));
        // Generate the features
        set.AddNodeFeatures(datas, node00);
        FeatureVectorString node00act =
                set.StringifyFeatureIndices(node00.GetFeatureVector());
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(node00exp, node00act);
        return ret;
    }

    int TestFeatureSetIO() {
        // Set up the data
        vector<FeatureDataBase*> datas;
        datas.push_back(&sent);
        datas.push_back(&sent_pos);
        // Create and generate some features
        FeatureSet exp;
        exp.ParseConfiguration("seq=S%SS|seq=T%ET,L%ET%LL");
        exp.AddNodeFeatures(datas, node00);
        exp.AddEdgeFeatures(datas, node02, edge02);
        // Read and write toa  stream
        ostringstream oss;
        exp.ToStream(oss);
        istringstream iss(oss.str());
        FeatureSet * act = FeatureSet::FromStream(iss);
        int ret = (exp == *act) ? 1 : 0;
        delete act;
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestSetNodeFeatures()" << endl; if(TestSetNodeFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestSetEdgeFeatures()" << endl; if(TestSetEdgeFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestFeatureSetFromConfiguration()" << endl; if(TestFeatureSetFromConfiguration()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestFeatureSetIO()" << endl; if(TestFeatureSetIO()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestFeatureSet Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    HyperEdge edge00, edge11, edge22, edge02, edge12t, edge12nt;
    HyperNode node00, node11, node22, node12, node02;
    CombinedAlignment cal;
    FeatureDataSequence sent, sent_pos;

};

}

#endif
