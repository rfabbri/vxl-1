//---------------------------------------------------------------------
// This is brcv/rec/dbasn/tests/dbasn_test_random_graph.cxx
//:
// \file
// \brief This is the Graduated Assignment test program
//        In this test, we generate a random graph G, and generate a noisy subgraph g.
//        Then we compare G and g using dbasn_gradasgn.
//        Since it's subgraph matching, we know the ground truth, enabling calculating matching rate.
//
// \author
//  O.C. Ozcanli - January 22, 2004
//
// \verbatim
//  Modifications
//   Ming Ching Chang   -  Extend the grad. assignment tests on random graphs.
// \endverbatim
//
//  To debug in Visual studio, put argument
//  dbasn_test_random_graph
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbasn\tests\data
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vul/vul_printf.h>
#include <testlib/testlib_test.h>

#include <dbasn/dbasn_gradasgn.h>
#include <dbasn/dbasn_graph_algos.h>

#define G_N_NODE      10        // # of nodes in G
#define G_LINK_CONN   0.3f      // connectivity (0 ~ 0.5): # of links = G_N_NODE*G_N_NODE*con (def: 0.3)
#define SUB_G_N       7         // # of nodes in g
#define L_NOISE       0.0f      // 0~1 noise add to the link of the subgraph g (def: 0.3, 0.2)
                                // give 0 noise to get an exact match
#define N_TEST        100       // number of tests (1, 100)

#define REMOVE_L      0.1       // 0.2: 20 % of links removed in the subgraph.

MAIN( dbasn_test_random_graph )
{
  testlib_test_start("Grad. Asgn. on random graphs.");

  vcl_cout <<"Graph G with nodes: "<< G_N_NODE << ", connectivity percentage: "<< G_LINK_CONN << vcl_endl;
  vcl_cout <<"Match with Sub-graph g with nodes: "<< SUB_G_N << vcl_endl;
  vcl_cout <<"Noise: " << L_NOISE << vcl_endl;
  vcl_cout <<"Repeat test for " << N_TEST << " times."<< vcl_endl;

  unsigned int incorrect = 0;
  unsigned int total_bad = 0;
  for (int count = 0; count < N_TEST; count++) {

    //labelg [g's node id] = G's node id.
    int *labelg = new int[SUB_G_N+1]; //'+1' for slacks

    //1)Initialize test graphs
    dbasn_graph* G = new dbasn_graph ();
    generate_random_graph (G, G_N_NODE, G_LINK_CONN);
    dbasn_graph* g = new dbasn_graph ();
    generate_noisy_subgraph (g, G, SUB_G_N, labelg, true, true, L_NOISE, REMOVE_L);

    //Testing simple graph matching
    dbasn_gradasgn GA;
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

    //3)Run Graduated Assignment
    GA.get_assignment();

    //Test result
    int bad = 0;
    for (int i = 0; i<SUB_G_N; i++) {
      if (i != GA.labelGg(labelg[i])) 
        bad++;
    }
    vcl_cout<<" Test # " << count << ": " << bad <<" bad matches out of " << SUB_G_N <<vcl_endl;

    total_bad += bad;
    if (bad != 0)
      incorrect++;
    
    delete G;
    delete g;
    delete labelg;
  }

  vul_printf (vcl_cout, "\n Out of %d graph matching tests, %d fails, %d bad matches.\n\n",
              N_TEST, incorrect, total_bad);
  TEST("# of incorrect matches (should be 0) ", incorrect, 0);
  
  return testlib_test_summary();
}

