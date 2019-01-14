//---------------------------------------------------------------------
// This is brcv/rec/dbasn/tests/dbasnh_test_simple_hypg.cxx
//:
// \file
// \brief Test grad. assign. matching of hypergraphs.
//        Load the simple graph from the data directory.
//        Create all possible corners from links and subsample such corners
//        to match.
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

#include <vector>
#include <sstream>
#include <iostream>
#include <ctime>
#include <fstream>
#include <vul/vul_printf.h>
#include <testlib/testlib_test.h>
#include <dbasn/dbasnh_gradasgn.h>
#include <dbasn/dbasnh_hypg_algos.h>


#define G_n 15              // # of nodes in G
#define G_link 30           // # of links in G
#define subg_n 10           // # if nodes in g
#define ITERATION 10          // # of test iterations
#define dNoise 0.1          // 0~1 noise add into the subgraph (0.3)

MAIN_ARGS( dbasnh_test_simple_hypg )
{
  std::string dir_base;

  if ( argc >= 2 ) {
      dir_base = argv[1];
      #ifdef VCL_WIN32
        dir_base += "\\";
      #else
        dir_base += "/";
      #endif
  }

  testlib_test_start("Grad. Asgn. on simple hypergraphs.");
/*
  dbasn_graph* G = new dbasn_graph ();
  dbasn_graph* g = new dbasn_graph ();
  
  std::string true_data_file = "GF1_bicr1p_GF2_bicr2p_ground_truth.txt";

  int G_nL = load_GA_graph_file (G, dir_base + "GraphFile1_bicr1p500x_NoSearchRadius.txt");
  TEST("Load first graph ", G->nN(), 20);
  int g_nL = load_GA_graph_file (g, dir_base + "GraphFile2_bicr2p500x_NoSearchRadius.txt");
  TEST("Load second graph ", g->nN(), 19);

  std::ifstream fin( (dir_base+true_data_file).c_str() );

  dbasn_gradasgn gradAssign;  
  dbasn_params params; //Use the default parameters.
  ///params.wN_ = 10, 2;
  ///params.wL_ = 2, 3;  
  params.wN_ = 1;
  params.wL_ = 1.5;
  std::cout<< params;
  gradAssign.setup_GA_params (G, g, params);

  std::cout <<"\n=================\n";
  std::cout <<"Graph G with nodes: "<< G->nN() << ", links: "<< G_nL << std::endl;
  std::cout <<"Graph g with nodes: "<< g->nN() << ", links: "<< g_nL << std::endl;

  gradAssign.get_assignment ();

  float *labelG = new float[G->nN()]; 
  for (int i = 0; i<G->nN(); i++) 
    labelG[i] = -1;

  //Read ground truth
  vul_printf (std::cout, "The ground truth result is:\n");
  float temp;
  for (int i = 0; i<G->nN(); i++) {
    for (int j = 0; j<g->nN()+1; j++) {
      fin >> temp;
      std::cout << temp << " ";
      if (temp > 0.0f) {
        labelG[i] = j;
      }
    }
    std::cout << std::endl;
  }

  std::cout << std::endl;
  for (int i = 0; i<G->nN(); i++) 
    std::cout << labelG[i] << " ";  

  int incorrect = 0;
  for (int i = 0; i<G->nN(); i++) {
    if (gradAssign.labelGg(i) > 0)
      if (labelG[i] != gradAssign.labelGg(i)) incorrect++;
  }
  std::cout<<"\n Result: "<< incorrect <<" incorrect matches out of " << G->nN() << "in the first graph\n";
  TEST("# of incorrect matches (should be 0) ", incorrect, 0);

  delete G;
  delete g;
  delete labelG;*/
  return testlib_test_summary();
}



