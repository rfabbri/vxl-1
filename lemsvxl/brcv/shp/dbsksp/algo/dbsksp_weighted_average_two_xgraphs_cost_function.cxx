// This is file shp/dbsksp/algo/dbsksp_weighted_average_two_xgraphs_cost_base.cxx

//:
// \file

#include "dbsksp_weighted_average_two_xgraphs_cost_function.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
#include <dbsksp/algo/dbsksp_avg_xgraph_utils.h>

#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>
#include <vcl_set.h>



//==============================================================================
// dbsksp_weighted_average_two_xgraphs_cost_base
//==============================================================================


//------------------------------------------------------------------------------
//: Constructor
dbsksp_weighted_average_two_xgraphs_cost_base::
dbsksp_weighted_average_two_xgraphs_cost_base(
  const dbsksp_xshock_directed_tree_sptr& tree1,      // original tree1
  const dbsksp_xshock_directed_tree_sptr& tree2,      // original tree2
  const dbsksp_xshock_directed_tree_sptr& model_tree, // model tree, to be optimized
  const vcl_vector<pathtable_key >& correspondence1,  // correspondence between tree1 and model tree
  const vcl_vector<pathtable_key >& correspondence2,  // correspondence between model tree and tree2
  float scurve_matching_R,                            // parameter R in shock matching
  dbsksp_xgraph_model_using_L_alpha_phi_radius* const xgraph_model // a model to modify the xgraph inside model_tree
  )
{
  this->parent_tree_[0] = tree1;
  this->parent_tree_[1] = tree2;
  this->model_tree_ = model_tree;
  this->dart_corr_parent_to_model_[0] = correspondence1;
  this->dart_corr_parent_to_model_[1] = correspondence2;
  this->scurve_matching_R_ = scurve_matching_R;
  this->xgraph_model_ = xgraph_model;
  return;
}


////------------------------------------------------------------------------------
////: Compute cache data for min and max of kdiff
//// Assumption: the two original graphs have exact same topology
//void dbsksp_weighted_average_two_xgraphs_cost_base::
//compute_kdiff_min_max()
//{
//  ////
//  //this->map_model_xfrag_properties_.clear();
//  //dbsksp_xshock_graph_sptr xgraph = this->model_tree_->xgraph();
//  //for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
//  //  xgraph->edges_end(); ++eit)
//  //{
//  //  dbsksp_xshock_edge_sptr model_xe = *eit;
//
//  //  if (model_xe->is_terminal_edge())
//  //    continue;
//
//  //  dbsksp_xshock_node_sptr xv_parent = model_xe->parent_node();
//  //  dbsksp_xshock_node_sptr xv_child = model_xe->child_node();
//
//  //  dbsksp_xshock_edge_sptr xgraph1_xe = this->edge_map_model_to_parent_[0][model_xe];
//  //  dbsksp_xshock_edge_sptr xgraph2_xe = this->edge_map_model_to_parent_[1][model_xe];
//
//  //  // Compute kdiff for xgraph1
//  //  double kdiff1 = vnl_numeric_traits<double >::maxval;
//  //  {
//  //    dbsksp_xshock_edge_sptr xe = xgraph1_xe;
//  //    dbsksp_xshock_node_descriptor start = *(xe->parent_node()->descriptor(xe));
//  //    dbsksp_xshock_node_descriptor end = xe->child_node()->descriptor(xe)->opposite_xnode();
//  //    dbsksp_xshock_fragment xfrag(start, end);
//
//  //    dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
//  //    dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();
//
//  //    double kdiff = vnl_math::sqr(left_bnd.k1() - left_bnd.k2()) + 
//  //      vnl_math::sqr(right_bnd.k1() - right_bnd.k2());
//
//  //    kdiff1 = kdiff;
//  //  }
//
//  //  double kdiff2 = vnl_numeric_traits<double >::maxval;
//  //  {
//  //    dbsksp_xshock_edge_sptr xe = xgraph2_xe;
//  //    dbsksp_xshock_node_descriptor start = *(xe->parent_node()->descriptor(xe));
//  //    dbsksp_xshock_node_descriptor end = xe->child_node()->descriptor(xe)->opposite_xnode();
//  //    dbsksp_xshock_fragment xfrag(start, end);
//
//  //    dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
//  //    dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();
//
//  //    double kdiff = vnl_math::sqr(left_bnd.k1() - left_bnd.k2()) + 
//  //      vnl_math::sqr(right_bnd.k1() - right_bnd.k2());
//
//  //    kdiff2 = kdiff;
//  //  }
//
//  //  xfrag_properties p;
//  //  p.kdiff_max = vnl_math::max(kdiff1, kdiff2);
//  //  p.kdiff_min = vnl_math::min(kdiff1, kdiff2);
//
//  //  this->map_model_xfrag_properties_.insert(vcl_make_pair(model_xe->id(), p));
//  //}
//  return;
//}





//------------------------------------------------------------------------------
//: Compute kdiff cost for a fragment, given the id of the edge
double dbsksp_weighted_average_two_xgraphs_cost_base::
compute_kdiff_cost(unsigned eid, const dbsksp_xshock_fragment& xfrag)
{
  return 0;

  dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
  dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

  double kdiff = vnl_math::sqr(left_bnd.k1() - left_bnd.k2()) + 
    vnl_math::sqr(right_bnd.k1() - right_bnd.k2());

  // compute cost if the sum_kdiff is not within range set by the two original graph
  xfrag_properties p = this->map_model_xfrag_properties_[eid];
  double min_kdiff = p.kdiff_min;
  double max_kdiff = p.kdiff_max;

  double kdiff_cost = 0;
  kdiff_cost = (kdiff < min_kdiff) ? (vcl_exp((min_kdiff - kdiff))-1) : kdiff_cost;
  kdiff_cost = (kdiff > max_kdiff) ? (vcl_exp((kdiff - max_kdiff))-1) : kdiff_cost;

  return kdiff_cost;
}


//------------------------------------------------------------------------------
//: Compute max-curvature cost - penalize boundary curvature higher than max
// curvature dictated by radius at the node
double dbsksp_weighted_average_two_xgraphs_cost_base::
compute_kmax_cost(unsigned eid, const dbsksp_xshock_fragment& xfrag)
{
  return dbsksp_compute_kmax_cost(xfrag);  
}








