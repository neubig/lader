#ifndef TEST_HYPER_GRAPH_H__
#define TEST_HYPER_GRAPH_H__

#include "test-base.h"
#include <kyldr/hyper-graph.h>
#include <kyldr/alignment.h>

namespace kyldr {

class TestHyperGraph : public TestBase {

public:

    TestHyperGraph() {
        span0 = HyperSpan(0,0,MakePair(0,0),MakePair(0,0));
        span1 = HyperSpan(1,1,MakePair(0,0),MakePair(0,0));
        span2 = HyperSpan(2,2,MakePair(1,1),MakePair(1,1));
        // Create a combined alignment
        //  x..
        //  ..x
        //  .x.
        Alignment al(3,3);
        al.AddAlignment(0,0);
        al.AddAlignment(1,2);
        al.AddAlignment(2,1);
        cal1 = CombinedAlignment(al);
    }

    ~TestHyperGraph() {
    }

    // Test whether getting a node at a particular span works
    int TestGetNodeAtSpan() {
        HyperGraph hyper_graph;
        hyper_graph.InitializeStructures(3,2);
        int ret = 1;
        // Get the first node
        HyperNode * node0 = hyper_graph.GetNodeAtSpan(span0);
        // Check to make sure the span is equal
        if(node0->GetSpan() != span0) {
            cout << "node0->GetSpan() != span0: " << node0->GetSpan() << " != " << span0 << endl; ret = 0;
        }
        if(node0->GetIdx() != 0) {
            cout << "node0->GetIdx() != 0: " << node0->GetIdx() << " != 0" << endl; ret = 0;
        }
        // Get the second node
        HyperNode * node1 = hyper_graph.GetNodeAtSpan(span1);
        // Check to make sure the span is equal
        if(node1->GetSpan() != span1) {
            cout << "node1->GetSpan() != span1: " << node1->GetSpan() << " != " << span1 << endl; ret = 0;
        }
        if(node1->GetIdx() != 1) {
            cout << "node1->GetIdx() != 1: " << node1->GetIdx() << " != 1" << endl; ret = 1;
        }
        // Check to make sure that we can retrieve the old span
        HyperNode * node0copy = hyper_graph.GetNodeAtSpan(span0);
        if(node0copy->GetIdx() != 0) {
            cout << "node0copy->GetIdx() != 0: " << node0copy->GetIdx() << " != 0" << endl; ret = 0;
        }
        return ret;
    }

    // Test whether we can add single terminals with no reversal
    int TestAddSingleTerminals() {
        HyperGraph hyper_graph(3,3);
        hyper_graph.AddTerminals(cal1, 1, false);
        int ret = 1;
        // Check that the number of nodes is 3
        const vector<HyperNode*> & nodes = hyper_graph.GetNodes();
        if(nodes.size() != 3) {
            cout << "nodes.size() "<<nodes.size()<<" != 3"<<endl; ret = 0;
        }
        const vector<HyperEdge*> & edges = hyper_graph.GetEdges();
        if(edges.size() != 3) {
            cout << "edges.size() "<<edges.size()<<" != 3"<<endl; ret = 0;
        }
        return ret;
    }

    // Test whether we can add single terminals with reversal
    int TestAddReverseTerminals() {
        HyperGraph hyper_graph(3,3);
        hyper_graph.AddTerminals(cal1, 1, true);
        int ret = 1;
        // Check that the number of nodes is 3
        const vector<HyperNode*> & nodes = hyper_graph.GetNodes();
        if(nodes.size() != 3) {
            cout << "nodes.size() "<<nodes.size()<<" != 3"<<endl; ret = 0;
        }
        const vector<HyperEdge*> & edges = hyper_graph.GetEdges();
        if(edges.size() != 6) {
            cout << "edges.size() "<<edges.size()<<" != 6"<<endl; ret = 0;
        }
        return ret;
    }

    // Test whether we can add multi-word terminals with no reversal
    int TestAddMultipleTerminals() {
        HyperGraph hyper_graph(3,3);
        hyper_graph.AddTerminals(cal1, 2, false);
        int ret = 1;
        // Check that the number of nodes is 3
        const vector<HyperNode*> & nodes = hyper_graph.GetNodes();
        if(nodes.size() != 5) {
            cout << "nodes.size() "<<nodes.size()<<" != 5"<<endl; ret = 0;
        }
        const vector<HyperEdge*> & edges = hyper_graph.GetEdges();
        if(edges.size() != 5) {
            cout << "edges.size() "<<edges.size()<<" != 5"<<endl; ret = 0;
        }
        return ret;
    }
    
    // Test whether we can add multi-word terminals with no reversal
    int TestAddNonTerminals() {
        HyperGraph hyper_graph(3,3);
        // Add the terminals
        // E0  (T,<0,0,0,0>)  -> <0,0,0,0> X0
        // E1  (T,<1,1,2,2>)  -> <1,1,2,2> X1
        // E2  (T,<2,2,1,1>)  -> <2,2,1,1> X2
        hyper_graph.AddTerminals(cal1, 1, false);
        // Add the non-terminals (*** are existing nodes)
        // E3  (S,X0,X1)      -> <0,1,0,2> X3
        // E4  (I,X0,X1)      -> <0,1,2,0> X4
        // E5  (S,X1,X2)      -> <1,2,2,1> X5
        // E6  (I,X1,X2)      -> <1,2,1,2> X6
        // E7  (S,X3,X2)      -> <0,2,0,1> X7
        // E8  (I,X3,X2)      -> <0,2,1,2> X8
        // E9  (S,X4,X2)      -> <0,2,2,1> X9
        // E10 (I,X4,X2)      -> <0,2,1,0> X10
        // E11 (S,X0,X5)      -> <0,2,0,1> X7  ***
        // E12 (I,X0,X5)      -> <0,2,2,0> X11
        // E13 (S,X0,X6)      -> <0,2,0,2> X12
        // E14 (I,X0,X6)      -> <0,2,1,0> X10 ***
        hyper_graph.AddNonTerminals();
        int ret = 1;
        // Check that the number of nodes is 3
        const vector<HyperNode*> & nodes = hyper_graph.GetNodes();
        if(nodes.size() != 13) {
            cout << "nodes.size() "<<nodes.size()<<" != 13"<<endl; ret = 0;
        }
        const vector<HyperEdge*> & edges = hyper_graph.GetEdges();
        if(edges.size() != 15) {
            cout << "edges.size() "<<edges.size()<<" != 15"<<endl; ret = 0;
        }
        return ret;
    }

    // Test that we can calculate the best cumulative score
    int TestCumulativeScore() {
        // Build the graph
        HyperGraph hyper_graph(2,2);
        HyperNode * node00 = hyper_graph.GetNodeAtSpan(
            HyperSpan(0,0,MakePair(0,0),MakePair(0,0)));
        HyperEdge * edge00 = hyper_graph.AddNewEdge(HyperEdge::EDGE_TERMSTR);
        edge00->SetScore(1);
        node00->AddEdge(edge00);
        node00->SetScore(2);
        HyperNode * node11 = hyper_graph.GetNodeAtSpan(
            HyperSpan(1,1,MakePair(1,1),MakePair(1,1)));
        HyperEdge * edge11 = hyper_graph.AddNewEdge(HyperEdge::EDGE_TERMSTR);
        edge11->SetScore(4);
        node11->AddEdge(edge11);
        HyperEdge * edge11b = hyper_graph.AddNewEdge(HyperEdge::EDGE_TERMINV);
        edge11b->SetScore(8);
        node11->AddEdge(edge11b);
        HyperNode * node01 = hyper_graph.GetNodeAtSpan(
            HyperSpan(0,1,MakePair(0,0),MakePair(1,1)));
        HyperEdge * edge01 = hyper_graph.AddNewEdge(HyperEdge::EDGE_STR);
        edge01->SetLeftChild(node00);
        edge01->SetRightChild(node11);
        edge01->SetScore(16);
        node01->AddEdge(edge01);
        node01->SetScore(32);
        // The cumulative score should be:
        // edge00=1 + node00=2 + edge11b=8 + edge01=16 + node01=32 --> 59
        int ret = 1;
        if(node01->GetCumulativeScore() != 59) {
            cout << "node01->GetCumulativeScore() " << node01->GetCumulativeScore() << " != 59" << endl; ret = 0;
        }
        return ret;
    }

private:

    HyperSpan span0, span1, span2;
    CombinedAlignment cal1;

public:

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestGetNodeAtSpan()" << endl; if(TestGetNodeAtSpan()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAddSingleTerminals()" << endl; if(TestAddSingleTerminals()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAddReverseTerminals()" << endl; if(TestAddReverseTerminals()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAddMultipleTerminals()" << endl; if(TestAddMultipleTerminals()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAddNonTerminals()" << endl; if(TestAddNonTerminals()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestCumulativeScore()" << endl; if(TestCumulativeScore()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestHyperGraph Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }


};

}

#endif
