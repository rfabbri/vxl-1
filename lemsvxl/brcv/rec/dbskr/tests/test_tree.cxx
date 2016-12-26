#include <testlib/testlib_test.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <vcl_iostream.h>
//#include <vcl_cmath.h>

#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_compute_scurve.h>

#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

MAIN_ARGS(test_tree)
{

  vcl_string dir_base;

  if ( argc >= 2 ) {
    dir_base = argv[1];
#ifdef VCL_WIN32
    dir_base += "\\";
#else
    dir_base += "/";
#endif
  }

  testlib_test_start("testing tree class ");

  vcl_string data_file1 = "data/brk001.shg";
  vcl_string data_file2 = "data/pln006.shg";
  
  dbskr_tree_sptr tree1 = new dbskr_tree();
  TEST("test constructor ", tree1->size(), 0);

  // Test acquire tree from .shg file 
  TEST("acquire() brk001.shg ", tree1->acquire(dir_base+data_file1), true);
  TEST("acquire() brk001.shg size ", tree1->size(), 14);
  TEST("next() brk001.shg 13", tree1->next(13), 0);
  TEST("prev() brk001.shg 0", tree1->prev(0), 13);
  TEST("head() & tail() brk001.shg ", tree1->head(0), tree1->tail(tree1->mate(0)));
  TEST("head() & tail() brk001.shg ", tree1->head(9), tree1->tail(tree1->mate(9)));
  vcl_vector<int> test;
  test.push_back(11);
  test.push_back(1);
  vcl_vector<int> &test_ch0 = tree1->children(0);
  vcl_vector<int> test_ch02 = tree1->children(0);
  TEST("children brk001.shg 0 ", tree1->children(0), test);
  TEST("children brk001.shg 0 ", test_ch0, test);
  TEST("children brk001.shg 0 ", test_ch02, test);
  vcl_cout << "size of ch: " << test_ch0.size() << vcl_endl;
  for (unsigned int i = 0; i<test_ch0.size(); i++)
    vcl_cout << "ch[" << i << "]: " << test_ch0.at(i) << " ";
  vcl_cout << vcl_endl;

  test.clear();
  test.push_back(9);
  test.push_back(7);
  vcl_vector<int> test_ch6(tree1->children(6));
  TEST("children() brk001.shg 6 ", tree1->children(6), test);
  TEST("children() brk001.shg 6 ", test_ch6, test);
  TEST_NEAR("subtree_delete_cost() brk001.shg 0 ", tree1->subtree_delete_cost(0), 222.77, 0.001);
  TEST_NEAR("subtree_delete_cost() brk001.shg 7 ", tree1->subtree_delete_cost(7), 0.91, 0.001);
  test.clear();
  test.push_back(1);
  test.push_back(13);
  test.push_back(11);
  TEST("out_darts() brk001.shg 0 ", tree1->out_darts(0), test);
  test.clear();
  test.push_back(6);
  TEST("out_darts() brk001.shg 5 ", tree1->out_darts(5), test);
  TEST("parent_dart() brk001.shg 8 ", tree1->parent_dart(8), 4);
  TEST("parent_dart_ccw() brk001.shg 5 ", tree1->parent_dart_ccw(5), 9);
  TEST("skip_parent_dart_ccw() brk001.shg 2 ", tree1->skip_parent_dart_ccw(2), 9);
  int root1 = tree1->centroid();
  TEST("centroid() brk001.shg ", root1, 0);
  //: test the set_up method using dart 0
  test.clear();
  test.push_back(0);
  tree1->set_up(test);
  TEST("set_up() brk001.shg 0 ", tree1->up(0), false);
  TEST("set_up() brk001.shg ", tree1->up(tree1->mate(0)), true);
  TEST("set_up() brk001.shg 4 ", tree1->up(4), false);
  TEST("set_up() brk001.shg ", tree1->up(tree1->mate(4)), true);
  TEST("set_up() brk001.shg 11 ", tree1->up(11), false);
  TEST("set_up() brk001.shg ", tree1->up(tree1->mate(11)), true);
  test.clear();
  test.push_back(8);
  test.push_back(9);
  test.push_back(12);
  test.push_back(0);
  test.push_back(1);
  test.push_back(3);
  test.push_back(4);
  test.push_back(6);
  test.push_back(10);
  //vcl_vector<int> tmp = tree1->order_subproblems();
  TEST("order_subproblems() brk001.shg ", tree1->order_subproblems(), test);

  test.clear();
  test.push_back(3);
  test.push_back(0);
  test.push_back(0);
  test.push_back(1);
  test.push_back(1);
  test.push_back(2);
  TEST("find_node_path() brk001.shg ", tree1->find_node_path(0,4), test);
  test.clear();
  test.push_back(3);
  test.push_back(0);
  test.push_back(0);
  test.push_back(7);
  TEST("find_node_path() brk001.shg ", tree1->find_node_path(0,11), test);
  test.clear();
  test.push_back(1);
  test.push_back(2);
  TEST("find_node_path() brk001.shg ", tree1->find_node_path(4,4), test);

  dbskr_tree_sptr tree2 = new dbskr_tree();
  TEST("acquire pln006.shg ", tree2->acquire(dir_base+data_file2), true);
  TEST("acquire pln006.shg size ", tree2->size(), 42);
  TEST("next pln006.shg 41", tree2->next(41), 0);
  TEST("prev pln006.shg 0", tree2->prev(0), 41);
  TEST("head() & tail() pln006.shg ", tree2->head(25), tree2->tail(tree2->mate(25)));
  TEST("head() & tail() pln006.shg ", tree2->head(41), tree2->tail(tree2->mate(41)));
  test.clear();
  TEST("children pln006.shg 21 ", tree2->children(21), test);
  test.clear();
  test.push_back(37);
  test.push_back(35);
  TEST("children pln006.shg 34 ", tree2->children(34), test);
  TEST_NEAR("subtree_delete_cost() pln006.shg 8 ", tree2->subtree_delete_cost(8), 499.84, 0.001);
  TEST_NEAR("subtree_delete_cost() pln006.shg 16 ", tree2->subtree_delete_cost(16), 163.8, 0.001);
  test.clear();
  test.push_back(19);
  test.push_back(23);
  test.push_back(21);
  TEST("out_darts() pln006.shg 13 ", tree2->out_darts(13), test);
  test.clear();
  test.push_back(32);
  TEST("out_darts() pln006.shg 18 ", tree2->out_darts(18), test);
  TEST("parent_dart() pln006.shg 22 ", tree2->parent_dart(22), 18);
  TEST("parent_dart_ccw() pln006.shg 35 ", tree2->parent_dart_ccw(35), 5);
  TEST("skip_parent_dart_ccw() pln006.shg 18 ", tree2->skip_parent_dart_ccw(18), 29);
  int root2 = tree2->centroid();
  TEST("centroid() pln006.shg ", root2, 1);
  //: test the set_up method using out darts of root2
  tree2->set_up(tree2->out_darts(root2));
  TEST("set_up() pln006.shg 16 ", tree2->up(16), false);
  TEST("set_up() pln006.shg ", tree2->up(tree2->mate(16)), true);
  TEST("set_up() pln006.shg 9 ", tree2->up(9), false);
  TEST("set_up() pln006.shg ", tree2->up(tree2->mate(9)), true);
  TEST("set_up() pln006.shg 1 ", tree2->up(1), false);
  TEST("set_up() pln006.shg ", tree2->up(tree2->mate(1)), true);
  
  test.clear();
  test.push_back(1);
  test.push_back(3);
  test.push_back(3);
  test.push_back(4);
  test.push_back(4);
  test.push_back(10);
  test.push_back(10);
  test.push_back(21);
  TEST("find_node_path() pln006.shg ", tree2->find_node_path(16,27), test);
  test.clear();
  test.push_back(1);
  test.push_back(6);
  test.push_back(6);
  test.push_back(7);
  test.push_back(7);
  test.push_back(12);
  TEST("find_node_path() pln006.shg ", tree2->find_node_path(6,12), test);
  
  test.clear();
  test.push_back(4);
  test.push_back(22);
  test.push_back(29);
  test.push_back(32);
  test.push_back(15);
  vcl_vector<int> tmp = tree2->find_special_darts(tree2->out_darts(root2));
  TEST("find_special_darts() pln006.shg ", tmp, test);
#if 0
  for (int i = 0; i<tmp.size(); i++)
    vcl_cout << "tmp[" << i << "]: " << tmp[i] << " ";
  vcl_cout << vcl_endl;
#endif

 
  //: testing new version of tree class which includes a smart pointer to a dbsk2d_shock_graph instance
  vcl_string data_file3 = "data/brk001.esf";
  vcl_string data_file4 = "data/pln006.esf";
  
  dbskr_tree_sptr tree3 = new dbskr_tree();
  dbsk2d_xshock_graph_fileio loader;
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(dir_base+data_file3);
  TEST("shock graph load() vertices ", sg->number_of_vertices(), 21);
  vcl_cout << " number of vertices: " << sg->number_of_vertices() << vcl_endl;
  TEST("shock graph load() edges ", sg->number_of_edges(), 20);
  vcl_cout << " number of edges: " << sg->number_of_edges() << vcl_endl;
  tree3->acquire(sg, true, true, true);

  // Test acquire tree from .esf file 
  TEST("acquire() brk001.esf size ", tree3->size(), 14);
  TEST("next() brk001.esf 13", tree3->next(13), 0);
  TEST("prev() brk001.esf 0", tree3->prev(0), 13);
  TEST("head() & tail() brk001.esf ", tree3->head(0), tree3->tail(tree3->mate(0)));
  TEST("head() & tail() brk001.esf ", tree3->head(9), tree3->tail(tree3->mate(9)));
  test.clear();
  test.push_back(11);
  test.push_back(1);
  test_ch0 = tree3->children(0);
  test_ch02 = tree3->children(0);
  TEST("children brk001.esf 0 ", tree3->children(0), test);
  TEST("children brk001.esf 0 ", test_ch0, test);
  TEST("children brk001.esf 0 ", test_ch02, test);

  test.clear();
  test.push_back(9);
  test.push_back(7);
  test_ch6 = (tree3->children(6));
  TEST("children() brk001.esf 6 ", tree3->children(6), test);
  TEST("children() brk001.esf 6 ", test_ch6, test);
  //TEST_NEAR("subtree_delete_cost() brk001.esf 0 ", tree3->subtree_delete_cost(0), 222.77, 0.001);
  //TEST_NEAR("subtree_delete_cost() brk001.esf 7 ", tree3->subtree_delete_cost(7), 0.91, 0.001);
  test.clear();
  test.push_back(1);
  test.push_back(13);
  test.push_back(11);
  TEST("out_darts() brk001.esf 0 ", tree3->out_darts(0), test);
  test.clear();
  test.push_back(6);
  vcl_vector<int> &tmpv = tree3->out_darts(5);
  vcl_cout << "printing tmpv out darts of node 5\n";
  for (unsigned int i = 0; i<tmpv.size(); i++)
    vcl_cout << tmpv[i] << "\n";
  TEST("out_darts() brk001.esf 5 ", tree3->out_darts(5), test);

  TEST("parent_dart() brk001.esf 8 ", tree3->parent_dart(8), 4);
  TEST("parent_dart_ccw() brk001.esf 5 ", tree3->parent_dart_ccw(5), 9);
  TEST("skip_parent_dart_ccw() brk001.esf 2 ", tree3->skip_parent_dart_ccw(2), 9);
  root1 = tree3->centroid();
  TEST("centroid() brk001.shg ", root1, 0);
  //: test the set_up method using dart 0
  test.clear();
  test.push_back(0);
  tree3->set_up(test);
  TEST("set_up() brk001.esf 0 ", tree3->up(0), false);
  TEST("set_up() brk001.esf ", tree3->up(tree3->mate(0)), true);
  TEST("set_up() brk001.esf 4 ", tree3->up(4), false);
  TEST("set_up() brk001.esf ", tree3->up(tree3->mate(4)), true);
  TEST("set_up() brk001.esf 11 ", tree3->up(11), false);
  TEST("set_up() brk001.esf ", tree3->up(tree3->mate(11)), true);
  test.clear();
  test.push_back(8);
  test.push_back(9);
  test.push_back(12);
  test.push_back(0);
  test.push_back(1);
  test.push_back(3);
  test.push_back(4);
  test.push_back(6);
  test.push_back(10);
  //vcl_vector<int> tmp = tree1->order_subproblems();
  TEST("order_subproblems() brk001.esf ", tree3->order_subproblems(), test);

  test.clear();
  test.push_back(3);
  test.push_back(0);
  test.push_back(0);
  test.push_back(1);
  test.push_back(1);
  test.push_back(2);
  TEST("find_node_path() brk001.esf ", tree3->find_node_path(0,4), test);
  test.clear();
  test.push_back(3);
  test.push_back(0);
  test.push_back(0);
  test.push_back(7);
  TEST("find_node_path() brk001.esf ", tree3->find_node_path(0,11), test);
  test.clear();
  test.push_back(1);
  test.push_back(2);
  TEST("find_node_path() brk001.esf ", tree3->find_node_path(4,4), test);

  test.clear();
  test.push_back(1);
  TEST("get_dart_path() brk001.esf ", tree3->get_dart_path(1,1), test);

  dbsk2d_shock_node_sptr start_node;
  vcl_vector<dbsk2d_shock_edge_sptr> path;
  tree3->edge_list(test, start_node, path);
  TEST("edge_list() path size brk001.esf ", path.size(), 14);
  TEST("edge_list() start node id brk001.esf ", start_node->id(), 1203);

  dbskr_scurve_sptr sc = dbskr_compute_scurve(start_node, path);
  TEST("dbskr_compute_scurve() ", !sc, false);
  //TEST("

  test.clear();
  test.push_back(10);
  TEST("get_dart_path() brk001.esf ", tree3->get_dart_path(10,10), test);

  test.clear();
  test.push_back(1);
  test.push_back(4);
  TEST("get_dart_path() brk001.esf ", tree3->get_dart_path(1,4), test);

  test.clear();
  test.push_back(8);
  test.push_back(9);
  test.push_back(10);
  test.push_back(13);
  TEST("get_dart_path() brk001.esf ", tree3->get_dart_path(8,13), test);

  tree3->edge_list(test, start_node, path);
  TEST("edge_list() path size brk001.esf ", path.size(), 17);
  TEST("edge_list() start node id brk001.esf ", start_node->id(), 868);

  // testing pln006.esf

  dbskr_tree_sptr tree4 = new dbskr_tree();

  dbsk2d_shock_graph_sptr sg2 = loader.load_xshock_graph(dir_base+data_file4);
  TEST("shock graph load() pln006.esf vertices ", sg2->number_of_vertices(), 32);
  vcl_cout << " number of vertices: " << sg2->number_of_vertices() << vcl_endl;
  TEST("shock graph load() pln006.esf edges ", sg2->number_of_edges(), 31);
  vcl_cout << " number of edges: " << sg2->number_of_edges() << vcl_endl;
  tree4->acquire(sg2, true, true, true);

  TEST("acquire pln006.esf size ", tree4->size(), 42);
  TEST("next pln006.esf 41", tree4->next(41), 0);
  TEST("prev pln006.esf 0", tree4->prev(0), 41);
  TEST("head() & tail() pln006.esf ", tree4->head(25), tree4->tail(tree4->mate(25)));
  TEST("head() & tail() pln006.esf ", tree4->head(41), tree4->tail(tree4->mate(41)));
  test.clear();
  TEST("children pln006.esf 21 ", tree4->children(21), test);
  test.clear();
  test.push_back(37);
  test.push_back(35);
  TEST("children pln006.esf 34 ", tree4->children(34), test);
  TEST_NEAR("subtree_delete_cost() pln006.esf 8 ", tree4->subtree_delete_cost(8), 499.84, 1.0);
  TEST_NEAR("subtree_delete_cost() pln006.esf 16 ", tree4->subtree_delete_cost(16), 163.8, 1.0);
  test.clear();
  test.push_back(19);
  test.push_back(23);
  test.push_back(21);
  TEST("out_darts() pln006.esf 13 ", tree4->out_darts(13), test);
  test.clear();
  test.push_back(32);
  TEST("out_darts() pln006.esf 18 ", tree4->out_darts(18), test);
  TEST("parent_dart() pln006.esf 22 ", tree4->parent_dart(22), 18);
  TEST("parent_dart_ccw() pln006.esf 35 ", tree4->parent_dart_ccw(35), 5);
  TEST("skip_parent_dart_ccw() pln006.esf 18 ", tree4->skip_parent_dart_ccw(18), 29);
  root2 = tree4->centroid();
  TEST("centroid() pln006.esf ", root2, 1);
  //: test the set_up method using out darts of root2
  tree4->set_up(tree4->out_darts(root2));
  TEST("set_up() pln006.esf 16 ", tree4->up(16), false);
  TEST("set_up() pln006.esf ", tree4->up(tree4->mate(16)), true);
  TEST("set_up() pln006.esf 9 ", tree4->up(9), false);
  TEST("set_up() pln006.esf ", tree4->up(tree4->mate(9)), true);
  TEST("set_up() pln006.esf 1 ", tree4->up(1), false);
  TEST("set_up() pln006.esf ", tree4->up(tree4->mate(1)), true);
  
  test.clear();
  test.push_back(1);
  test.push_back(3);
  test.push_back(3);
  test.push_back(4);
  test.push_back(4);
  test.push_back(10);
  test.push_back(10);
  test.push_back(21);
  TEST("find_node_path() pln006.esf ", tree4->find_node_path(16,27), test);
  test.clear();
  test.push_back(1);
  test.push_back(6);
  test.push_back(6);
  test.push_back(7);
  test.push_back(7);
  test.push_back(12);
  TEST("find_node_path() pln006.esf ", tree4->find_node_path(6,12), test);

  test.clear();
  test.push_back(5);
  test.push_back(16);
  test.push_back(17);
  test.push_back(18);
  TEST("get_dart_path_from_nodes() pln006.esf ", tree4->get_dart_path_from_nodes(0,13), test);

  test.clear();
  test.push_back(11);
  test.push_back(14);
  test.push_back(15);
  test.push_back(16);
  test.push_back(17);
  test.push_back(24);
  TEST("get_dart_path_from_nodes() pln006.esf ", tree4->get_dart_path_from_nodes(9,10), test);

  test.clear();
  test.push_back(37);
  TEST("get_dart_path_from_nodes() pln006.esf ", tree4->get_dart_path_from_nodes(0,2), test);

  
  test.clear();
  test.push_back(4);
  test.push_back(22);
  test.push_back(29);
  test.push_back(32);
  test.push_back(15);
  vcl_vector<int> tmp2 = tree4->find_special_darts(tree4->out_darts(root2));
  TEST("find_special_darts() pln006.esf ", tmp2, test);


  return testlib_test_summary();
}
