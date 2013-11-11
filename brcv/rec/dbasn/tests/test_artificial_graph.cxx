//---------------------------------------------------------------------
// This is brcv/rec/dbasn/tests/test_random_graph.cxx
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
//   Ming Ching Chang   -  Extend the grad. assignment tests on random graphs.
// \endverbatim
//
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbasn\tests\file_read_data
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vul/vul_printf.h>
#include <vul/vul_sprintf.h>
#include <testlib/testlib_test.h>

#include <dbasn/dbasn_gradasgn.h>
#include <dbasn/dbasn_graph_algos.h>

#define G_N_NODES   3
#define G_N_LINKS   3

//Generate a simple artificial graph with 3 nodes and 3 links.
void generate_graph_3n_3l (dbasn_graph* G, float* n_costs, float* l_costs)
{
  for (int i = 0; i<G_N_NODES; i++)
    G->_add_node (new dbasn_node (n_costs[i], i));

  G->alloc_links ();  //Allocate space for links.

  G->add_link (0, 1, l_costs[0]);
  G->add_link (1, 2, l_costs[1]);
  G->add_link (0, 2, l_costs[2]);  
}

//Test graph database.
float n_equal[] = {1, 1, 1};
float n_ident[] = {1, 2, 3};
float n_scale[] = {2, 4, 6};
float n_modi1[] = {1, 2, 4};
float n_modi2[] = {1, 2.5, 3.5};
float n_modi3[] = {1.3, 2.2, 4.5};

float n_rotat[] = {3, 1, 2};

float n_flip_id[] = {1, 3, 2};
float n_flip_m1[] = {1, 4, 2};
float n_flip_m2[] = {1, 3.5, 2.5};
float n_flip_m3[] = {1.3, 4.5, 2.2};

float l_zeros[] = {0, 0, 0};
float l_equal[] = {1, 1, 1};
float l_ident[] = {1, 2, 3}; //{1, 2, 2.08}; //
float l_scale[] = {2, 4, 6};
float l_modi1[] = {1, 2, 4};
float l_modi2[] = {1, 2.5, 3.5};
float l_modi3[] = {1.3, 2.2, 4.5};

float l_rotat[] = {3, 1, 2};

float l_flip_id[] = {1, 3, 2}; //{1, 2.08, 2}; //
//float l_flip_m1[] = {1, 2, 4};
//float l_flip_m2[] = {1, 2.5, 3.5};
//float l_flip_m3[] = {1.3, 2.2, 4.5};

//ground truth for the standard cases.
int labelgG[] = {0, 1, 2}; //labelgG [g's node id] = G's node id.

//ground truth for the rotated case.
int labelg_rotat[] = {2, 0, 1};

//ground truth for the flip2 case.
int labelg_flip2[] = {1, 0, 2};

//###################################################################

#define N_TEST_GA_NODES_ONLY  5

void test_ga_nodes_only ()
{
  for (int t=0; t<N_TEST_GA_NODES_ONLY; t++) {
    //Initialize graph G
    dbasn_graph* G = new dbasn_graph ();
    generate_graph_3n_3l (G, n_ident, l_zeros);

    //Initialize graph g
    dbasn_graph* g = new dbasn_graph ();
    switch (t) {
    case 0: 
      generate_graph_3n_3l (g, n_ident, l_zeros);
    break;
    case 1: 
      generate_graph_3n_3l (g, n_scale, l_zeros);
    break;
    case 2: 
      generate_graph_3n_3l (g, n_modi1, l_zeros);
    break;
    case 3: 
      generate_graph_3n_3l (g, n_modi2, l_zeros);
    break;
    case 4: 
      generate_graph_3n_3l (g, n_modi3, l_zeros);
    break;
    default:
      assert (0);
    break;
    }    

    //Test graph matching
    dbasn_gradasgn GA;
    dbasn_params params; //Use the default parameters.
    GA.setup_GA_params (G, g, params);
    GA.set_debug_out (1); //0: Silent, 1: only matching matrix.
    GA.normalize_costs (); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad = GA.compare_to_gtruth (labelgG, G_N_NODES);    
    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("nodes_only identical graphs: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("nodes_only graphs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("nodes_only 1 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("nodes_only 2 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("nodes_only 3 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cerr,"\n");

    delete G;
    delete g;
  }
}

//###################################################################

#define N_TEST_GA_DIFF_N_SAME_E  5

void test_ga_diff_n_same_e ()
{
  for (int t=0; t<N_TEST_GA_DIFF_N_SAME_E; t++) {
    //Initialize graph G
    dbasn_graph* G = new dbasn_graph ();
    generate_graph_3n_3l (G, n_ident, l_equal);

    //Initialize graph g
    dbasn_graph* g = new dbasn_graph ();
    switch (t) {
    case 0: 
      generate_graph_3n_3l (g, n_ident, l_equal);
    break;
    case 1: 
      generate_graph_3n_3l (g, n_scale, l_equal);
    break;
    case 2: 
      generate_graph_3n_3l (g, n_modi1, l_equal);
    break;
    case 3: 
      generate_graph_3n_3l (g, n_modi2, l_equal);
    break;
    case 4: 
      generate_graph_3n_3l (g, n_modi3, l_equal);
    break;
    default:
      assert (0);
    break;
    }    

    //Test graph matching
    dbasn_gradasgn GA;
    dbasn_params params; //Use the default parameters.
    GA.setup_GA_params (G, g, params);
    GA.set_debug_out (1); //0: Silent, 1: only matching matrix.
    GA.normalize_costs (); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad = GA.compare_to_gtruth (labelgG, G_N_NODES);    
    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("diff_n_same_e identical graphs: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("diff_n_same_e graphs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("diff_n_same_e 1 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("diff_n_same_e 2 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("diff_n_same_e 3 modif. N: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cerr,"\n");

    delete G;
    delete g;
  }
}

//###################################################################

#define N_TEST_GA_SAME_N_DIFF_E  5

void test_ga_same_n_diff_e ()
{
  for (int t=0; t<N_TEST_GA_SAME_N_DIFF_E; t++) {
    //Initialize graph G
    dbasn_graph* G = new dbasn_graph ();
    generate_graph_3n_3l (G, n_equal, l_ident);

    //Initialize graph g
    dbasn_graph* g = new dbasn_graph ();
    switch (t) {
    case 0: 
      generate_graph_3n_3l (g, n_equal, l_ident);
    break;
    case 1: 
      generate_graph_3n_3l (g, n_equal, l_scale);
    break;
    case 2: 
      generate_graph_3n_3l (g, n_equal, l_modi1);
    break;
    case 3: 
      generate_graph_3n_3l (g, n_equal, l_modi2);
    break;
    case 4: 
      generate_graph_3n_3l (g, n_equal, l_modi3);
    break;
    default:
      assert (0);
    break;
    }  

    //Test graph matching
    dbasn_gradasgn GA;
    dbasn_params params; //Use the default parameters.
    GA.setup_GA_params (G, g, params);
    //0: Silent, 1: only matching matrix, 2: basic results, 3: more details. 4: full necessary details.
    GA.set_debug_out (1); 
    GA.normalize_costs (); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad = GA.compare_to_gtruth (labelgG, G_N_NODES);    
    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("same_n_diff_e identical graphs: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("same_n_diff_e graphs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("same_n_diff_e 1 modif. L: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("same_n_diff_e 2 modif. L: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("same_n_diff_e 3 modif. L: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cerr,"\n");

    delete G;
    delete g;  
  }
}

//###################################################################

#define N_TEST_GA_DIFF_N_E  5

void test_ga_diff_n_e ()
{
  for (int t=0; t<N_TEST_GA_DIFF_N_E; t++) {
    //Initialize graph G
    dbasn_graph* G = new dbasn_graph ();
    generate_graph_3n_3l (G, n_ident, l_ident);

    //Initialize graph g
    dbasn_graph* g = new dbasn_graph ();
    switch (t) {
    case 0: 
      generate_graph_3n_3l (g, n_rotat, l_rotat);
    break;
    case 1: 
      generate_graph_3n_3l (g, n_scale, l_scale);
    break;
    case 2: 
      generate_graph_3n_3l (g, n_modi1, l_modi1);
    break;
    case 3: 
      generate_graph_3n_3l (g, n_modi2, l_modi2);
    break;
    case 4: 
      generate_graph_3n_3l (g, n_modi3, l_modi3);
    break;
    default:
      assert (0);
    break;
    }  

    //Test graph matching
    dbasn_gradasgn GA;
    dbasn_params params; //Use the default parameters.
    GA.setup_GA_params (G, g, params);
    //0: Silent, 1: only matching matrix, 2: basic results, 3: more details. 4: full necessary details.
    GA.set_debug_out (1); 
    GA.normalize_costs (); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad;
    if (t==0)
      bad = GA.compare_to_gtruth (labelg_rotat, G_N_NODES);    
    else
      bad = GA.compare_to_gtruth (labelgG, G_N_NODES);

    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("diff_n_e rotation of indices: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("diff_n_e graphs with scaling: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("diff_n_e 1 modif. L: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("diff_n_e 2 modif. L: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("diff_n_e 3 modif. L: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cerr,"\n");

    delete G;
    delete g;  
  }
}

//###################################################################

#define N_TEST_GA_DIFF_N_E_FLIP   5

void test_ga_diff_n_e_flip ()
{
  for (int t=0; t<N_TEST_GA_DIFF_N_E_FLIP; t++) {
    //Initialize graph G
    dbasn_graph* G = new dbasn_graph ();
    generate_graph_3n_3l (G, n_ident, l_ident);

    //Initialize graph g
    dbasn_graph* g = new dbasn_graph ();
    switch (t) {
    case 0: 
      generate_graph_3n_3l (g, n_flip_id, l_ident);
    break;
    case 1: 
      generate_graph_3n_3l (g, n_flip_id, l_flip_id);
    break;
    case 2: 
      generate_graph_3n_3l (g, n_flip_m1, l_modi1);
    break;
    case 3: 
      generate_graph_3n_3l (g, n_flip_m2, l_modi2);
    break;
    case 4: 
      generate_graph_3n_3l (g, n_flip_m3, l_modi3);
    break;
    default:
      assert (0);
    break;
    }  

    //Test graph matching
    dbasn_gradasgn GA;
    dbasn_params params; //Use the default parameters.
    GA.setup_GA_params (G, g, params);
    //0: Silent, 1: only matching matrix, 2: basic results, 3: more details. 4: full necessary details.
    GA.set_debug_out (1); 
    GA.normalize_costs (); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad;
    if (t==1)
      bad = GA.compare_to_gtruth (labelg_flip2, G_N_NODES);
    else
      bad = GA.compare_to_gtruth (labelgG, G_N_NODES);

    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("diff_n_e_flip noeds: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("diff_n_e_flip nodes and links: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("diff_n_e_flip 1 modif. L: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("diff_n_e_flip 2 modif. L: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 4: 
      s = vul_sprintf ("diff_n_e_flip 3 modif. L: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    default:
      assert (0);
    break;
    }       
    vul_printf (vcl_cerr,"\n");

    delete G;
    delete g;  
  }
}

//###################################################################

MAIN( test_artificial_graph )
{
  testlib_test_start("Grad. Asgn. on artificial graphs (3 nodes, 3 links)");
  
  test_ga_nodes_only ();
  test_ga_diff_n_same_e ();
  test_ga_same_n_diff_e ();
  test_ga_diff_n_e ();
  test_ga_diff_n_e_flip ();
  
  return testlib_test_summary();
}

