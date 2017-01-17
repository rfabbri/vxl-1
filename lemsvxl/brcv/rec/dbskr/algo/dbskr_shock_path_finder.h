// This is brcv/rec/dbskr/algo/dbskr_shock_path_finder.h
#ifndef dbskr_shock_path_finder_h_
#define dbskr_shock_path_finder_h_

//-----------------------------------------------------------------------------
//:
// \file
// \brief A class that finds paths between any given two nodes in a shock graph
//         
//   a-b-c-g
//     | |
//     d-e-f
//
//   for the shock above the path tree from a is as follows
//           a
//   0       b
//   1   c      d
//   2  g e     e
//   3   d f   c f
//   4         g
//
//    so for instance there are two paths from a to g as g appears twice
// \author Ozge C. Ozcanli March 29, 07
//
// \verbatim
//  Modifications
//
//
// \endverbatim 
//-----------------------------------------------------------------------------

#include <vbl/vbl_ref_count.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_string.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbskr/dbskr_v_graph_sptr.h>
#include <dbskr/algo/dbskr_shock_path_finder_sptr.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <dbskr/dbskr_scurve_sptr.h>

//: a helper class to build path tree of a given node
class dbskr_path_tree_node : public vbl_ref_count
{
public:
  dbskr_v_node_sptr node_;
  dbskr_path_tree_node_sptr parent_;
  float length_;

  dbskr_path_tree_node(dbskr_v_node_sptr n) : node_(n), parent_(0) {};
  dbskr_path_tree_node(dbskr_v_node_sptr n, dbskr_path_tree_node_sptr p, float len) : node_(n), parent_(p), length_(len) {}; 
  ~dbskr_path_tree_node() { node_ = 0; parent_ = 0; }
};

class dbskr_shock_path_finder : public vbl_ref_count
{
public:

  //: constructor
  dbskr_shock_path_finder(dbsk2d_shock_graph_sptr sg) : sg_(sg) {}

  ~dbskr_shock_path_finder() { clear(); }

  void clear();

  bool construct_v();
  bool get_shortest_v_node_path(dbsk2d_shock_node_sptr n1, 
                                dbsk2d_shock_node_sptr n2, 
                                vcl_vector<dbskr_v_node_sptr>& path);

  //: return all the paths whose normalized length is within length_thres
  //  restrict the search with this length constraint
  //  required because number of paths explode in complicated shock graphs
  bool get_all_v_node_paths(dbsk2d_shock_node_sptr n1, 
                              dbsk2d_shock_node_sptr n2, 
                              vcl_vector<vcl_vector<dbskr_v_node_sptr> >& paths, vcl_vector<float>& abs_lengths, float normalization_length, float length_thres);

  bool get_all_scurves(dbsk2d_shock_node_sptr n1, 
                       dbsk2d_shock_node_sptr n2, 
                       vcl_vector<dbskr_scurve_sptr>& scurves, 
                       vcl_vector<float>& abs_lengths, float norm_length, float length_threshold, float interpolate_ds, float sample_ds);


  //: the input v graphs and all the paths between n1 and n2 induce various v graphs 
  //  as the portion of the shape that connects n1 and n2 which may or may not be the root of v1 and v2
  //  find all such v graphs using the paths and various unions of paths
  bool get_all_v_graphs(dbsk2d_shock_node_sptr n1, 
                              dbsk2d_shock_node_sptr n2, dbskr_v_graph_sptr v1, dbskr_v_graph_sptr v2,
                              vcl_vector<dbskr_v_graph_sptr>& graphs, vcl_vector<float>& abs_lengths, float norm_length, float length_threshold);

  bool get_edges_on_path(vcl_vector<dbskr_v_node_sptr>& path, 
                         vcl_vector<dbsk2d_shock_edge_sptr>& edges);

  dbskr_v_graph_sptr get_v() { return v_; }
  dbsk2d_shock_graph_sptr get_sg() { return sg_; }

protected:
  bool get_path(vcl_vector<vcl_vector<dbskr_path_tree_node_sptr>* >* vec, int ind1, int ind2, vcl_vector<dbskr_v_node_sptr>& path, float& length, int n1_id, int n2_id);
  vcl_vector<dbskr_path_tree_node_sptr>* initialize_depth_zero(dbskr_v_node_sptr v1);
  vcl_vector<dbskr_path_tree_node_sptr>* advance(vcl_vector<dbskr_path_tree_node_sptr>* last_vec, dbskr_v_node_sptr v1, float norm, float threshold);

  dbsk2d_shock_graph_sptr sg_;
  dbskr_v_graph_sptr v_;

  //: a map from each node on the v_graph to its path_tree
  //  a path_tree is a vector indexed by depth, each depth contains a vector of tree_node's
  //  a tree_node contains the v_node of that depth and a pointer to its parent tree_node
  vcl_map<int, vcl_vector<vcl_vector<dbskr_path_tree_node_sptr>* >*> path_tree_map_; 
  typedef vcl_map<int, vcl_vector<vcl_vector<dbskr_path_tree_node_sptr>* >*> path_tree_map_type;
  
  //: a map to keep track of path_tree_node's to prevent infinite loops while finding all possible paths
  vcl_multimap<vcl_pair<int, int>, dbskr_path_tree_node_sptr> path_tree_multimap_;
};


#endif // dbskr_shock_path_finder_h_

