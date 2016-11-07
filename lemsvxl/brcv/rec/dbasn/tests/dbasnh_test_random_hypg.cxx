//---------------------------------------------------------------------
// This is brcv/rec/dbasn/tests/dbasnh_test_simple_hypg.cxx
//:
// \file
// \brief Test grad. assign. matching of hypergraphs.
//        In this test, we generate a random hypergraph G, and generate a noisy sub-hypergraph g.
//        Then we compare G and g using dbasnh_gradassign.
//        Since it's subgraph matching, we know the ground truth, enabling calculating matching rate.
//
// \author
//  Ming-Ching Chang - January 6, 2008.
//
// \verbatim
//  Modifications
// \endverbatim
//
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbasn\tests\data
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vul/vul_printf.h>
#include <testlib/testlib_test.h>

#include <dbasn/dbasn_graph_algos.h>
#include <dbasn/dbasnh_gradasgn.h>
#include <dbasn/dbasnh_hypg_algos.h>

#define G_N_NODE       10       // # of nodes in G
#define G_LINK_CONN    0.2f     // connectivity (0 ~ 0.5): # of links = G_N_NODE*G_N_NODE*con (def: 0.3)
#define SUB_G_N        7        // # of nodes in g
#define L_NOISE        0.2f     // 0~1 noise add to the link of the subgraph g (def: 0.3)
                                // give 0 noise to get an exact match
#define N_TEST         100      // number of tests (1, 100)

#define G_CORNER_CONN  0.5f     // connectivity (0 ~ 1): # of corners = G_N_NODE*G_N_NODE*con (def: 0.8)
#define C_NOISE        0.2f     // 0~1 noise add to the corner of the subgraph g (def: 0.3, 0.0)

#define REMOVE_L       0.2      // 20 % of links removed in the subgraph.
#define REMOVE_C       0.2      // 20 % of corners removed in the subgraph.

MAIN( dbasnh_test_random_hypg )
{
  testlib_test_start("Grad. Asgn. on random hypergraphs.");

  vcl_cout <<"Hypergraph G with nodes: "<< G_N_NODE << ", connectivity percentage: "<< G_LINK_CONN << vcl_endl;
  vcl_cout <<"Match with Sub-graph g with nodes: "<< SUB_G_N << vcl_endl;
  vcl_cout <<"Noise: " << L_NOISE << vcl_endl;
  vcl_cout <<"Repeat test for " << N_TEST << " times."<< vcl_endl;

  unsigned int incorrect_hypg = 0;
  unsigned int error_hypg = 0;
  unsigned int incorrect_graph = 0;
  unsigned int error_graph = 0;
  for (int count = 0; count < N_TEST; count++) {

    int *labelg = new int[SUB_G_N+1]; //'+1' for slacks
    //1)Initialize test graphs
    dbasnh_hypg* G = new dbasnh_hypg ();    
    generate_random_graph (G, G_N_NODE, G_LINK_CONN);
    generate_random_hypg (G, G_CORNER_CONN);

    dbasnh_hypg* g = new dbasnh_hypg ();
    generate_noisy_subgraph (g, G, SUB_G_N, labelg, true, true, L_NOISE, REMOVE_L);
    generate_noisy_subhypg (g, G, labelg, C_NOISE, REMOVE_C);

    //Testing simple graph matching
    dbasnh_gradasgn GA;
    dbasn_params params; //Use the default parameters. 
    vcl_cout<< params;
    ///GA.setup_GA_params (G, g, params);
    GA.set_G (G);
    GA.set_g (g);
    GA.set_params (params);
    GA._reset_mem ();
    //0: Silent, 1: only matching matrix, 2: basic results, 3: more details. 4: full necessary details.
    GA.set_debug_out (1);

    //2)Normalize the node_cost and link_cost to between 0 and 1
    GA.normalize_costs (false);

    //3)Run Grad. Asgn. hypergraph matching.
    GA.get_assignment();

    //Test result
    int bad = 0;
    for (int i = 0; i<SUB_G_N; i++) {
      if (i != GA.labelGg(labelg[i])) 
        bad++;
    }
    vul_printf (vcl_cout, "Hypergraph Matching Test # %d: %d bad matches (out of %d).\n", count, bad, SUB_G_N);
    if (bad)
      error_hypg++;
    incorrect_hypg += bad;
    
    //4)Run Grad. Asgn. graph matching.
    GA.dbasn_gradasgn::get_assignment ();

    //Test result
    bad = 0;
    for (int i = 0; i<SUB_G_N; i++) {
      if (i != GA.labelGg(labelg[i])) 
        bad++;
    }
    vul_printf (vcl_cout, "Graph Matching Test # %d: %d bad matches (out of %d).\n", count, bad, SUB_G_N);
    if (bad)
      error_graph++;
    incorrect_graph += bad;

    delete G;
    delete g;
    delete labelg;
  }

  vul_printf (vcl_cout, "\n Totally %d errors / %d bad matches in %d hypergraph matching tests.\n",
              error_hypg, incorrect_hypg, N_TEST);
  vul_printf (vcl_cout, " Totally %d errors / %d bad matches in %d graph matching tests.\n\n",
              error_graph, incorrect_graph, N_TEST);

  return testlib_test_summary();
}

