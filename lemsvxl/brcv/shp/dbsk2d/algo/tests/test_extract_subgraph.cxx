#include <testlib/testlib_test.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/algo/dbsk2d_extract_subgraph.h>

static void test_extract_subgraph(int argc, char* argv[])
{
  testlib_test_start("testing extract_subgraph ");
 
  dbsk2d_xshock_graph_fileio file_io;
  dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph("bonefishes.esf");

  TEST("load_xshock_graph() ", !sg, false);
  TEST("load_xshock_graph() ", sg->number_of_vertices(), 14);

  int depth = 1;
  for (dbsk2d_shock_graph::vertex_iterator v_itr = sg->vertices_begin(); v_itr != sg->vertices_end(); v_itr++)
  { 
    if ((*v_itr)->degree() < 3)
      continue;
    
    dbsk2d_shock_graph_sptr sub_sg = new dbsk2d_shock_graph();    
    dbsk2d_extract_subgraph(sub_sg, 0, *v_itr, depth);

    TEST("dbsk2d_extract_subgraph() ", !sub_sg, false);
    vcl_cout << "n: " << sub_sg->number_of_vertices() << vcl_endl;
    //TEST("dbsk2d_extract_subgraph() ", sub_sg->number_of_vertices(), 4);
  }

  depth = 2;
  for (dbsk2d_shock_graph::vertex_iterator v_itr = sg->vertices_begin(); v_itr != sg->vertices_end(); v_itr++)
  { 
    if ((*v_itr)->degree() < 3)
      continue;
    
    dbsk2d_shock_graph_sptr sub_sg = new dbsk2d_shock_graph();    
    dbsk2d_extract_subgraph(sub_sg, 0, *v_itr, depth);

    TEST("dbsk2d_extract_subgraph() ", !sub_sg, false);
    vcl_cout << "n: " << sub_sg->number_of_vertices() << vcl_endl;
    //TEST("dbsk2d_extract_subgraph() ", sub_sg->number_of_vertices(), 4);
  }

  depth = 3;
  for (dbsk2d_shock_graph::vertex_iterator v_itr = sg->vertices_begin(); v_itr != sg->vertices_end(); v_itr++)
  { 
    if ((*v_itr)->degree() < 3)
      continue;
    
    dbsk2d_shock_graph_sptr sub_sg = new dbsk2d_shock_graph();    
    dbsk2d_extract_subgraph(sub_sg, 0, *v_itr, depth);

    TEST("dbsk2d_extract_subgraph() ", !sub_sg, false);
    vcl_cout << "n: " << sub_sg->number_of_vertices() << vcl_endl;
    //TEST("dbsk2d_extract_subgraph() ", sub_sg->number_of_vertices(), 4);
  }

  depth = 4;
  for (dbsk2d_shock_graph::vertex_iterator v_itr = sg->vertices_begin(); v_itr != sg->vertices_end(); v_itr++)
  { 
    if ((*v_itr)->degree() < 3)
      continue;
    
    dbsk2d_shock_graph_sptr sub_sg = new dbsk2d_shock_graph();    
    dbsk2d_extract_subgraph(sub_sg, 0, *v_itr, depth);

    TEST("dbsk2d_extract_subgraph() ", !sub_sg, false);
    vcl_cout << "n: " << sub_sg->number_of_vertices() << vcl_endl;
    //TEST("dbsk2d_extract_subgraph() ", sub_sg->number_of_vertices(), 4);
  }

  depth = 5;
  for (dbsk2d_shock_graph::vertex_iterator v_itr = sg->vertices_begin(); v_itr != sg->vertices_end(); v_itr++)
  { 
    if ((*v_itr)->degree() < 3)
      continue;
    
    dbsk2d_shock_graph_sptr sub_sg = new dbsk2d_shock_graph();    
    dbsk2d_extract_subgraph(sub_sg, 0, *v_itr, depth);

    TEST("dbsk2d_extract_subgraph() ", !sub_sg, false);
    vcl_cout << "n: " << sub_sg->number_of_vertices() << vcl_endl;
    //TEST("dbsk2d_extract_subgraph() ", sub_sg->number_of_vertices(), 4);
  }


  //TEST_NEAR("parse() bone 10", bd->get_box_vector("chair")[0]->get_min_x(), 263.2, 0.001);
  //TEST_NEAR("parse() bone 11", bd->get_box_vector("chair")[0]->get_min_y(), 211.345, 0.001);
  
}

TESTMAIN_ARGS(test_extract_subgraph)
