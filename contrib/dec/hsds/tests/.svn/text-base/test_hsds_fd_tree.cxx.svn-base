#include <testlib/testlib_test.h>
#include <vbl/vbl_bounding_box.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>

#include "../hsds_fd_tree.h"
//#include "../hsds_fd_tree_io.h"

static void test_hsds_fd_tree()
{
  START("hsds_fd_tree test");
  {
  // create a quadtree 
  vbl_bounding_box<double,2> bbox2;
  bbox2.update(0.0 ,0.0);
  bbox2.update(100.0, 100.0);

  hsds_fd_tree<double,2> quadtree(bbox2, 1, 0.5);
  // make sure tree has proper number of nodes
  TEST_EQUAL("quadtree has correct number of nodes after initialization",quadtree.size(),4);
  hsds_fd_tree<double,2>::iterator it = quadtree.begin();
  for (; it != quadtree.end(); ++it) {
    vcl_cout << (*it).first << "  data = " << (*it).second << vcl_endl;
  }
  // get list of neighbors above first node
  hsds_fd_tree_node_index<2> test_node = quadtree.index_at(vnl_vector_fixed<double,2>(10.0, 10.0));
  vcl_vector<hsds_fd_tree_node_index<2> > neighbors;
  quadtree.neighbor_cells(test_node,1,true,neighbors);
  TEST_EQUAL("one neighbor returned before split",neighbors.size(),1);
  vcl_cout << "neighbor = " << neighbors[0] << vcl_endl;

  hsds_fd_tree_node_index<2> split_node = quadtree.index_at(vnl_vector_fixed<double,2>(10.0,60.0));
  quadtree.split(split_node, 0.25);
  vcl_cout << vcl_endl << "after split: " << vcl_endl;
  for (it = quadtree.begin(); it != quadtree.end(); ++it) {
    vcl_cout << (*it).first << "  data = " << (*it).second << vcl_endl;
  }
  
  TEST_EQUAL("quadtree has correct number of nodes after split",quadtree.size(),7);

  quadtree.neighbor_cells(test_node,1,true,neighbors);
  TEST_EQUAL("two neighbors returned after split",neighbors.size(),2);
  vcl_cout << "neighbors[0] = " << neighbors[0] << vcl_endl;
  vcl_cout << "neighbors[1] = " << neighbors[1] << vcl_endl;

  // merge nodes back to level 1
  hsds_fd_tree_node_index<2> merged_node = quadtree.index_at(vnl_vector_fixed<double,2>(10.0,60.0));
  merged_node = merged_node.parent_index();
  vcl_cout << "merging index " << merged_node << vcl_endl;
  quadtree.merge(merged_node,0.9);
  vcl_cout << vcl_endl << "after merge: " << vcl_endl;
  for (it = quadtree.begin(); it != quadtree.end(); ++it) {
    vcl_cout << (*it).first << "  data = " << (*it).second << vcl_endl;
  }

  TEST_EQUAL("quadtree has correct number of nodes after split and merge",quadtree.size(),4);
  }
  {
  // create an octree 
  vbl_bounding_box<double,3> bbox3;
  bbox3.update(0.0 ,0.0, 0.0);
  bbox3.update(100.0, 100.0, 100.0);

  hsds_fd_tree<double,3> octree(bbox3, 1, 0.5);
  // make sure tree has proper number of nodes
  TEST_EQUAL("octree has correct number of nodes after initialization",octree.size(),8);
  hsds_fd_tree<double,3>::iterator it = octree.begin();
  for (; it != octree.end(); ++it) {
    vcl_cout << (*it).first << "  data = " << (*it).second << vcl_endl;
  }
  hsds_fd_tree_node_index<3> split_node = octree.index_at(vnl_vector_fixed<double,3>(10.0,60.0,84.0));
  octree.split(split_node, 0.25);
  vcl_cout << vcl_endl << "after split: " << vcl_endl;
  for (it = octree.begin(); it != octree.end(); ++it) {
    vcl_cout << (*it).first << "  data = " << (*it).second << vcl_endl;
  }

  TEST_EQUAL("octree has correct number of nodes after split",octree.size(),15);

  // merge nodes back to level 1
  hsds_fd_tree_node_index<3> merged_node = octree.index_at(vnl_vector_fixed<double,3>(10.0,60.0,84.0));
  merged_node = merged_node.parent_index();
  vcl_cout << "merging index " << merged_node << vcl_endl;
  octree.merge(merged_node,0.9);
  vcl_cout << vcl_endl << "after merge: " << vcl_endl;
  for (it = octree.begin(); it != octree.end(); ++it) {
    vcl_cout << (*it).first << "  data = " << (*it).second << vcl_endl;
  }

  TEST_EQUAL("octree has correct number of nodes after split and merge",octree.size(),8);

  
  }

  return;
}



TESTMAIN( test_hsds_fd_tree );
