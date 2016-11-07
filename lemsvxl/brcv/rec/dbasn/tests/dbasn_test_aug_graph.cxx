//---------------------------------------------------------------------
// This is brcv/rec/dbasn/tests/dbasn_test_aug_graph.cxx
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
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbasn\tests\data
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

#include "dbasn_test_artificial_def.h"

#define G_N_NODES   3
#define G_N_LINKS   3

//Generate a simple artificial graph with 3 nodes and 3 links.
void generate_graph_aug_3n_3l (dbasn_graph_aug* G, float* n_costs, float* l_costs, 
                               char* n_types, char* l_types)
{
  for (int i = 0; i<G_N_NODES; i++)
    G->_add_node (new dbasn_node_aug (i, n_costs[i], n_types[i]));

  G->alloc_links ();  //Allocate space for links.
  G->alloc_links_type ();  //Allocate space for links_type.

  G->add_link (0, 1, l_costs[0]);
  G->add_link (1, 2, l_costs[1]);
  G->add_link (0, 2, l_costs[2]);
  G->add_link_type (0, 1, l_types[0]);
  G->add_link_type (1, 2, l_types[1]);
  G->add_link_type (0, 2, l_types[2]);
}

//###################################################################

#define N_TEST_GA_AUG_GRAPH_TYPES  4

void test_ga_aug_graph_types ()
{
  for (int t=0; t<N_TEST_GA_AUG_GRAPH_TYPES; t++) {
    //Initialize graph G
    dbasn_graph_aug* G = new dbasn_graph_aug ();
    switch (t) {
    case 0: 
      generate_graph_aug_3n_3l (G, n_ident, l_ident, n_types, l_types);
    break;
    case 1: 
      generate_graph_aug_3n_3l (G, n_equal, l_equal, n_types, l_types);
    break;
    case 2: 
      generate_graph_aug_3n_3l (G, n_pertb, l_pertb, n_types, l_types);
    break;
    case 3: 
      generate_graph_aug_3n_3l (G, n_pertb, l_pertb, n_types, l_types);
    break;
    default:
      assert (0);
    break;
    }    

    //Initialize graph g
    dbasn_graph_aug* g = new dbasn_graph_aug ();
    switch (t) {
    case 0: 
      generate_graph_aug_3n_3l (g, n_ident, l_ident, n_types, l_types);
    break;
    case 1: 
      generate_graph_aug_3n_3l (g, n_equal, l_equal, n_types, l_types);
    break;
    case 2: 
      generate_graph_aug_3n_3l (g, n_pertb2, l_pertb2, n_types, l_types);
    break;
    case 3: 
      generate_graph_aug_3n_3l (g, n_pert_rot, l_pert_rot, n_types, l_types);
    break;
    default:
      assert (0);
    break;
    }    

    //Test graph matching
    dbasn_gradasgn_aug GA;
    dbasn_params params; //Use the default parameters.
    ///GA.setup_GA_params (G, g, params);
    GA.set_G (G);
    GA.set_g (g);
    GA.set_params (params);
    GA._reset_mem ();

    GA.set_debug_out (1); //0: Silent, 1: only matching matrix.
    GA.normalize_costs (false); //Normalize costs.

    GA.get_assignment(); //Run Graduated Assignment

    int bad = GA.compare_to_gtruth (labelgG, G_N_NODES);    
    vcl_string s;
    switch (t) {
    case 0: 
      s = vul_sprintf ("graph_types identical Ns and Es: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 1: 
      s = vul_sprintf ("graph_types equal Ns and Es: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 2: 
      s = vul_sprintf ("graph_types perturbed Ns and Es: %d bad matches.", bad);
      TEST (s.c_str(), bad, 0);
    break;
    case 3: 
      s = vul_sprintf ("graph_types rotated perturbed Ns and Es: %d bad matches.", bad);
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

MAIN( dbasn_test_aug_graph )
{
  testlib_test_start("Grad. Asgn. on augmented graphs (3 nodes, 3 links)");
   
  test_ga_aug_graph_types ();
  
  return testlib_test_summary();
}

