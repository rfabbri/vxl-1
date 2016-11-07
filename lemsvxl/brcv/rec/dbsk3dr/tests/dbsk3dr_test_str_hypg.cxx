//---------------------------------------------------------------------
// This is brcv/rec/dbasn/tests/dbsk3dr_test_str_hypg.cxx
//:
// \file
// \brief This is the Graduated Assignment test program.
//
// \author
//  Ming-Ching Chang - January 7, 2008
//
// \verbatim
//  Modifications
//   Ming Ching Chang   -  Extend the grad. assignment tests on random graphs.
// \endverbatim
//
//  To debug in Visual studio, put argument
//  all 
//  dbsk3dr_test_str_hypg
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vnl/vnl_random.h>
#include <vul/vul_printf.h>
#include <vul/vul_sprintf.h>
#include <testlib/testlib_test.h>

#include <dbasn/dbasnh_gradasgn.h>
#include <dbasn/dbasnh_hypg_algos.h>

///#include "dbasn_test_artificial_def.h"

#define G_N_NODES   9
#define G_N_LINKS   18
#define G_N_CORNERS 36

int l_nodes[][2] = {{0, 1}, {1, 2}, {0, 2}, {0, 3}, {0, 4}, {1, 5}, {1, 6}, {2, 7}, {2, 8},
                    {3, 5}, {4, 6}, {5, 7}, {6, 8}, {3, 7}, {4, 8}, {3, 4}, {5, 6}, {7, 8}};

int c_nodes[][3] = {{1, 0, 2}, {3, 0, 1}, {3, 0, 2}, {4, 0, 1}, {4, 0, 2}, {3, 0, 4},
                    {0, 1, 2}, {5, 1, 0}, {5, 1, 2}, {6, 1, 0}, {6, 1, 2}, {5, 1, 6},
                    {0, 2, 1}, {7, 2, 0}, {7, 2, 1}, {8, 2, 0}, {8, 2, 1}, {7, 2, 8},
                    {7, 3, 0}, {5, 3, 0}, {4, 3, 0}, 
                    {3, 4, 0}, {6, 4, 0}, {8, 4, 0}, 
                    {3, 5, 1}, {7, 5, 1}, {6, 5, 1},
                    {4, 6, 1}, {5, 6, 1}, {8, 6, 1},
                    {3, 7, 2}, {5, 7, 2}, {8, 7, 2},
                    {7, 8, 2}, {6, 8, 2}, {4, 8, 2}};

//ground truth for the standard cases.
int str_labelgG[] = {0, 1, 2, 3, 4, 5, 6, 7, 8}; //labelgG [g's node id] = G's node id.

//: generate hypg with discriminative node info.
void generate_hypg_discri_n (dbasnh_hypg* G, dbasnh_hypg* g, const float noise)
{
  assert (G->nodes().size() == 0);
  assert (g->nodes().size() == 0);
  vnl_random mzr;

  for (int i = 0; i<G_N_NODES; i++) {
    G->_add_node (new dbasn_node (i, i+1));
    double r = mzr.drand32(0, noise);
    g->_add_node (new dbasn_node (i, i+1 + r));
  }
}

//: generate hypg with discriminative node info.
void generate_hypg_random_n (dbasnh_hypg* G, dbasnh_hypg* g, const float avg, const float noise)
{
  assert (G->nodes().size() == 0);
  assert (g->nodes().size() == 0);
  vnl_random mzr;

  for (int i = 0; i<G_N_NODES; i++) {
    double c = avg + mzr.drand32(0, noise);
    G->_add_node (new dbasn_node (i, c));
    c = avg + mzr.drand32(0, noise);
    g->_add_node (new dbasn_node (i, c));
  }
}

//: generate hypg with discriminative node info.
void generate_hypg_discri_l (dbasnh_hypg* G, dbasnh_hypg* g, const float noise)
{
  vnl_random mzr;
  G->alloc_links ();
  g->alloc_links ();

  //make all links discriminative. can only make some so.
  for (int i = 0; i<G_N_LINKS; i++) {
    G->add_link (l_nodes[i][0], l_nodes[i][1], i+1);
    double r = mzr.drand32(0, noise);
    g->add_link (l_nodes[i][0], l_nodes[i][1], i+1 + r);
  }
}

//: generate hypg with discriminative node info.
void generate_hypg_random_l (dbasnh_hypg* G, dbasnh_hypg* g, const float avg, const float noise)
{
  vnl_random mzr;
  G->alloc_links ();
  g->alloc_links ();

  //make all links discriminative. can only make some so.
  for (int i = 0; i<G_N_LINKS; i++) {
    double c = avg + mzr.drand32(0, noise);
    G->add_link (l_nodes[i][0], l_nodes[i][1], c);    
    c = avg + mzr.drand32(0, noise);
    g->add_link (l_nodes[i][0], l_nodes[i][1], c);
  }
}

//: generate hypg with discriminative node info.
void generate_hypg_discri_c (dbasnh_hypg* G, dbasnh_hypg* g, const float noise)
{
  vnl_random mzr;
  G->alloc_corners_angle ();
  g->alloc_corners_angle ();

  //make all links discriminative. can only make some so.
  for (int i = 0; i<G_N_CORNERS; i++) {
    G->add_corner_angle (c_nodes[i][0], c_nodes[i][1], c_nodes[i][2], i+1);
    double r = mzr.drand32(0, noise);
    g->add_corner_angle (c_nodes[i][0], c_nodes[i][1], c_nodes[i][2], i+1 + r);
  }
}

//: generate hypg with discriminative node info.
void generate_hypg_random_c (dbasnh_hypg* G, dbasnh_hypg* g, const float avg, const float noise)
{
  vnl_random mzr;
  G->alloc_corners_angle ();
  g->alloc_corners_angle ();

  //make all links discriminative. can only make some so.
  for (int i = 0; i<G_N_CORNERS; i++) {
    double c = avg + mzr.drand32(0, noise);
    G->add_corner_angle (c_nodes[i][0], c_nodes[i][1], c_nodes[i][2], c);
    c = avg + mzr.drand32(0, noise);
    g->add_corner_angle (c_nodes[i][0], c_nodes[i][1], c_nodes[i][2], c);
  }
}

//###################################################################

#define N_TEST_GA_STR_HYPG  16
#define SMALL_NOISE         0.1f

//###################################################################

MAIN( dbsk3dr_test_str_hypg )
{
  testlib_test_start("Grad. Asgn. on structural hypergraphs (9 nodes, 18 links, 10 sheets, 36 corners)");
  
  for (int t=0; t<N_TEST_GA_STR_HYPG; t++) {
    //Initialize hypergraph G and g
    dbasnh_hypg* G = new dbasnh_hypg ();
    dbasnh_hypg* g = new dbasnh_hypg ();
    switch (t) {
    case 0: //exactly identical
      generate_hypg_discri_n (G, g, 0);
      generate_hypg_discri_l (G, g, 0);
      generate_hypg_discri_c (G, g, 0);
    break;
    case 1: //roughly identical
      generate_hypg_discri_n (G, g, SMALL_NOISE);
      generate_hypg_discri_l (G, g, SMALL_NOISE);
      generate_hypg_discri_c (G, g, SMALL_NOISE);
    break;
    case 2: //exactly same n, l , diff c
      generate_hypg_discri_n (G, g, 0);
      generate_hypg_discri_l (G, g, 0);
      generate_hypg_random_c (G, g, 0.5, 0);
    break;
    case 3: //roughly same n, l , diff c
      generate_hypg_discri_n (G, g, SMALL_NOISE);
      generate_hypg_discri_l (G, g, SMALL_NOISE);
      generate_hypg_random_c (G, g, 0.5, SMALL_NOISE);
    break;
    case 4: //exactly same n, c , diff l
      generate_hypg_discri_n (G, g, 0);
      generate_hypg_random_l (G, g, 0.5, 0);
      generate_hypg_discri_c (G, g, 0);
    break;
    case 5: //roughly same n, c , diff l
      generate_hypg_discri_n (G, g, SMALL_NOISE);
      generate_hypg_random_l (G, g, 0.5, SMALL_NOISE);
      generate_hypg_discri_c (G, g, SMALL_NOISE);
    break;
    case 6: //exactly same l, c , diff n
      generate_hypg_random_n (G, g, 0.5, 0);
      generate_hypg_discri_l (G, g, 0);
      generate_hypg_discri_c (G, g, 0);
    break;
    case 7: //roughly same l, c , diff n
      generate_hypg_random_n (G, g, 0.5, SMALL_NOISE);
      generate_hypg_discri_l (G, g, SMALL_NOISE);
      generate_hypg_discri_c (G, g, SMALL_NOISE);
    break;
    case 8:  //exactly same n, diff l, c
      generate_hypg_discri_n (G, g, 0);
      generate_hypg_random_l (G, g, 0.5, 0);
      generate_hypg_random_c (G, g, 0.5, 0);
    break;
    case 9:  //roughly same n, diff l, c
      generate_hypg_discri_n (G, g, SMALL_NOISE);
      generate_hypg_random_l (G, g, 0.5, SMALL_NOISE);
      generate_hypg_random_c (G, g, 0.5, SMALL_NOISE);
    break;
    case 10:  //exactly same l, diff n, c
      generate_hypg_random_n (G, g, 0.5, 0);
      generate_hypg_discri_l (G, g, 0);
      generate_hypg_random_c (G, g, 0.5, 0);
    break;
    case 11:  //roughly same l, diff n, c
      generate_hypg_random_n (G, g, 0.5, SMALL_NOISE);
      generate_hypg_discri_l (G, g, SMALL_NOISE);
      generate_hypg_random_c (G, g, 0.5, SMALL_NOISE);
    break;
    case 12:  //exactly same c, diff n, l
      generate_hypg_random_n (G, g, 0.5, 0);
      generate_hypg_random_l (G, g, 0.5, 0);
      generate_hypg_discri_c (G, g, 0);
    break;
    case 13:  //roughly same c, diff n, l
      generate_hypg_random_n (G, g, 0.5, SMALL_NOISE);
      generate_hypg_random_l (G, g, 0.5, SMALL_NOISE);
      generate_hypg_discri_c (G, g, SMALL_NOISE);
    break;
    case 14:  //exactly same n, l, c
      generate_hypg_random_n (G, g, 0.5, 0);
      generate_hypg_random_l (G, g, 0.5, 0);
      generate_hypg_random_c (G, g, 0.5, 0);
    break;
    case 15:  //roughly same n, l, c
      generate_hypg_random_n (G, g, 0.5, SMALL_NOISE);
      generate_hypg_random_l (G, g, 0.5, SMALL_NOISE);
      generate_hypg_random_c (G, g, 0.5, SMALL_NOISE);
    break;
    default:
      assert (0);
    break;
    }    

    //Test graph matching
    dbasnh_gradasgn GA;
    dbasn_params params; //Use the default parameters.
    ///GA.setup_GA_params (G, g, params);
    GA.set_G (G);
    GA.set_g (g);
    GA.set_params (params);
    GA._reset_mem ();
    //0: Silent, 1: only matching matrix, 2: basic results, 3: more details. 4: full necessary details.
    GA.set_debug_out (1);
    //option abs_max: true: use absolute max of G and g.
    //                false: normalize w.r.t. the max of each graph.
    GA.normalize_costs (false); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad = GA.compare_to_gtruth (str_labelgG, G_N_NODES);    
    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("exactly identical: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("roughly identical: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("exactly same n, l , diff c: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("roughly same n, l , diff c: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("roughly exactly same n, c , diff l: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 5: 
      s = vul_sprintf ("roughly same n, c , diff l: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 6: 
      s = vul_sprintf ("exactly same l, c , diff n: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 7: 
      s = vul_sprintf ("roughly same l, c , diff n: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 8: 
      s = vul_sprintf ("exactly same n, diff l, c: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 9: 
      s = vul_sprintf ("roughly same n, diff l, c: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 10: 
      s = vul_sprintf ("exactly same l, diff n, c: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 11: 
      s = vul_sprintf ("roughly same l, diff n, c: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 12: 
      s = vul_sprintf ("exactly same c, diff n, l: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 13: 
      s = vul_sprintf ("roughly same c, diff n, l: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 14: 
      s = vul_sprintf ("exactly same n, l, c: %d bad matches.", bad);
      TEST (s.c_str(), bad<=G_N_NODES, true); //dummy test
    break;
    case 15: 
      s = vul_sprintf ("roughly same n, l, c: %d bad matches.", bad);
      TEST (s.c_str(), bad<=G_N_NODES, true); //dummy test
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cout,"\n");

    delete G;
    delete g;
  }

  return testlib_test_summary();
}
 


