#ifndef TEST_HYPER_GRAPH_H__
#define TEST_HYPER_GRAPH_H__

#include "test-base.h"
#include <climits>
#include <lader/hyper-graph.h>
#include <lader/alignment.h>
#include <lader/reorderer-model.h>
#include <lader/feature-data-sequence.h>
#include <lader/feature-sequence.h>
#include <lader/feature-set.h>
#include <lader/ranks.h>
#include <lader/hyper-graph.h>
#include <fstream>
#include <time.h>

namespace lader {

class TestHyperGraph : public TestBase {

public:

    TestHyperGraph() : 
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
        vector<string> words(3,"x");
        Alignment al(MakePair(3,3));
        al.AddAlignment(MakePair(0,0));
        al.AddAlignment(MakePair(1,2));
        al.AddAlignment(MakePair(2,1));
        cal = CombinedAlign(words, al);
        // Create a sentence
        string str = "he ate rice";
        sent.FromString(str);
        string str_pos = "PRP VBD NN";
        sent_pos.FromString(str_pos);
        // Create a reorderer model with two weights
        model.SetWeight("W1", 1);
        model.SetWeight("W2", 2);
        // Set up the feature generators
        featw = new FeatureSequence;
        featp = new FeatureSequence;
        featw->ParseConfiguration("SW%LS%RS");
        featp->ParseConfiguration("SP%LS%RS");
        // Set up the feature set
        set.AddFeatureGenerator(featw);
        set.AddFeatureGenerator(featp);
        // Set up the data
        datas.push_back(&sent);
        datas.push_back(&sent_pos);

        // ------ Make a hypergraph for testing various things ------
        // Make the target side spans
        ts00 = new TargetSpan(0,0,0,0);
        ts01f = new TargetSpan(0,1,0,1);
        ts01b = new TargetSpan(0,1,1,0);
        ts11 = new TargetSpan(1,1,1,1);
        tsr = new TargetSpan(0,1,0,1);
        // Add the hypotheses
        ts00->AddHypothesis(new Hypothesis(1,1,0,0,0,0,HyperEdge::EDGE_FOR));
        ts11->AddHypothesis(new Hypothesis(2,2,1,1,1,1,HyperEdge::EDGE_FOR));
        ts01f->AddHypothesis(new Hypothesis(4,4,0,1,0,1,HyperEdge::EDGE_FOR));
        ts01f->AddHypothesis(new Hypothesis(3,3,0,1,0,1,HyperEdge::EDGE_STR,1,0,0,ts00,ts11));
        ts01b->AddHypothesis(new Hypothesis(5,5,0,1,1,0,HyperEdge::EDGE_INV,1,0,0,ts00,ts11));
        tsr->AddHypothesis(new Hypothesis(6,6,1,0,-1,2,HyperEdge::EDGE_ROOT,-1,0,-1,ts01b));
        tsr->AddHypothesis(new Hypothesis(6,6,0,1,-1,2,HyperEdge::EDGE_ROOT,-1,0,-1,ts01f));
        // Make the stacks
        SpanStack *stack00 = new SpanStack(0,0), *stack01 = new SpanStack(0,1),
                  *stack11 = new SpanStack(0,1), *stackr = new SpanStack(0,2);
        stack00->AddSpan(ts00); stack01->AddSpan(ts01f); stack01->AddSpan(ts01b);
        stack11->AddSpan(ts11); stackr->AddSpan(tsr);
        my_hg.SetStack(0,0,stack00);
        my_hg.SetStack(0,1,stack01);
        my_hg.SetStack(1,1,stack11);
        my_hg.SetStack(0,2,stackr); // Abusing SetStack to set the root
        // Add the features
        FeatureVectorInt 
            *fv00 = new FeatureVectorInt(1, MakePair(1,1)),
            *fv11 = new FeatureVectorInt(1, MakePair(2,1)),
            *fv01f = new FeatureVectorInt(1,MakePair(4,1)),
            *fv01s = new FeatureVectorInt(1,MakePair(3,1)),
            *fv01b = new FeatureVectorInt(1,MakePair(5,1));
        fv00->push_back(MakePair(10,1));
        fv11->push_back(MakePair(10,1));
        fv01f->push_back(MakePair(10,1));
        fv01s->push_back(MakePair(10,1));
        fv01b->push_back(MakePair(10,1));
        stack00->SaveStraightFeautures(-1, fv00);
        stack11->SaveStraightFeautures(-1, fv11);
        stack01->SaveStraightFeautures(-1, fv01f);
        stack01->SaveStraightFeautures(1, fv01s);
        stack01->SaveInvertedFeautures(1, fv01b);
        // features are saved in stack
        my_hg.SetSaveFeatures(true);
        // Add the loss
        ts00->GetHypothesis(0)->SetLoss(1);
        ts11->GetHypothesis(0)->SetLoss(2);
        ts01f->GetHypothesis(0)->SetLoss(4);
        ts01f->GetHypothesis(1)->SetLoss(3);
        ts01b->GetHypothesis(0)->SetLoss(5);
        tsr->GetHypothesis(0)->SetLoss(6);
        // // Sort the stacks so we get the best value first
        // BOOST_FOREACH(SpanStack & stack, my_hg.GetStacks())
        //     stack.SortSpans(true);
    }

    int TestGetTrgSpanID() {
        vector<pair<int,int> > in;
        in.push_back(MakePair(0,0));
        in.push_back(MakePair(0,1));
        in.push_back(MakePair(1,1));
        in.push_back(MakePair(0,2));
        in.push_back(MakePair(1,2));
        in.push_back(MakePair(2,2));
        int ret = 1;
        HyperGraph hg;
        for(int i = 0; i < (int)in.size(); i++) {
            if(hg.GetTrgSpanID(in[i].first, in[i].second) != i) {
                cerr << "hg.GetTrgSpanID("<<in[i].first<<", "<<in[i].second<<") == " << hg.GetTrgSpanID(in[i].first, in[i].second) << endl; ret = 0;
            }
        }
        return ret;
    }

    int TestGetEdgeFeaturesAndWeights() {
        // Make a reorderer model
        ReordererModel mod;
        // Test that these features are made properly
        FeatureVectorString edge02exp;
        edge02exp.push_back(MakePair(string("SW||he||ate rice"), 1));
        edge02exp.push_back(MakePair(string("SP||PRP||VBD NN"), 1));
        // Make the hypergraph and get the features
        HyperGraph hyper_graph;
        // -save_features
        hyper_graph.SetSaveFeatures(true);
        // for Save{Striaght,Inverted}Features
        SpanStack * stack02 = new SpanStack(0, 2);
        hyper_graph.SetStack(0, 2, stack02);
        // Generate the features
        const FeatureVectorInt * edge02int = 
                        hyper_graph.GetEdgeFeatures(mod, set, datas, edge02);
        FeatureVectorString * edge02act =
                        mod.StringifyFeatureVector(*edge02int);
        // Do the parsing and checking
        int ret = 1;
        ret *= CheckVector(edge02exp, *edge02act);

        FeatureVectorInt edge02intexp;
        edge02intexp.push_back(MakePair(mod.GetFeatureIds().GetId("SW||he||ate rice"), 1));
        edge02intexp.push_back(MakePair(mod.GetFeatureIds().GetId("SP||PRP||VBD NN"), 1));
        ret *= CheckVector(edge02intexp, *edge02int);
        // Generate the features again
        const FeatureVectorInt * edge02int2 = 
                        hyper_graph.GetEdgeFeatures(mod, set, datas, edge02);
        // Make sure that the pointers are equal
        if(edge02int != edge02int2) {
            cerr << "Edge pointers are not equal." << endl;
            ret = 0;
        }
        mod.SetWeight("SW||he||ate rice", 1);
        mod.SetWeight("SP||PRP||VBD NN", 2);
        // Check to make sure that the weights are Ok
        double weight_act = hyper_graph.GetEdgeScore(mod, set,
                                                     datas, edge02);
        if(weight_act != 1+2) {
            cerr << "Weight is not the expected 3: "<<weight_act<<endl;
            ret = 0;
        }
        return ret;
    }

    // Test the processing of a single span
    int TestProcessOneSpan() {
        HyperGraph graph;
        ReordererModel model;
        model.SetMaxTerm(0);
    	model.SetUseReverse(true);
        // Create two spans for 00 and 11, so we can process 01
        SpanStack *stack00 = new SpanStack(0,0), *stack11 = new SpanStack(1,1);
        stack00->push_back(new TargetSpan(0,0,0,0));
        (*stack00)[0]->AddHypothesis(new Hypothesis(1,1.0,0,0,0,0,HyperEdge::EDGE_FOR));
        graph.SetStack(0, 0, stack00);
        stack11->push_back(new TargetSpan(1,1,1,1));
        (*stack11)[0]->AddHypothesis(new Hypothesis(2,2.0,1,1,1,1,HyperEdge::EDGE_FOR));
        graph.SetStack(1, 1, stack11);
        // Try processing 01
        SpanStack *stack01 = new SpanStack(0, 1);
    	graph.SetStack(0, 1, stack01);
        graph.ProcessOneSpan(model, set, datas, 0, 1);
        // The stack should contain two target spans I(1,0) and S(0,1),
        // each with two hypotheses
        int ret = 1;
        if(stack01->size() != 2) {
            cerr << "stack01->size() != 2: " << stack01->size() << endl; ret = 0;
        } else if((*stack01)[0]->size() != 2) {
            cerr << "(*stack01)[0].size() != 2: " << (*stack01)[0]->size() << endl; ret = 0;
        } else if((*stack01)[1]->size() != 2) {
            cerr << "(*stack01)[1].size() != 2: " << (*stack01)[1]->size() << endl; ret = 0;
        }
        if(!ret) return 0;
        // Check to make sure that the scores are in order
        vector<double> score_exp(4,0), score_act(4);
        score_exp[0] = 3; score_exp[2] = 3;
        score_act[0] = (*stack01)[0]->GetHypothesis(0)->GetScore();
        score_act[1] = (*stack01)[0]->GetHypothesis(1)->GetScore();
        score_act[2] = (*stack01)[1]->GetHypothesis(0)->GetScore();
        score_act[3] = (*stack01)[1]->GetHypothesis(1)->GetScore();
        ret = CheckVector(score_exp, score_act);
        // Check to make sure that pruning works
        stack01->Clear();
		graph.ProcessOneSpan(model, set, datas, 0, 1, 3);
        if(stack01->size() != 2) {
            cerr << "stack01pruned->size() != 2: " << stack01->size() << endl; ret = 0;
        } else if((*stack01)[0]->size() != 2) {
            cerr << "(*stack01pruned)[0].size() != 2: " << (*stack01)[0]->size() << endl; ret = 0;
        } else if((*stack01)[1]->size() != 1) {
            cerr << "(*stack01pruned)[1].size() != 1: " << (*stack01)[1]->size() << endl; ret = 0;
        }
        return ret;
    }

    // Test the processing of a single span
    int TestProcessOneSpanNoSave() {
        HyperGraph graph;
        ReordererModel model;
        model.SetMaxTerm(0);
    	model.SetUseReverse(true);
        // Create two spans for 00 and 11, so we can process 01
        SpanStack *stack00 = new SpanStack(0,0), *stack11 = new SpanStack(1,1);
        stack00->push_back(new TargetSpan(0,0,-1,-1));
        (*stack00)[0]->AddHypothesis(new Hypothesis(1,1.0,0,0,-1,-1,HyperEdge::EDGE_FOR));
        graph.SetStack(0, 0, stack00);
        stack11->push_back(new TargetSpan(1,1,-1,-1));
        (*stack11)[0]->AddHypothesis(new Hypothesis(2,2.0,1,1,-1,-1,HyperEdge::EDGE_FOR));
        graph.SetStack(1, 1, stack11);
        // Try processing 01

        SpanStack *stack01 = new SpanStack(0, 1);
        graph.SetStack(0, 1, stack01);
        graph.ProcessOneSpan(model, set, datas, 0, 1, 0, false);
        // The stack should contain two target spans (1,0) and (0,1),
        // each with two hypotheses
        int ret = 1;
        if(stack01->size() != 1) {
            cerr << "stack01->size() != 1: " << stack01->size() << endl; ret = 0;
            for(int i = 0; i < (int)stack01->size(); i++)
                cerr << " " << i << ": " << (*stack01)[i]->GetTrgLeft() << ", " <<(*stack01)[i]->GetTrgRight() << endl;
        } else if((*stack01)[0]->GetHypotheses().size() != 4) {
            cerr << "(*stack01)[0].size() != 4: " << (*stack01)[0]->GetHypotheses().size() << endl; ret = 0;
        }
        if(!ret) return 0;
        // Check to make sure that the scores are in order
        vector<double> score_exp(4,0), score_act(4);
        score_exp[0] = 3; score_exp[1] = 3;
        score_act[0] = (*stack01)[0]->GetHypothesis(0)->GetScore();
        score_act[1] = (*stack01)[0]->GetHypothesis(1)->GetScore();
        score_act[2] = (*stack01)[0]->GetHypothesis(2)->GetScore();
        score_act[3] = (*stack01)[0]->GetHypothesis(3)->GetScore();
        ret = CheckVector(score_exp, score_act);
        // Check to make sure that pruning works

        stack01->Clear();
        graph.ProcessOneSpan(model, set, datas, 0, 1, 3, false);
        if(stack01->size() != 1) {
            cerr << "stack01pruned->size() != 1: " << stack01->size() << endl; ret = 0;
        } else if((*stack01)[0]->GetHypotheses().size() != 3) {
            cerr << "(*stack01pruned)[0].size() != 3: " << (*stack01)[0]->GetHypotheses().size() << endl; ret = 0;
        }
        // delete stack00; delete stack01;
        // delete stack11; delete stack01pruned;
        return ret;
    }

    int TestBuildHyperGraph() {
        HyperGraph graph;
        ReordererModel model;
        model.SetMaxTerm(0);
        model.SetUseReverse(false);
        graph.BuildHyperGraph(model, set, datas);
        const std::vector<SpanStack*> & stacks = graph.GetStacks();
        int ret = 1;
        // The total number of stacks should be 7: 0-0 0-1 1-1 0-2 1-2 2-2 root
        if(stacks.size() != 7) {
            cerr << "stacks.size() != 7: " << stacks.size() << endl; ret = 0;
        // The number of target spans should be 6: 0-1 1-0 0-2 2-0 1-2 2-1
        } else if (stacks[3]->size() != 6) {
            cerr << "Root node stacks[3]->size() != 6: " <<stacks[3]->size()<< endl;
            BOOST_FOREACH(const TargetSpan *span, stacks[3]->GetSpans())
                cerr << " " << span->GetTrgLeft() << "-" <<span->GetTrgRight() << endl;
            ret = 0;
        } else if (stacks[6]->GetSpans().size() != stacks[3]->size()) {
            cerr << "Root hypotheses " << stacks[6]->GetSpans().size()
                 << " and root spans " << stacks[3]->size() << " don't match." <<
                 endl; ret = 0;
        }
        return ret;
    }

    int TestBuildHyperGraphNoSave() {
        HyperGraph graph;
        ReordererModel model;
        model.SetMaxTerm(0);
        model.SetUseReverse(false);
        graph.BuildHyperGraph(model, set, datas, false);
        const std::vector<SpanStack*> & stacks = graph.GetStacks();
        int ret = 1;
        // The total number of stacks should be 7: 0-0 0-1 1-1 0-2 1-2 2-2 root
        if(stacks.size() != 7) {
            cerr << "stacks.size() != 7: " << stacks.size() << endl; ret = 0;
        // The number of target spans should be 1: -1--1
        } else if (stacks[3]->size() != 1) {
            cerr << "Root node stacks[3]->size() != 1: " <<stacks[3]->size()<< endl;
            BOOST_FOREACH(const TargetSpan *span, stacks[3]->GetSpans())
                cerr << " " << span->GetTrgLeft() << "-" <<span->GetTrgRight() << endl;
            ret = 0;
        } else if (stacks[6]->GetSpans().size() != stacks[3]->size()) {
            cerr << "Root hypotheses " << stacks[6]->GetSpans().size()
                 << " and root spans " << stacks[3]->size() << " don't match." <<
                 endl; ret = 0;
        }
        return ret;
    }

    int TestBuildHyperGraphCubeGrowing() {
        HyperGraph graph(true);
        ReordererModel model;
        model.SetMaxTerm(0);
        model.SetUseReverse(false);
        graph.BuildHyperGraph(model, set, datas);
        const std::vector<SpanStack*> & stacks = graph.GetStacks();
        int ret = 1;
        // The total number of stacks should be 7: 0-0 0-1 1-1 0-2 1-2 2-2 root
        if(stacks.size() != 7) {
            cerr << "stacks.size() != 7: " << stacks.size() << endl; ret = 0;
		// The number of target spans should be 3*2: 0-1 1-0 0-2 2-0 1-2 2-1
        } else if (stacks[3]->size() != 6) {
            cerr << "Root node stacks[3]->size() != 6: " <<stacks[3]->size()<< endl;
            BOOST_FOREACH(const TargetSpan *span, stacks[3]->GetSpans()){
            	cerr << " " << span->GetTrgLeft() << "-" <<span->GetTrgRight() << endl;
            }
            ret = 0;
        } else if (stacks[6]->size() != stacks[3]->size()) {
            cerr << "Root hypotheses " << stacks[6]->size()
                 << " and root spans " << stacks[3]->size() << " don't match." <<
                 endl; ret = 0;
        }
        return ret;
    }
    
    int TestAccumulateLoss() {
        // The value of the loss should be 1+2+5+6 = 14 (3 and 4 are not best)
        double val = my_hg.AccumulateLoss(tsr);
        int ret = 1;
        if(val != 14) {
            cerr << "my_hg.AccumulateLoss() != 14: " <<
                     my_hg.AccumulateLoss(tsr) << endl; ret = 0;
        }
        // Test the rescoring
        return ret;
    }

    int TestAccumulateFeatures() {
        // The value of the loss should be 1:1, 2:1, 5:1, 10:3
        FeatureVectorInt act;
        std::tr1::unordered_map<int, double> feat_map;
        my_hg.AccumulateFeatures(feat_map, model, set, datas, tsr);
        ClearAndSet(act, feat_map);
        FeatureVectorInt exp;
        exp.push_back(MakePair(1,1));
        exp.push_back(MakePair(2,1));
        exp.push_back(MakePair(5,1));
        exp.push_back(MakePair(10,3));
        // Test the rescoring
        return CheckVector(exp, act);
    }

    // Test that rescoring works
    int TestRescore() {
        // Create a model that assigns a weight of -1 to each production
        ReordererModel mod;
        for(int i = 0; i < 20; i++) {
            ostringstream oss;
            oss << "WEIGHT" << i;
            mod.SetWeight(oss.str(), 0);
        }
        mod.SetWeight("WEIGHT10", -1);
        int ret = 1;
        // Simply rescoring with this model should pick the forward production
        // with a score of -1
        double score = my_hg.Rescore(mod, 0.0);
        if(score != -1) {
            cerr << "Rescore(mod, 0.0) != -1: " << score << endl; ret = 0;
        }
        // Rescoring with loss +1 should pick the inverted terminal
        // with a loss of 14, minus a weight of 3 -> 11
        score = my_hg.Rescore(mod, 1.0);
        if(score != 11) {
            cerr << "Rescore(mod, 1.0) != 11: " << score << endl; ret = 0;
        }
        return ret;
    }

    // Test various types of reordering in the hypergraph
    int TestReorderingAndPrint() {
        // Create the expected reordering vectors
        vector<int> vec01(2,0); vec01[0] = 0; vec01[1] = 1;
        vector<int> vec10(2,0); vec10[0] = 1; vec10[1] = 0;
        vector<string> str01(2); str01[0] = "0"; str01[1] = "1";
        // Create a forest that can handle various things
        TargetSpan *span00 = new TargetSpan(0,0,-1,-1),
                   *span01 = new TargetSpan(0,1,-1,-1),
                   *span11 = new TargetSpan(1,1,-1,-1),
                   *spanr = new TargetSpan(0,1,-1,-1);
        span00->AddHypothesis(new Hypothesis(1,1, 0,0,-1,-1,HyperEdge::EDGE_FOR));
        span11->AddHypothesis(new Hypothesis(1,1, 1,1,-1,-1,HyperEdge::EDGE_FOR));
        span01->AddHypothesis(new Hypothesis(1,1, 0,1,-1,-1,HyperEdge::EDGE_FOR));
        spanr->AddHypothesis(new Hypothesis(1,1, 0,1,-1,-1,HyperEdge::EDGE_ROOT,-1,0,-1,span01));
        // Get the reordering for forward
        int ret = 1;
        vector<int> for_reorder; spanr->GetReordering(for_reorder);
        ostringstream for_oss; spanr->PrintParse(str01, for_oss);
        ret = min(ret, CheckVector(vec01, for_reorder));
        ret = min(ret, CheckString("(F (FW 0) (FW 1))", for_oss.str()));
        // Get the reordering bac backward
        span01->GetHypothesis(0)->SetType(HyperEdge::EDGE_BAC);
        vector<int> bac_reorder; spanr->GetReordering(bac_reorder);
        ostringstream bac_oss; spanr->PrintParse(str01, bac_oss);
        ret = min(ret, CheckVector(vec10, bac_reorder));
        ret = min(ret, CheckString("(B (BW 0) (BW 1))", bac_oss.str()));
        // Get the reordering for forward
        span01->GetHypothesis(0)->SetType(HyperEdge::EDGE_STR);
        span01->GetHypothesis(0)->SetLeftChild(span00);
        span01->GetHypothesis(0)->SetRightChild(span11);
        vector<int> str_reorder; spanr->GetReordering(str_reorder);
        ostringstream str_oss; spanr->PrintParse(str01, str_oss);
        ret = min(ret, CheckVector(vec01, str_reorder));
        ret = min(ret,CheckString("(S (F (FW 0)) (F (FW 1)))",str_oss.str()));
        // Get the reordering for forward
        span01->GetHypothesis(0)->SetType(HyperEdge::EDGE_INV);
        vector<int> inv_reorder; spanr->GetReordering(inv_reorder);
        ostringstream inv_oss; spanr->PrintParse(str01, inv_oss);
        ret = min(ret, CheckVector(vec10, inv_reorder)); 
        ret = min(ret,CheckString("(I (F (FW 0)) (F (FW 1)))",inv_oss.str()));
        return ret;
    }

    // Test various types of reordering in the hypergraph
    int TestPrintHyperGraph() {
        // Create the expected reordering vectors
        vector<string> str01(2); str01[0] = "0"; str01[1] = "1";
        // Create a forest that can handle various things
        TargetSpan *span00 = new TargetSpan(0,0,-1,-1),
                   *span01 = new TargetSpan(0,1,-1,-1),
                   *span11 = new TargetSpan(1,1,-1,-1),
                   *spanr = new TargetSpan(0,1,-1,-1);
        span00->AddHypothesis(new Hypothesis(1,1, 0,0,-1,-1,HyperEdge::EDGE_FOR));
        span11->AddHypothesis(new Hypothesis(1,1, 1,1,-1,-1,HyperEdge::EDGE_BAC));
        span01->AddHypothesis(new Hypothesis(1,1, 0,1,-1,-1,HyperEdge::EDGE_INV,-1,1,-1,span00,span11));
        spanr->AddHypothesis(new Hypothesis(1,1, 0,1,-1,-1,HyperEdge::EDGE_ROOT,-1,0,-1,span01));
        // Get the reordering for forward
        int ret = 1;
        span01->GetHypothesis(0)->SetType(HyperEdge::EDGE_FOR);
        ret = min(ret, CheckString("[R] ||| [F]",
            spanr->GetHypothesis(0)->GetRuleString(str01, 'F')));
        ret = min(ret, CheckString("[F] ||| 0 1",
            span01->GetHypothesis(0)->GetRuleString(str01)));
        // Get the reordering bac backward
        span01->GetHypothesis(0)->SetType(HyperEdge::EDGE_BAC);
        ret = min(ret, CheckString("[R] ||| [B]",
            spanr->GetHypothesis(0)->GetRuleString(str01, 'B')));
        ret = min(ret, CheckString("[B] ||| 0 1",
            span01->GetHypothesis(0)->GetRuleString(str01)));
        // Get the reordering for forward
        span01->GetHypothesis(0)->SetType(HyperEdge::EDGE_STR);
        span01->GetHypothesis(0)->SetLeftChild(span00);
        span01->GetHypothesis(0)->SetRightChild(span11);
        ret = min(ret, CheckString("[R] ||| [S]",
            spanr->GetHypothesis(0)->GetRuleString(str01, 'S')));
        ret = min(ret, CheckString("[S] ||| [F] [B]",
            span01->GetHypothesis(0)->GetRuleString(str01, 'F', 'B')));
        ret = min(ret, CheckString("[F] ||| 0",
            span00->GetHypothesis(0)->GetRuleString(str01)));
        ret = min(ret, CheckString("[B] ||| 1",
            span11->GetHypothesis(0)->GetRuleString(str01)));
        // Get the reordering for forward
        span01->GetHypothesis(0)->SetType(HyperEdge::EDGE_INV);
        ret = min(ret, CheckString("[R] ||| [I]",
            spanr->GetHypothesis(0)->GetRuleString(str01, 'I')));
        ret = min(ret, CheckString("[I] ||| [F] [B]",
            span01->GetHypothesis(0)->GetRuleString(str01, 'F', 'B')));
        // Create a hypergraph
        HyperGraph hg;
        SpanStack * stack00 = new SpanStack(0,0); stack00->push_back(span00); hg.SetStack(0, 0, stack00);
        SpanStack * stack11 = new SpanStack(1,1); stack11->push_back(span11); hg.SetStack(1, 1, stack11);
        SpanStack * stack01 = new SpanStack(0,1); stack01->push_back(span01); hg.SetStack(0, 1, stack01);
        SpanStack * stackr  = new SpanStack(0,2); stackr->push_back(spanr); hg.SetStack(0, 2, stackr);
        // Check that the trimmed hypergraph only contains rules that should be there
        span01->GetHypothesis(0)->SetType(HyperEdge::EDGE_FOR);
        span01->GetHypothesis(0)->SetLeftChild(NULL);
        span01->GetHypothesis(0)->SetRightChild(NULL);
        ostringstream graph_stream1;
        hg.PrintHyperGraph(str01, graph_stream1);
        ret = min(ret, CheckString("{\"rules\": [\"[F] ||| 0 1\", \"[R] ||| [F]\"], \"nodes\": [[{\"feature\":{\"parser\":1},\"rule\":1}], [{\"tail\":[0],\"feature\":{\"parser\":1},\"rule\":2}]], \"goal\": 1}",graph_stream1.str()));
        // Make another hypothesis
        span01->GetHypothesis(0)->SetType(HyperEdge::EDGE_INV);
        span01->GetHypothesis(0)->SetLeftChild(span00);
        span01->GetHypothesis(0)->SetRightChild(span11);
        span01->AddHypothesis(new Hypothesis(1,1, 0,1,-1,-1,HyperEdge::EDGE_STR,-1,1,-1,span00,span11));
        // Print again
        ostringstream graph_stream;
        hg.PrintHyperGraph(str01, graph_stream);
        ret = min(ret, CheckString("{\"rules\": [\"[F] ||| 0\", \"[B] ||| 1\", \"[I] ||| [F] [B]\", \"[S] ||| [F] [B]\", \"[R] ||| [I]\", \"[R] ||| [S]\"], \"nodes\": [[{\"feature\":{\"parser\":1},\"rule\":1}], [{\"feature\":{\"parser\":1},\"rule\":2}], [{\"tail\":[0,1],\"feature\":{\"parser\":1},\"rule\":3}], [{\"tail\":[0,1],\"feature\":{\"parser\":1},\"rule\":4}], [{\"tail\":[2],\"feature\":{\"parser\":1},\"rule\":5}, {\"tail\":[3],\"feature\":{\"parser\":1},\"rule\":6}]], \"goal\": 4}",graph_stream.str()));
        return ret;
    }

    bool RunTest() {
        int done = 0, succeeded = 0;
        done++; cout << "TestGetTrgSpanID()" << endl; if(TestGetTrgSpanID()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestGetEdgeFeaturesAndWeights()" << endl; if(TestGetEdgeFeaturesAndWeights()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestProcessOneSpan()" << endl; if(TestProcessOneSpan()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestProcessOneSpanNoSave()" << endl; if(TestProcessOneSpanNoSave()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestBuildHyperGraph()" << endl; if(TestBuildHyperGraph()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestBuildHyperGraphCubeGrowing()" << endl; if(TestBuildHyperGraphCubeGrowing()) succeeded++; else cout << "FAILED!!!" << endl;        
        done++; cout << "TestBuildHyperGraphNoSave()" << endl; if(TestBuildHyperGraphNoSave()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAccumulateLoss()" << endl; if(TestAccumulateLoss()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestAccumulateFeatures()" << endl; if(TestAccumulateFeatures()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestRescore()" << endl; if(TestRescore()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestReorderingAndPrint()" << endl; if(TestReorderingAndPrint()) succeeded++; else cout << "FAILED!!!" << endl;
        done++; cout << "TestPrintHyperGraph()" << endl; if(TestPrintHyperGraph()) succeeded++; else cout << "FAILED!!!" << endl;
        cout << "#### TestHyperGraph Finished with "<<succeeded<<"/"<<done<<" tests succeeding ####"<<endl;
        return done == succeeded;
    }

private:
    HyperEdge edge00, edge11, edge22, edge12t, edge12nt, edge02;
    CombinedAlign cal;
    Ranks ranks;
    FeatureDataSequence sent, sent_pos;
    ReordererModel model;
    std::vector<double> weights;
    FeatureSet set;
    Sentence datas;
    FeatureSequence *featw, *featp;
    TargetSpan *ts00, *ts01f, *ts01b, *ts11, *tsr;
    HyperGraph my_hg;


};

}

#endif
