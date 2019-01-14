#include <testlib/testlib_test.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/algo/dbsk2d_extract_subgraph.h>
#include <dbskr/pro/dbskr_shock_patch_model_selector.h>
#include <dbskr/pro/dbskr_shock_patch_selector.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/algo/dbskr_shock_path_finder.h>
#include <dbsk2d/dbsk2d_file_io.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <vil/vil_load.h>
#include <bgrld/algo/bgrld_algs.h>

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
  std::cout << "final_node: " << final_node->id() << std::endl;

  dbskr_v_graph_sptr v = construct_v_graph(sg, final_node, d);
  print_v_graph(v);

  dbskr_v_graph_sptr v2 = construct_v_graph(sg, final_node);
  print_v_graph(v2);
*/
  std::cout << "node1: " << node1->id() << " graph: " << std::endl;
  dbskr_v_graph_sptr v3 = construct_v_graph(sg, node1, 1);
  print_v_graph(v3);

  std::cout << "node2: " << node2->id() << " graph: " << std::endl;
  dbskr_v_graph_sptr v4 = construct_v_graph(sg, node2, 1);
  print_v_graph(v4);

  std::vector<dbsk2d_shock_edge_sptr> edges;
  //shortest_first_path(sg, node1, node2, edges); 

  //for (unsigned k = 0; k < edges.size(); k++)
  //  std::cout << "e id: " << edges[k]->id() << std::endl;

  dbskr_shock_path_finder f(sg);
  TEST("testing shock path finder", f.construct_v(), true);
  std::vector<dbskr_v_node_sptr> path;
  f.get_shortest_v_node_path(node1, node2, path);
  std::cout << "shortest v_node path from node1 to node2: \n";
  for (unsigned k = 0; k < path.size(); k++)
    std::cout << "v node id: " << path[k]->id_ << std::endl;
 
  f.get_edges_on_path(path, edges);
  for (unsigned k = 0; k < edges.size(); k++)
    std::cout << "e id: " << edges[k]->id() << std::endl;

  //: test getting the shock paths
  //dbskr_scurve_sptr sc = dbskr_compute_scurve(node1, edges, false, true, true, 0.5f, 1.0f);
  //std::cout << "sc arclength: " << sc->arclength(sc->num_points()-1) << std::endl;

  path.clear();
  f.get_shortest_v_node_path(node1, node1, path);
  TEST("test find path from node1 to node1", path.size(), 1);
  for (unsigned k = 0; k < path.size(); k++)
    std::cout << "v node id: " << path[k]->id_ << std::endl;

  std::vector<std::vector<dbskr_v_node_sptr> > paths;
  std::vector<float> abs_lengths;
  f.get_all_v_node_paths(node1, node2, paths, abs_lengths, 1.0f, 1000.0f);
  TEST_NEAR("test all v node paths", paths.size(), 4, 0.01);
  TEST_NEAR("test all v node paths lengths size", abs_lengths.size(), 4, 0.01);
  for (unsigned kk = 0; kk < paths.size(); kk++) {
    std::cout << "\t path: " << kk << " length: " << abs_lengths[kk] << " ";
    for (unsigned k = 0; k < paths[kk].size(); k++)
      std::cout << paths[kk][k]->id_ << " ";
    std::cout << std::endl << "\t\t";

    std::vector<dbsk2d_shock_edge_sptr> edges;
    f.get_edges_on_path(paths[kk], edges);
    for (unsigned k = 0; k < edges.size(); k++)
      std::cout << edges[k]->id() << " "; 
    std::cout << std::endl;
  }

  std::vector<dbskr_v_graph_sptr> graphs;
  f.get_all_v_graphs(node1, node2, v3, v4, graphs, abs_lengths, 1.0f, 1000.0f);
  std::cout << graphs.size() << " graphs between vg1 and vg2 of node1 and node2, lengths size: " << abs_lengths.size() << "\n";
  for (unsigned i = 0; i < graphs.size(); i++) {
    std::cout << "graph " << i << std::endl;
    print_v_graph(graphs[i]);
    std::cout << "--------\n";
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
    std::cout << "v node id: " << path[k]->id_ << std::endl;
 
  edges.clear();
  TEST("testing shock path finder get edges", f2.get_edges_on_path(path, edges), true);
  
  for (unsigned k = 0; k < edges.size(); k++)
    std::cout << "e id: " << edges[k]->id() << std::endl;

  if (path.size() > 0 && edges.size() > 0) {
    //: test getting the shock paths
    dbskr_scurve_sptr sc = dbskr_compute_scurve(node1, edges, false, true, true, 0.5f, 1.0f);
    std::cout << "sc arclength: " << sc->arclength(sc->num_points()-1) << std::endl;
  }

  //std::vector<dbskr_v_graph_sptr> graphs;
  graphs.clear();
  dbskr_v_graph_sptr vg1 = construct_v_graph(sg2, node1, 1);
  print_v_graph(vg1);
  dbskr_v_graph_sptr vg2 = construct_v_graph(sg2, node2, 2);
  print_v_graph(vg2);
  std::vector<float> abs_lengths2;
  f2.get_all_v_graphs(node1, node2, vg1, vg2, graphs, abs_lengths2, 1.0f, 1000.0f);
  TEST("graphs size: ", graphs.size(), 5);

  unsigned k =0;
  for (unsigned k = 0; k < graphs.size(); k++) {
    dbskr_v_graph_sptr vg = graphs[k];  
    std::cout << "graph k: " << k << ": " << " length: " << abs_lengths2[k] << std::endl;
    print_v_graph(vg);
    std::cout << "-----------\n";
    std::cout.flush();
  
    dbskr_shock_patch_sptr p0 = extract_patch_from_v_graph(graphs[k], node1->id()*100000 + node2->id(), 0, 0.05);
    std::vector<dbskr_v_edge_sptr> es;
    get_edges_on_outer_face(graphs[k], es);
    std::cout << "edges on outer face of graph k: " << std::endl;
    for (unsigned i = 0; i < es.size(); i++) {
      std::cout << "i: " << i << " s: " << es[i]->source()->id_ << " t: " << es[i]->target()->id_ << std::endl;
    }
    std::cout << "-----------------\n";

    TEST("trace outer boundary", p0->trace_outer_boundary(), true);
    std::cout << "traced boundary length: " << p0->traced_boundary_length() << std::endl;
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
