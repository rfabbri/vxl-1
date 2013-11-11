//This is lemsvxl/brcv/shp/dbskr/tests/test_hypergraph.cxx
//  Ming-Ching Chang  Mar 11, 2005
//
//
//
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbsk3dr\tests\data
//    or 
//  dbsk3dr_test_shock_match D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbsk3dr\tests\data
//

#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vcl_fstream.h>

#include <dbasn/dbasn_gradasgn.h>
#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3dr/dbsk3dr_match.h>

#if 0

MAIN_ARGS(dbsk3dr_test_shock_match)
{
  vcl_string dir_base;
  if ( argc >= 2 ) {
      dir_base = argv[1];
      #ifdef VCL_WIN32
        dir_base += "\\";
      #else
        dir_base += "/";
      #endif
  }

  testlib_test_start("testing shock graph matching.");

  dbsk3dr_ms_gradasn_graph* SG0 = new dbsk3dr_ms_gradasn_graph;
  dbsk3dr_ms_gradasn_graph* SG1 = new dbsk3dr_ms_gradasn_graph;
  
  vcl_string file1 = dir_base + "davidhead_20k-ht-gt-sm5.sg";
  SG0->setup_from_sg (file1.c_str());
  TEST("Load first graph ", SG0->n_nodes(), 71);

  //The modified version of bones1.sm5.sg by
  //removing 12 curves and 12 vertices of 6 A1A3_I tabs.
  vcl_string file2 = dir_base + "davidhead_10k-ht-gt-sm5.sg";
  SG1->setup_from_sg (file2.c_str());
  TEST("Load second graph ", SG1->n_nodes(), 71);

  //Prepare shock matching
  dbsk3dr_match SM;
  dbasn_params params_shock;
  vcl_cout<< params_shock;
  ///SM.setup_GA_params (SG0, SG1, params_shock);
  SM.set_G (SG0);
  SM.set_g (SG1);
  SM.set_params (params_shock);
  SM._reset_mem ();

  vcl_cout <<"\n=================\n";
  vcl_cout <<"Graph SG0 with nodes: "<< SG0->n_nodes() << ", links: "<< -1 << vcl_endl;
  vcl_cout <<"Graph SG1 with nodes: "<< SG1->n_nodes() << ", links: "<< -1 << vcl_endl;

  //Normalize all nodes, links, corner costs to be within [0 ~ 1].
  SM.normalize_costs (false);

  SM.compute_matching ();

  //: test if matching is correct.
  int ground_truth_result_g[71] = 
  { 47, 7, 49, 8, 4, 12, 1, 16, 44, 2, 
    43, 3, 14, 13, 5, 9, 65, 32, 23, 20,
    33, 29, 37, 17, 0, 31, 24, 40, 44, 67, 
    38, 39, 10, 22, 45, 48, 46, 30, 18, 42,
    21, 35, 34, 11, 15, 28, -1, -1, 27, 6,
    26, 36, 54, 68, 63, 53, 51, 56, 55, 41,
    19, 37, 60, 66, 64, 58, 50, 70, 61, 62, 59
  };

  for (int a=0; a<SM.M_row()-1; a++) {
    int i = SM.labelGg(a);
    
    TEST("Compare each matching node", i, ground_truth_result_g[a]);

    int SGid = SM.ms_G()->sid(a);
    if (i != -1) { //slack!
      int Sgid = SM.ms_g()->sid (i);

      /*const dbsk3d_ms_node* v1 = (dbsk3d_ms_node*) SM.ms_G()->vertexmap (SGid);
      const dbsk3d_ms_node* v2 = (dbsk3d_ms_node*) SM.ms_g()->vertexmap (Sgid);
      double radius1 = v1->time();
      double radius2 = v2->time();
      TEST("Compare each matching node radius ", radius1, radius2);*/

      vcl_fprintf (stderr, "%d(%d) <-> %d(%d)\n", SGid, a, Sgid, i);
    }
  }
  
  return testlib_test_summary();
}

#endif


#if 0
//: 040411 Ming: old test of Ozge's new graduated assignment code

#include "algo/gradassign_new/ga_simpleg.h"
#include "algo/gradassign_new/ga_simpleg_link.h"
#include "algo/gradassign_new/gradassign_new_params.h"
#include "algo/gradassign_new/gradassign_new.h"

void GradAssign_Ozge_Test ()
{
  int Gn, gn;
  ga_simpleg G, g;
  G.read_graph2("..\\algo\\gradassign_new\\examples\\GraphFile1_bicr1p500x_NoSearchRadius.txt", &Gn);
  g.read_graph2("..\\algo\\gradassign_new\\examples\\GraphFile2_bicr2p500x_NoSearchRadius.txt", &gn);
  
  double **M;
  M = new double*[Gn+1]; //  Gn+gn
  for (int i=0; i<Gn+1; i++)
    M[i] = new double[gn+1]; //  gn+Gn
  
  gradassign_params params_(
    100,    //T, initial temperature (should be high)
    0.1,    //Tf, termination temperature
    0.9,    //Tr, step in temperature
    4,      // i0
    30,   // i1
    10,   //alpha, weight on the node cost
    0.1,    //thres, way to assign negative cost, not used in my case.
    false,  // curve_version          
    true,   // link_compare
    false,  // spatial_link_version               
    true,   // node_comparison
    false); // normalized energy
  gradassign ga(params_);

  ga.get_assignment_Rangarajan_version(G, g, M);
  
  ga.print_M2(M, Gn+1, gn+1);
  
  ga.cleanup(M, M, Gn+1, gn+1);
  
  ga.print_M2(M, Gn+1, gn+1);
}

/////////////////////////////////////////////////////////////////////////


void MakeTestSC1 (dbsk3dr_ms_gradasn_graph* GAScaffold)
{
  dbskr_ms_gradasn_vertex* SNode1 = new dbskr_ms_gradasn_vertex ();
  SNode1->id_ = 0;
  SNode1->nodeType_ = ScaffoldNode::NODE_TYPE_A1A3;
  SNode1->ShockRedNode_ = new ShockRedNode ();
  SNode1->ShockRedNode_->fDistToGene_ = 1;
  SNode1->ShockRedNode_->fPosX_ = 1;
  SNode1->ShockRedNode_->fPosY_ = 0;
  SNode1->ShockRedNode_->fPosZ_ = 0;
  GAScaffold->addScaffoldVertex (SNode1);

  dbskr_ms_gradasn_vertex* SNode2 = new dbskr_ms_gradasn_vertex ();
  SNode2->id_ = 1;
  SNode2->nodeType_ = ScaffoldNode::NODE_TYPE_A14;
  SNode2->ShockRedNode_ = new ShockRedNode ();
  SNode2->ShockRedNode_->fDistToGene_ = 2;
  SNode2->ShockRedNode_->fPosX_ = 0;
  SNode2->ShockRedNode_->fPosY_ = 1;
  SNode2->ShockRedNode_->fPosZ_ = 0;
  GAScaffold->addScaffoldVertex (SNode2);

  ScaffoldLink* SLink1 = new ScaffoldLink ();
  SLink1->linkType_ = ScaffoldLink::LINK_TYPE_A13;
  SLink1->startSNode_ = SNode1;
  SLink1->endSNode_ = SNode2;
  ShockRedNode* red1 = new ShockRedNode ();
  red1->fDistToGene_ = 1;
  red1->fPosX_ = 1;
  red1->fPosY_ = 0;
  red1->fPosZ_ = 0;
  SLink1->ShockRedNodes_.push_back (red1);
  ShockRedNode* red2 = new ShockRedNode ();
  red2->fDistToGene_ = 2;
  red2->fPosX_ = 0;
  red2->fPosY_ = 1;
  red2->fPosZ_ = 0;
  SLink1->ShockRedNodes_.push_back (red2);
  GAScaffold->addScaffoldCurve (SLink1);

  SNode1->connectedSLinks_.push_back (SLink1);
  SNode2->connectedSLinks_.push_back (SLink1);

  GAScaffold->setup_gradasgn_hypg ();

  GAScaffold->print_node_cost ();
  GAScaffold->print_link_cost ();

}

void MakeTestSC2 (dbsk3dr_ms_gradasn_graph* GAScaffold)
{

  dbskr_ms_gradasn_vertex* SNode1 = new dbskr_ms_gradasn_vertex ();
  SNode1->id_ = 0;
  SNode1->nodeType_ = ScaffoldNode::NODE_TYPE_A1A3;
  SNode1->ShockRedNode_ = new ShockRedNode ();
  SNode1->ShockRedNode_->fDistToGene_ = 1;
  SNode1->ShockRedNode_->fPosX_ = 1;
  SNode1->ShockRedNode_->fPosY_ = 0;
  SNode1->ShockRedNode_->fPosZ_ = 0;
  GAScaffold->addScaffoldVertex (SNode1);

  dbskr_ms_gradasn_vertex* SNode2 = new dbskr_ms_gradasn_vertex ();
  SNode2->id_ = 1;
  SNode2->nodeType_ = ScaffoldNode::NODE_TYPE_A14;
  SNode2->ShockRedNode_ = new ShockRedNode ();
  SNode2->ShockRedNode_->fDistToGene_ = 2;
  SNode2->ShockRedNode_->fPosX_ = 0;
  SNode2->ShockRedNode_->fPosY_ = 1;
  SNode2->ShockRedNode_->fPosZ_ = 0;
  GAScaffold->addScaffoldVertex (SNode2);

  ScaffoldLink* SLink1 = new ScaffoldLink ();
  SLink1->linkType_ = ScaffoldLink::LINK_TYPE_A13;
  SLink1->startSNode_ = SNode1;
  SLink1->endSNode_ = SNode2;
  ShockRedNode* red1 = new ShockRedNode ();
  red1->fDistToGene_ = 1;
  red1->fPosX_ = 1;
  red1->fPosY_ = 0;
  red1->fPosZ_ = 0;
  SLink1->ShockRedNodes_.push_back (red1);
  ShockRedNode* red2 = new ShockRedNode ();
  red2->fDistToGene_ = 2;
  red2->fPosX_ = 0;
  red2->fPosY_ = 1;
  red2->fPosZ_ = 0;
  SLink1->ShockRedNodes_.push_back (red2);
  GAScaffold->addScaffoldCurve (SLink1);

  SNode1->connectedSLinks_.push_back (SLink1);
  SNode2->connectedSLinks_.push_back (SLink1);

  GAScaffold->setup_gradasgn_hypg ();

  GAScaffold->print_node_cost ();
  GAScaffold->print_link_cost ();
}
#endif



