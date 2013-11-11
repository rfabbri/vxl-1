#include <testlib/testlib_test.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/algo/dbsk2d_extract_subgraph.h>
#include <dbskr/algo/dbskr_shock_patch_model_selector.h>
#include <dbskr/algo/dbskr_shock_patch_selector.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/algo/dbskr_shock_path_finder.h>
#include <dbsk2d/dbsk2d_file_io.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <vil/vil_load.h>
#include <dbgrl/algo/dbgrl_algs.h>

#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_compute_scurve.h>

static void test_graph_algos(int argc, char* argv[])
{
  testlib_test_start("testing graph algos ");
 
  dbsk2d_xshock_graph_fileio file_io;
  dbsk2d_shock_graph_sptr sg = file_io.load_xshock_graph("rectangle.esf");

  print_shock_graph(sg);

  TEST("load_xshock_graph() ", !sg, false);
  TEST("load_xshock_graph() ", sg->number_of_vertices(), 29);

  //: find nodes with ids 10 and 1
  int id1 = 81, id2 = 139;
  dbsk2d_shock_node_sptr& node1 = get_node_sptr(sg, id1);
  dbsk2d_shock_node_sptr& node2 = get_node_sptr(sg, id2);
  
  TEST("find node1 ", node1->id(), id1);
  TEST("find node2 ", node2->id(), id2);
/*
  //: find the min full depth of this shock graph
  dbsk2d_shock_node_sptr final_node; 
  int d = find_min_full_depth(sg, final_node);
  TEST("min full depth", d, 4);
  vcl_cout << "final_node: " << final_node->id() << vcl_endl;

  dbskr_v_graph_sptr v = construct_v_graph(sg, final_node, d);
  print_v_graph(v);

  dbskr_v_graph_sptr v2 = construct_v_graph(sg, final_node);
  print_v_graph(v2);
*/
  vcl_cout << "node1: " << node1->id() << " graph: " << vcl_endl;
  dbskr_v_graph_sptr v3 = construct_v_graph(sg, node1, 1);
  print_v_graph(v3);

  vcl_cout << "node2: " << node2->id() << " graph: " << vcl_endl;
  dbskr_v_graph_sptr v4 = construct_v_graph(sg, node2, 1);
  print_v_graph(v4);

  vcl_vector<dbsk2d_shock_edge_sptr> edges;
  //shortest_first_path(sg, node1, node2, edges); 

  //for (unsigned k = 0; k < edges.size(); k++)
  //  vcl_cout << "e id: " << edges[k]->id() << vcl_endl;

  dbskr_shock_path_finder f(sg);
  TEST("testing shock path finder", f.construct_v(), true);
  vcl_vector<dbskr_v_node_sptr> path;
  f.get_shortest_v_node_path(node1, node2, path);
  vcl_cout << "shortest v_node path from node1 to node2: \n";
  for (unsigned k = 0; k < path.size(); k++)
    vcl_cout << "v node id: " << path[k]->id_ << vcl_endl;
 
  f.get_edges_on_path(path, edges);
  for (unsigned k = 0; k < edges.size(); k++)
    vcl_cout << "e id: " << edges[k]->id() << vcl_endl;

  //: test getting the shock paths
  //dbskr_scurve_sptr sc = dbskr_compute_scurve(node1, edges, false, true, true, 0.5f, 1.0f);
  //vcl_cout << "sc arclength: " << sc->arclength(sc->num_points()-1) << vcl_endl;

  path.clear();
  f.get_shortest_v_node_path(node1, node1, path);
  TEST("test find path from node1 to node1", path.size(), 1);
  for (unsigned k = 0; k < path.size(); k++)
    vcl_cout << "v node id: " << path[k]->id_ << vcl_endl;

  vcl_vector<vcl_vector<dbskr_v_node_sptr> > paths;
  vcl_vector<float> abs_lengths;
  f.get_all_v_node_paths(node1, node2, paths, abs_lengths, 1.0f, 1000.0f);
  TEST_NEAR("test all v node paths", paths.size(), 4, 0.01);
  TEST_NEAR("test all v node paths lengths size", abs_lengths.size(), 4, 0.01);
  for (unsigned kk = 0; kk < paths.size(); kk++) {
    vcl_cout << "\t path: " << kk << " length: " << abs_lengths[kk] << " ";
    for (unsigned k = 0; k < paths[kk].size(); k++)
      vcl_cout << paths[kk][k]->id_ << " ";
    vcl_cout << vcl_endl << "\t\t";

    vcl_vector<dbsk2d_shock_edge_sptr> edges;
    f.get_edges_on_path(paths[kk], edges);
    for (unsigned k = 0; k < edges.size(); k++)
      vcl_cout << edges[k]->id() << " "; 
    vcl_cout << vcl_endl;
  }

  vcl_vector<dbskr_v_graph_sptr> graphs;
  f.get_all_v_graphs(node1, node2, v3, v4, graphs, abs_lengths, 1.0f, 1000.0f);
  vcl_cout << graphs.size() << " graphs between vg1 and vg2 of node1 and node2, lengths size: " << abs_lengths.size() << "\n";
  for (unsigned i = 0; i < graphs.size(); i++) {
    vcl_cout << "graph " << i << vcl_endl;
    print_v_graph(graphs[i]);
    vcl_cout << "--------\n";
  }
 
#if 1  
  dbsk2d_shock_graph_sptr sg2 = file_io.load_xshock_graph("dog1.esf");

  //print_shock_graph(sg2);

  TEST("load_xshock_graph() ", !sg2, false);
  TEST_NEAR("load_xshock_graph() ", sg2->number_of_vertices(), 117, 0.1);

  //: find nodes with ids 1935 and 2560
  //id1 = 2371; id2 = 1970;
  id1 = 2539; id2 = 2372;
  node1 = get_node_sptr(sg2, id1);
  node2 = get_node_sptr(sg2, id2);

  TEST("find n1 ", node1->id(), id1);
  TEST("find n2 ", node2->id(), id2);

  dbskr_shock_path_finder f2(sg2);
  TEST("testing shock path finder", f2.construct_v(), true);
  dbskr_v_graph_sptr v_sg2 = f2.get_v();
  //print_v_graph(v_sg2);

  dbskr_v_node_sptr& n1 = get_node_sptr(v_sg2, id1);
  dbskr_v_node_sptr& n2 = get_node_sptr(v_sg2, id2);

  TEST("find node1 ", !n1, false);
  TEST("find node2 ", !n2, false);
  if (n1) TEST("find node1 ", n1->id_, id1);
  if (n2) TEST("find node2 ", n2->id_, id2);
  //TEST("test v graph construction", v_sg2

  path.clear();
  TEST("testing shock path finder shortest path", f2.get_shortest_v_node_path(node1, node2, path), true);
  for (unsigned k = 0; k < path.size(); k++)
    vcl_cout << "v node id: " << path[k]->id_ << vcl_endl;
 
  edges.clear();
  TEST("testing shock path finder get edges", f2.get_edges_on_path(path, edges), true);
  
  for (unsigned k = 0; k < edges.size(); k++)
    vcl_cout << "e id: " << edges[k]->id() << vcl_endl;

  if (path.size() > 0 && edges.size() > 0) {
    //: test getting the shock paths
    dbskr_scurve_sptr sc = dbskr_compute_scurve(node1, edges, false, true, true, 0.5f, 1.0f);
    vcl_cout << "sc arclength: " << sc->arclength(sc->num_points()-1) << vcl_endl;
  }

  //vcl_vector<dbskr_v_graph_sptr> graphs;
  graphs.clear();
  dbskr_v_graph_sptr vg1 = construct_v_graph(sg2, node1, 1);
  print_v_graph(vg1);
  dbskr_v_graph_sptr vg2 = construct_v_graph(sg2, node2, 2);
  print_v_graph(vg2);
  vcl_vector<float> abs_lengths2;
  f2.get_all_v_graphs(node1, node2, vg1, vg2, graphs, abs_lengths2, 1.0f, 1000.0f);
  TEST("graphs size: ", graphs.size(), 5);

  unsigned k =0;
  for (unsigned k = 0; k < graphs.size(); k++) {
    dbskr_v_graph_sptr vg = graphs[k];  
    vcl_cout << "graph k: " << k << ": " << " length: " << abs_lengths2[k] << vcl_endl;
    print_v_graph(vg);
    vcl_cout << "-----------\n";
    vcl_cout.flush();
  
    dbskr_shock_patch_sptr p0 = extract_patch_from_v_graph(graphs[k], node1->id()*100000 + node2->id(), 0, 0.05);
    vcl_vector<dbskr_v_edge_sptr> es;
    get_edges_on_outer_face(graphs[k], es);
    vcl_cout << "edges on outer face of graph k: " << vcl_endl;
    for (unsigned i = 0; i < es.size(); i++) {
      vcl_cout << "i: " << i << " s: " << es[i]->source()->id_ << " t: " << es[i]->target()->id_ << vcl_endl;
    }
    vcl_cout << "-----------------\n";

    TEST("trace outer boundary", p0->trace_outer_boundary(), true);
    vcl_cout << "traced boundary length: " << p0->traced_boundary_length() << vcl_endl;
    p0->clear();
  }

  graphs.clear();
  abs_lengths2.clear();
  f2.get_all_v_graphs(node1, node2, vg1, vg2, graphs, abs_lengths2, 1.0f, 1000.0f);
  TEST("graphs size: ", graphs.size(), 5);

  graphs.clear();
  abs_lengths2.clear();
  f2.get_all_v_graphs(node1, node2, vg1, vg2, graphs, abs_lengths2, 1.0f, 300.0f);
  TEST_NEAR("graphs size: ", graphs.size(), 1, 0.01);


#endif
}

TESTMAIN_ARGS(test_graph_algos)
