//This is lemsvxl/brcv/shp/dbskr/tests/test_simple_graph_match.cxx
//  Ming-Ching Chang  Aug 29, 2007
//
//
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbsk3dr\tests\data
//    or 
//  dbsk3dr_simple_graph_match D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbsk3dr\tests\data
//

#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_ctime.h>
#include <vnl/vnl_random.h>
#include <vul/vul_printf.h>

#include <dbasn/dbasn_gradasgn.h>
#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3dr/dbsk3dr_match.h>

#if 0

vnl_random get_rand_s;

//: Repeat the test for N times.
#define REPEAT_N 100

#define T1_VERTEX_RADIUS        5
#define T1_VERTEX_RADIUS_VAR    3

#define T2_VERTEX_RADIUS_SMALL  3
#define T2_VERTEX_RADIUS_LARGE  10
#define T2_VERTEX_RADIUS_VAR    1
#define T2_EDGE_LEN             5
#define T2_EDGE_LEN_VAR         1

#define T3_VERTEX_RADIUS        5
#define T3_VERTEX_RADIUS_VAR    1 // 0.5: ok, 1: some error
#define T3_EDGE_LEN_LARGE       10
#define T3_EDGE_LEN_SMALL       3
#define T3_EDGE_LEN_VAR         0.5

bool test_graphs_1_node (dbsk3dr_match& SM, dbasn_params& GAparams);
bool test_graphs_2_nodes (dbsk3dr_match& SM, dbasn_params& GAparams);
bool test_graphs_3_nodes (dbsk3dr_match& SM, dbasn_params& GAparams);

MAIN_ARGS (dbsk3dr_simple_graph_match)
{
  dbsk3dr_match SM;  
  dbasn_params GAparams;
  get_rand_s.reseed ((unsigned int) vcl_time(NULL));

  //Repeat the test for N times.
  int i;
  int n_test_1_success = 0;
  int n_test_2_success = 0;
  int n_test_3_success = 0;
  bool r;
  for (i=0; i< REPEAT_N; i++) {
    r = test_graphs_1_node (SM, GAparams);
    if (r)
      n_test_1_success++;
  }
  for (i=0; i< REPEAT_N; i++) {
    r = test_graphs_2_nodes (SM, GAparams);  
    if (r)
      n_test_2_success++;
  }
  for (i=0; i< REPEAT_N; i++) {
    r = test_graphs_3_nodes (SM, GAparams);
    if (r)
      n_test_3_success++;
  }
  vul_printf (vcl_cout, "\n===========================================\n");
  vul_printf (vcl_cout, "Test 1 success: %d (out of total %d).\n", n_test_1_success, REPEAT_N);
  vul_printf (vcl_cout, "Test 2 success: %d (out of total %d).\n", n_test_2_success, REPEAT_N);
  vul_printf (vcl_cout, "Test 3 success: %d (out of total %d).\n", n_test_3_success, REPEAT_N);
  
  return testlib_test_summary();
}

//: Trivial test of graphs with only 1 node.
bool test_graphs_1_node (dbsk3dr_match& SM, dbasn_params& GAparams)
{
  //Setup the two graphs to match.
  dbsk3dr_ms_gradasn_graph* SG0 = new dbsk3dr_ms_gradasn_graph;
  dbsk3dr_ms_gradasn_graph* SG1 = new dbsk3dr_ms_gradasn_graph;

  dbskr_ms_gradasn_vertex* V0 = (dbskr_ms_gradasn_vertex*) SG0->_new_vertex ();
  SG0->_add_vertex (V0);
  float r = T1_VERTEX_RADIUS + (float) get_rand_s.drand32 (0, T1_VERTEX_RADIUS_VAR);
  V0->set_radius (r);

  dbskr_ms_gradasn_vertex* V1 = (dbskr_ms_gradasn_vertex*) SG1->_new_vertex ();
  SG1->_add_vertex (V1);
  r = T1_VERTEX_RADIUS + (float) get_rand_s.drand32 (0, T1_VERTEX_RADIUS_VAR);
  V1->set_radius (r);

  vul_printf (vcl_cout, "Graph G: %u nodes, %u links.\n", SG0->vertexmap().size(), SG0->edgemap().size());
  vul_printf (vcl_cout, "Graph g: %u nodes, %u links.\n", SG1->vertexmap().size(), SG1->edgemap().size());

  SG0->setup_gradasgn_hypg ();
  SG1->setup_gradasgn_hypg ();

  //Setup and run the matching.
  ///SM.setup_GA_params (SG0, SG1, GAparams);
  SM.set_G (SG0);
  SM.set_g (SG1);
  SM.set_params (GAparams);
  SM._reset_mem ();

  //Normalize all nodes, links, corner costs to be within [0 ~ 1].
  SM.normalize_costs (false);
  
  //Run the shock grad assign matching.
  SM.get_assignment ();
  SM.print_match_results ();

  //Test the matching result.
  TEST ("Matching graphs of single node ", SM.labelGg(0) == 0, true);
  bool result = SM.labelGg(0) == 0;

  delete SG0, SG1;
  SM._free_mem ();
  return result;
}

//: Trivial test of graphs with only 2 nodes.
//  one with low cost, the other with high cost.
bool test_graphs_2_nodes (dbsk3dr_match& SM, dbasn_params& GAparams)
{
  //Setup the two graphs to match.
  dbsk3dr_ms_gradasn_graph* SG0 = new dbsk3dr_ms_gradasn_graph;
  dbsk3dr_ms_gradasn_graph* SG1 = new dbsk3dr_ms_gradasn_graph;

  dbskr_ms_gradasn_vertex* V00 = (dbskr_ms_gradasn_vertex*) SG0->_new_vertex ();
  SG0->_add_vertex (V00);
  float r = T2_VERTEX_RADIUS_SMALL + (float) get_rand_s.drand32 (0, T2_VERTEX_RADIUS_VAR);
  V00->set_radius (r);
  dbskr_ms_gradasn_vertex* V01 = (dbskr_ms_gradasn_vertex*) SG0->_new_vertex ();
  SG0->_add_vertex (V01);
  r = T2_VERTEX_RADIUS_LARGE + (float) get_rand_s.drand32 (0, T2_VERTEX_RADIUS_VAR);
  V01->set_radius (r);
  dbsk3d_ms_curve* SC0 = (dbsk3d_ms_curve*) SG0->add_new_edge (V00, V01);
  float l = T2_EDGE_LEN + get_rand_s.drand32 (0, T2_EDGE_LEN_VAR);
  SC0->set_length (l);
  SC0->set_type (C_TYPE_AXIAL);

  dbskr_ms_gradasn_vertex* V10 = (dbskr_ms_gradasn_vertex*) SG1->_new_vertex ();
  SG1->_add_vertex (V10);
  r = T2_VERTEX_RADIUS_LARGE + (float) get_rand_s.drand32 (0, T2_VERTEX_RADIUS_VAR);
  V10->set_radius (r);
  dbskr_ms_gradasn_vertex* V11 = (dbskr_ms_gradasn_vertex*) SG1->_new_vertex ();
  SG1->_add_vertex (V11);
  r = T2_VERTEX_RADIUS_SMALL + (float) get_rand_s.drand32 (0, T2_VERTEX_RADIUS_VAR);
  V11->set_radius (r);
  dbsk3d_ms_curve* SC1 = (dbsk3d_ms_curve*) SG1->add_new_edge (V10, V11);
  l = T2_EDGE_LEN + get_rand_s.drand32 (0, T2_EDGE_LEN_VAR);
  SC1->set_length (l);
  SC1->set_type (C_TYPE_AXIAL);

  vul_printf (vcl_cout, "Graph G: %u nodes, %u links.\n", SG0->vertexmap().size(), SG0->edgemap().size());
  vul_printf (vcl_cout, "Graph g: %u nodes, %u links.\n", SG1->vertexmap().size(), SG1->edgemap().size());

  SG0->setup_gradasgn_hypg ();
  SG1->setup_gradasgn_hypg ();

  //Setup and run the matching.
  ///SM.setup_GA_params (SG0, SG1, GAparams);
  SM.set_G (SG0);
  SM.set_g (SG1);
  SM.set_params (GAparams);
  SM._reset_mem ();

  //Normalize all nodes, links, corner costs to be within [0 ~ 1].
  SM.normalize_costs (false);

  //Run the shock grad assign matching.
  SM.get_assignment ();
  SM.print_match_results ();

  //Test the matching result.
  int incorrect = 0;
  if (SM.labelGg(0) != 1)
    incorrect++;
  if (SM.labelGg(1) != 0)
    incorrect++;
  TEST ("Matching graphs of two nodes (incorrect should be 0) ", incorrect, 0);

  delete SG0, SG1;
  SM._free_mem ();
  return incorrect == 0;
}

//: Trivial test of graphs with only 3 nodes with roughly equal cost.
//  Assign one large and one small cost to two links.
bool test_graphs_3_nodes (dbsk3dr_match& SM, dbasn_params& GAparams)
{
  //Setup the two graphs to match.
  dbsk3dr_ms_gradasn_graph* SG[2];

  vgl_point_3d<double> pos[6];
  pos[0] = vgl_point_3d<double> (0,-1,0);
  pos[1] = vgl_point_3d<double> (0,0,0);
  pos[2] = vgl_point_3d<double> (-1,0,0);
  pos[3] = vgl_point_3d<double> (1,0,0);
  pos[4] = vgl_point_3d<double> (0,0,0);
  pos[5] = vgl_point_3d<double> (0,1,0);
  
  int id = 0;
  for (int i=0; i<2; i++) {
    SG[i] = new dbsk3dr_ms_gradasn_graph;
    for (int j=0; j<3; j++) {
      dbskr_ms_gradasn_vertex* V = (dbskr_ms_gradasn_vertex*) SG[i]->_new_vertex ();
      SG[i]->_add_vertex (V);
      float r = float (T3_VERTEX_RADIUS + get_rand_s.drand32 (0, T3_VERTEX_RADIUS_VAR));
      V->set_radius (r);
      dbsk3d_fs_vertex* FV = new dbsk3d_fs_vertex (id);
      FV->set_pt (pos[id]);
      V->set_V (FV);
      id++;
    }
  }

  //SG[0]: L01 large, L12 small.
  dbskr_ms_gradasn_vertex* V0 = (dbskr_ms_gradasn_vertex*) SG[0]->vertexmap(0);
  dbskr_ms_gradasn_vertex* V1 = (dbskr_ms_gradasn_vertex*) SG[0]->vertexmap(1);
  dbsk3d_ms_curve* SC01 = (dbsk3d_ms_curve*) SG[0]->add_new_edge (V0, V1);
  float l = T3_EDGE_LEN_LARGE + get_rand_s.drand32 (0, T3_EDGE_LEN_VAR);
  SC01->set_length (l);
  SC01->set_type (C_TYPE_AXIAL);
  dbskr_ms_gradasn_vertex* V2 = (dbskr_ms_gradasn_vertex*) SG[0]->vertexmap(2);
  dbsk3d_ms_curve* SC12 = (dbsk3d_ms_curve*) SG[0]->add_new_edge (V1, V2);
  l = T3_EDGE_LEN_SMALL + get_rand_s.drand32 (0, T3_EDGE_LEN_VAR);
  SC12->set_length (l);
  SC12->set_type (C_TYPE_AXIAL);

  //SG[1]: L02 large, L01 small.
  V0 = (dbskr_ms_gradasn_vertex*) SG[1]->vertexmap(0);
  V2 = (dbskr_ms_gradasn_vertex*) SG[1]->vertexmap(2);
  dbsk3d_ms_curve* SC02 = (dbsk3d_ms_curve*) SG[1]->add_new_edge (V0, V2);
  l = T3_EDGE_LEN_LARGE + get_rand_s.drand32 (0, T3_EDGE_LEN_VAR);
  SC02->set_length (l);
  SC02->set_type (C_TYPE_AXIAL);
  V1 = (dbskr_ms_gradasn_vertex*) SG[1]->vertexmap(1);
  SC01 = (dbsk3d_ms_curve*) SG[1]->add_new_edge (V0, V1);
  l = T3_EDGE_LEN_SMALL + get_rand_s.drand32 (0, T3_EDGE_LEN_VAR);
  SC01->set_length (l);
  SC01->set_type (C_TYPE_AXIAL);

  vul_printf (vcl_cout, "Graph G: %u nodes, %u links.\n", SG[0]->vertexmap().size(), SG[0]->edgemap().size());
  vul_printf (vcl_cout, "Graph g: %u nodes, %u links.\n", SG[1]->vertexmap().size(), SG[1]->edgemap().size());

  SG[0]->setup_gradasgn_hypg ();
  SG[1]->setup_gradasgn_hypg ();

  //Setup and run the matching.
  SM.setup_GA_params (SG[0], SG[1], GAparams);
  SM.set_G (SG[0]);
  SM.set_g (SG[1]);
  SM.set_params (GAparams);
  SM._reset_mem ();

  //Normalize all nodes, links, corner costs to be within [0 ~ 1].
  SM.normalize_costs (false);

  //Run the shock grad assign matching.
  SM.get_assignment ();
  SM.print_match_results ();

  //Test the matching result.
  //SG[0]: 0-1-2.  pos: (0,-1), (0,0), (-1, 0). 
  //SG[1]: 2-0-1.  pos: (1,0), (0,0), (0,1).
  int incorrect = 0;
  if (SM.labelGg(0) != 2)
    incorrect++;
  if (SM.labelGg(1) != 0)
    incorrect++;
  if (SM.labelGg(2) != 1)
    incorrect++;
  TEST ("Matching graphs of two nodes (incorrect should be 0) ", incorrect, 0);

  delete SG[0], SG[1];
  SM._free_mem ();
  return incorrect == 0;
}

#endif



