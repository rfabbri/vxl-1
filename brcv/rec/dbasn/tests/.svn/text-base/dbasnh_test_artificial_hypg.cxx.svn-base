//---------------------------------------------------------------------
// This is brcv/rec/dbasn/tests/dbasnh_test_random_hypg.cxx
//:
// \file
// \brief This is the Graduated Assignment test program
//        In this test, we generate a random graph G, and generate a noisy subgraph g.
//        Then we compare G and g using dbasn_gradasgn.
//        Since it's subgraph matching, we know the ground truth, enabling calculating matching rate.
//
// \author
//  Ming-Ching Chang - January 7, 2008
//
// \verbatim
//  Modifications
// \endverbatim
//
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbasn\tests\file_read_data
//  test_artificial_hypg D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbasn\tests\data
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vul/vul_printf.h>
#include <vul/vul_sprintf.h>
#include <testlib/testlib_test.h>

#include <dbasn/dbasnh_gradasgn.h>
#include <dbasn/dbasnh_hypg_algos.h>

#include "dbasn_test_artificial_def.h"

#define G_N_NODES   3
#define G_N_LINKS   3

//Generate a simple artificial graph with 3 nodes and 3 links.
void generate_hypg_3n_3l_3c (dbasnh_hypg* G, float* n_costs, float* l_costs, float* c_costs)
{
  for (int i = 0; i<G_N_NODES; i++)
    G->_add_node (new dbasn_node (i, n_costs[i]));

  G->alloc_links ();  //Allocate space for links.
  G->add_link (0, 1, l_costs[0]);
  G->add_link (1, 2, l_costs[1]);
  G->add_link (0, 2, l_costs[2]);  

  G->alloc_corners_angle (); //Allocate space for corners.
  G->add_corner_angle (2, 0, 1, c_costs[0]);
  G->add_corner_angle (0, 1, 2, c_costs[1]);
  G->add_corner_angle (1, 2, 0, c_costs[2]);
}

//###################################################################

#define N_TEST_GA_SAME_N_E_DIFF_C  5

void test_ga_same_n_e_diff_c ()
{
  for (int t=0; t<N_TEST_GA_SAME_N_E_DIFF_C; t++) {
    //Initialize hypergraph G
    dbasnh_hypg* G = new dbasnh_hypg ();
    generate_hypg_3n_3l_3c (G, n_equal, l_equal, c_ident);

    //Initialize graph g
    dbasnh_hypg* g = new dbasnh_hypg ();
    switch (t) {
    case 0: 
      generate_hypg_3n_3l_3c (g, n_equal, l_equal, c_ident);
    break;
    case 1: 
      generate_hypg_3n_3l_3c (g, n_equal, l_equal, c_scale);
    break;
    case 2: 
      generate_hypg_3n_3l_3c (g, n_equal, l_equal, c_modi1);
    break;
    case 3: 
      generate_hypg_3n_3l_3c (g, n_equal, l_equal, c_modi2);
    break;
    case 4: 
      generate_hypg_3n_3l_3c (g, n_equal, l_equal, c_modi3);
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
    GA.normalize_costs (false); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad = GA.compare_to_gtruth (labelgG, G_N_NODES);    
    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("same_n_e_diff_c identical hypgs: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("same_n_e_diff_c hypgs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("same_n_e_diff_c 1 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("same_n_e_diff_c 2 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("same_n_e_diff_c 3 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cout,"\n");

    delete G;
    delete g;
  }
}

//###################################################################

#define N_TEST_GA_SAME_N_C_DIFF_L  5

void test_ga_same_n_c_diff_l ()
{
  for (int t=0; t<N_TEST_GA_SAME_N_C_DIFF_L; t++) {
    //Initialize hypergraph G
    dbasnh_hypg* G = new dbasnh_hypg ();
    generate_hypg_3n_3l_3c (G, n_equal, l_ident, c_equal);

    //Initialize graph g
    dbasnh_hypg* g = new dbasnh_hypg ();
    switch (t) {
    case 0: 
      generate_hypg_3n_3l_3c (g, n_equal, l_ident, c_equal);
    break;
    case 1: 
      generate_hypg_3n_3l_3c (g, n_equal, l_scale, c_equal);
    break;
    case 2: 
      generate_hypg_3n_3l_3c (g, n_equal, l_modi1, c_equal);
    break;
    case 3: 
      generate_hypg_3n_3l_3c (g, n_equal, l_modi2, c_equal);
    break;
    case 4: 
      generate_hypg_3n_3l_3c (g, n_equal, l_modi3, c_equal);
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
    GA.normalize_costs (false); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad = GA.compare_to_gtruth (labelgG, G_N_NODES);    
    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("same_n_c_diff_l identical hypgs: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("same_n_c_diff_l hypgs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("same_n_c_diff_l 1 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("same_n_c_diff_l 2 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("same_n_c_diff_l 3 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cout,"\n");

    delete G;
    delete g;
  }
}

//###################################################################

#define N_TEST_GA_SAME_L_C_DIFF_N  5

void test_ga_same_l_c_diff_n ()
{
  for (int t=0; t<N_TEST_GA_SAME_L_C_DIFF_N; t++) {
    //Initialize hypergraph G
    dbasnh_hypg* G = new dbasnh_hypg ();
    generate_hypg_3n_3l_3c (G, n_ident, l_equal, c_equal);

    //Initialize graph g
    dbasnh_hypg* g = new dbasnh_hypg ();
    switch (t) {
    case 0: 
      generate_hypg_3n_3l_3c (g, n_ident, l_equal, c_equal);
    break;
    case 1: 
      generate_hypg_3n_3l_3c (g, n_scale, l_equal, c_equal);
    break;
    case 2: 
      generate_hypg_3n_3l_3c (g, n_modi1, l_equal, c_equal);
    break;
    case 3: 
      generate_hypg_3n_3l_3c (g, n_modi2, l_equal, c_equal);
    break;
    case 4: 
      generate_hypg_3n_3l_3c (g, n_modi3, l_equal, c_equal);
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
    GA.normalize_costs (false); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad = GA.compare_to_gtruth (labelgG, G_N_NODES);    
    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("same_l_c_diff_n identical hypgs: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("same_l_c_diff_n hypgs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("same_l_c_diff_n 1 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("same_l_c_diff_n 2 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("same_l_c_diff_n 3 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cout,"\n");

    delete G;
    delete g;
  }
}

//###################################################################

#define N_TEST_GA_SAME_N_DIFF_L_C  5

void test_ga_same_n_diff_l_c ()
{
  for (int t=0; t<N_TEST_GA_SAME_N_DIFF_L_C; t++) {
    //Initialize hypergraph G
    dbasnh_hypg* G = new dbasnh_hypg ();
    generate_hypg_3n_3l_3c (G, n_ident, l_ident, c_ident);

    //Initialize graph g
    dbasnh_hypg* g = new dbasnh_hypg ();
    switch (t) {
    case 0: 
      generate_hypg_3n_3l_3c (g, n_ident, l_rotat, c_rotat);
    break;
    case 1: 
      generate_hypg_3n_3l_3c (g, n_ident, l_scale, c_scale);
    break;
    case 2: 
      generate_hypg_3n_3l_3c (g, n_ident, l_modi1, c_modi1);
    break;
    case 3: 
      generate_hypg_3n_3l_3c (g, n_ident, l_modi2, c_modi2);
    break;
    case 4: 
      generate_hypg_3n_3l_3c (g, n_ident, l_modi3, c_modi3);
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
    GA.normalize_costs (false); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad;
    if (t==0)
      bad = GA.compare_to_gtruth (labelg_rotat, G_N_NODES);    
    else
      bad = GA.compare_to_gtruth (labelgG, G_N_NODES);

    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("same_n_diff_l_c rotated hypgs: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("same_n_diff_l_c hypgs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("same_n_diff_l_c 1 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("same_n_diff_l_c 2 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("same_n_diff_l_c 3 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cout,"\n");

    delete G;
    delete g;
  }
}

//###################################################################

#define N_TEST_GA_SAME_L_DIFF_N_C  5

void test_ga_same_l_diff_n_c ()
{
  for (int t=0; t<N_TEST_GA_SAME_L_DIFF_N_C; t++) {
    //Initialize hypergraph G
    dbasnh_hypg* G = new dbasnh_hypg ();
    generate_hypg_3n_3l_3c (G, n_ident, l_ident, c_ident);

    //Initialize graph g
    dbasnh_hypg* g = new dbasnh_hypg ();
    switch (t) {
    case 0: 
      generate_hypg_3n_3l_3c (g, n_rotat, l_ident, c_rotat);
    break;
    case 1: 
      generate_hypg_3n_3l_3c (g, n_scale, l_ident, c_scale);
    break;
    case 2: 
      generate_hypg_3n_3l_3c (g, n_modi1, l_ident, c_modi1);
    break;
    case 3: 
      generate_hypg_3n_3l_3c (g, n_modi2, l_ident, c_modi2);
    break;
    case 4: 
      generate_hypg_3n_3l_3c (g, n_modi3, l_ident, c_modi3);
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
    GA.normalize_costs (false); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad;
    if (t==0)
      bad = GA.compare_to_gtruth (labelg_rotat, G_N_NODES);    
    else
      bad = GA.compare_to_gtruth (labelgG, G_N_NODES);

    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("same_l_diff_n_c rotated hypgs: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("same_l_diff_n_c hypgs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("same_l_diff_n_c 1 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("same_l_diff_n_c 2 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("same_l_diff_n_c 3 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cout,"\n");

    delete G;
    delete g;
  }
}

//###################################################################

#define N_TEST_GA_SAME_C_DIFF_N_L  5

void test_ga_same_c_diff_n_l ()
{
  for (int t=0; t<N_TEST_GA_SAME_C_DIFF_N_L; t++) {
    //Initialize hypergraph G
    dbasnh_hypg* G = new dbasnh_hypg ();
    generate_hypg_3n_3l_3c (G, n_ident, l_ident, c_ident);

    //Initialize graph g
    dbasnh_hypg* g = new dbasnh_hypg ();
    switch (t) {
    case 0: 
      generate_hypg_3n_3l_3c (g, n_rotat, l_rotat, c_ident);
    break;
    case 1: 
      generate_hypg_3n_3l_3c (g, n_scale, l_scale, c_ident);
    break;
    case 2: 
      generate_hypg_3n_3l_3c (g, n_modi1, l_modi1, c_ident);
    break;
    case 3: 
      generate_hypg_3n_3l_3c (g, n_modi2, l_modi2, c_ident);
    break;
    case 4: 
      generate_hypg_3n_3l_3c (g, n_modi3, l_modi3, c_ident);
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
    GA.normalize_costs (false); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad;
    if (t==0)
      bad = GA.compare_to_gtruth (labelg_rotat, G_N_NODES);    
    else
      bad = GA.compare_to_gtruth (labelgG, G_N_NODES);

    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("test_ga_same_c_diff_n_l rotated hypgs: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("test_ga_same_c_diff_n_l hypgs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("test_ga_same_c_diff_n_l 1 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("test_ga_same_c_diff_n_l 2 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("test_ga_same_c_diff_n_l 3 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cout,"\n");

    delete G;
    delete g;
  }
}

//###################################################################

#define N_TEST_GA_DIFF_N_L_C  5

void test_ga_diff_n_l_c ()
{
  for (int t=0; t<N_TEST_GA_DIFF_N_L_C; t++) {
    //Initialize hypergraph G
    dbasnh_hypg* G = new dbasnh_hypg ();
    generate_hypg_3n_3l_3c (G, n_ident, l_ident, c_ident);

    //Initialize graph g
    dbasnh_hypg* g = new dbasnh_hypg ();
    switch (t) {
    case 0: 
      generate_hypg_3n_3l_3c (g, n_rotat, l_rotat, c_rotat);
    break;
    case 1: 
      generate_hypg_3n_3l_3c (g, n_scale, l_scale, c_scale);
    break;
    case 2: 
      generate_hypg_3n_3l_3c (g, n_modi1, l_modi1, c_modi1);
    break;
    case 3: 
      generate_hypg_3n_3l_3c (g, n_modi2, l_modi2, c_modi2);
    break;
    case 4: 
      generate_hypg_3n_3l_3c (g, n_modi3, l_modi3, c_modi3);
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
    GA.normalize_costs (false); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad;
    if (t==0)
      bad = GA.compare_to_gtruth (labelg_rotat, G_N_NODES);    
    else
      bad = GA.compare_to_gtruth (labelgG, G_N_NODES);

    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("test_ga_diff_n_l_c rotated hypgs: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("test_ga_diff_n_l_c hypgs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("test_ga_diff_n_l_c 1 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("test_ga_diff_n_l_c 2 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("test_ga_diff_n_l_c 3 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cout,"\n");

    delete G;
    delete g;
  }
}

//###################################################################



    /*int bad;
    if (t==1)
      bad = GA.compare_to_gtruth (labelg_flip2, G_N_NODES);
    else
      bad = GA.compare_to_gtruth (labelgG, G_N_NODES);*/

//###################################################################

MAIN( dbasnh_test_artificial_hypg )
{
  testlib_test_start("Grad. Asgn. on artificial hypergraphs (3 nodes, 3 links, 3 corners)");
  
  test_ga_same_n_e_diff_c ();  
  test_ga_same_n_c_diff_l ();
  test_ga_same_l_c_diff_n ();
  
  test_ga_same_n_diff_l_c ();
  test_ga_same_l_diff_n_c ();
  test_ga_same_c_diff_n_l ();

  test_ga_diff_n_l_c ();
  
  return testlib_test_summary();
}

