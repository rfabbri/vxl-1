// This is file shp/dbsksp/algo/dbsksp_avg_xgraph_cost_function_utils.cxx

//:
// \file

#include "dbsksp_avg_xgraph_utils.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>



//------------------------------------------------------------------------------
//: Compute max-curvature cost - penalize boundary curvature higher than max
// curvature dictated by radius at the node
double dbsksp_compute_kmax_cost(const dbsksp_xshock_fragment& xfrag)
{
  dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
  dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

  if (left_bnd.flag() == -1 || right_bnd.flag() == -1)
    return 1e6; //vnl_numeric_traits<double >::maxval;

  // penalize for "long" boundary
  double ds_left  = left_bnd.len()  - vnl_math::pi * vgl_distance(left_bnd.start(), left_bnd.end());
  double ds_right = right_bnd.len() - vnl_math::pi * vgl_distance(right_bnd.start(), right_bnd.end());

  if (ds_left > 0 || ds_right >0)
    return 1e6; //vnl_numeric_traits<double >::maxval;

  double left_k[2];
  left_k [0] = left_bnd.k1();
  left_k [1] = (left_bnd.flag() == 0) ? left_bnd.k1() : left_bnd.k2();

  double right_k[2];
  right_k [0] = right_bnd.k1();
  right_k [1] = (right_bnd.flag() == 0) ? right_bnd.k1() : right_bnd.k2();

  double r[2];
  r[0] = xfrag.start().radius();
  r[1] = xfrag.end().radius();

  double kmax_cost = 0;
  // left side: min (negative) curvature allowed is (-1/r)
  kmax_cost       += (-left_k[0] > (1/r[0])) ? 
                     (vcl_exp(r[0] + 1/left_k[0]) -1) : 0;

  kmax_cost       += (-left_k[1] > (1/r[1])) ? 
                     (vcl_exp(r[1] + 1/left_k[1]) -1) : 0;


  // right side: max (positive) curvature allowed is (1/r)
  kmax_cost       += (right_k[0] > (1/r[0])) ? 
                     (vcl_exp(r[0] - 1/right_k[0]) -1) : 0;

  kmax_cost       += (right_k[1] > (1/r[1])) ? 
                     (vcl_exp(r[1] - 1/right_k[1]) -1) : 0;

  return kmax_cost;
}





//------------------------------------------------------------------------------
//: Return true if parameter values of xgraph at active nodes and active edges
// are within allowed range
bool dbsksp_is_valid_xgraph(const dbsksp_xshock_graph_sptr& xgraph,
                            const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
                            const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges)
{
  //a) check the nodes  
  for (unsigned i =0; i < active_nodes.size(); ++i)
  {
    bool is_valid = true;

    dbsksp_xshock_node_sptr xv = active_nodes[i];
    is_valid = is_valid && (xv->radius() >= 0);

    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != xv->edges_end(); ++eit)
    {
      dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(*eit);
      is_valid = is_valid && (xdesc->phi() >= 0);
    }

    if (!is_valid)
      return false;
  }

  //b) check the edges
  for (unsigned i =0; i < active_edges.size(); ++i)
  {
    dbsksp_xshock_edge_sptr xe = active_edges[i];
    if (xe->is_terminal_edge())
      continue;

    bool is_valid = true;

    dbsksp_xshock_node_descriptor start = *(xe->parent_node()->descriptor(xe));
    dbsksp_xshock_node_descriptor end = xe->child_node()->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(start, end);
    is_valid = is_valid && xfrag.is_legal();

    if (!is_valid)
      return false;
  }

  return true;
}













//------------------------------------------------------------------------------
//: Compute root node and edge for a given xgraph tree
bool dbsksp_compute_xgraph_root_node(const dbsksp_xshock_directed_tree_sptr& tree,
                                     dbsksp_xshock_node_sptr& root_node, 
                                     dbsksp_xshock_edge_sptr& pseudo_parent_edge)
{
  dbsksp_xshock_directed_tree_sptr common_tree1 = tree;

  // use tree1 as the basis for choosing root node
  int centroid = common_tree1->centroid();

  // find the dart with maximum costs
  vcl_vector<int > out_darts = common_tree1->out_darts(centroid);
  float max_subtree_cost = -1.0f;
  int max_dart_id = 0;
  for (unsigned i =0; i < out_darts.size(); ++i)
  {
    float subtree_cost = common_tree1->subtree_delete_cost(out_darts[i]);
    if ( subtree_cost > max_subtree_cost)
    {
      max_subtree_cost = subtree_cost;
      max_dart_id = out_darts[i];
    }
  }

  vcl_vector<dbsksp_xshock_edge_sptr > edge_path;
  dbsksp_xshock_node_sptr start_node = 0;
  common_tree1->get_edge_list(vcl_vector<int >(1, max_dart_id), start_node, edge_path);

  // save results
  root_node = start_node;
  pseudo_parent_edge = edge_path.front();

  return true;
}





//------------------------------------------------------------------------------
//: Compute coarse Euler tour for an xgraph (ignore degree-2 nodes)
bool dbsksp_compute_coarse_euler_tour(const dbsksp_xshock_graph_sptr& model_xgraph, 
    vcl_vector<dbsksp_xshock_node_sptr >& coarse_euler_tour)
{
  // sanitize output storage
  coarse_euler_tour.clear();

  // Begin with a degree-1 node
  dbsksp_xshock_node_sptr start_xv = *(model_xgraph->vertices_begin());
  dbsksp_xshock_edge_sptr start_xe = *(start_xv->edges_begin());
  while (start_xv->degree() != 1)
  {
    start_xv = start_xe->opposite(start_xv);
    start_xe = model_xgraph->cyclic_adj_succ(start_xe, start_xv);
  }

  // Keep traversing along the Euler tour, keeping only degree-1 and -3 nodes
  dbsksp_xshock_node_sptr cur_xv = start_xv;
  dbsksp_xshock_edge_sptr cur_xe = start_xe;
  do
  {
    // save the node
    if (cur_xv->degree() != 2)
    {
      coarse_euler_tour.push_back(cur_xv);
    }

    cur_xv = cur_xe->opposite(cur_xv);
    cur_xe = model_xgraph->cyclic_adj_succ(cur_xe, cur_xv);
  }
  while (cur_xv != start_xv || cur_xe != start_xe);

  return true;
}
