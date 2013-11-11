// This is file shp/dbsksp/algo/dbsksp_average_two_xgraphs_cost_function.cxx

//:
// \file

#include "dbsksp_average_two_xgraphs_cost_function.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>

#include <vnl/vnl_math.h>
#include <vcl_set.h>

//==============================================================================
// dbsksp_average_two_xgraphs_cost_function
//==============================================================================


//------------------------------------------------------------------------------
//: Constructor
dbsksp_average_two_xgraphs_cost_function::
dbsksp_average_two_xgraphs_cost_function(const dbsksp_xshock_directed_tree_sptr& tree1, // original tree1
                                         const dbsksp_xshock_directed_tree_sptr& tree2, // original tree2
                                         const dbsksp_xshock_directed_tree_sptr& model_tree, // model tree, to be optimized
                                         const vcl_vector<pathtable_key >& correspondence1, // correspondence between tree1 and model tree
                                         const vcl_vector<pathtable_key >& correspondence2, // correspondence between model tree and tree2
                                         float scurve_matching_R, // parameter R in shock matching
                                         dbsksp_xgraph_model_using_L_alpha_phi_radius* const xgraph_model // a model to modify the xgraph inside model_tree
                                         ): vnl_cost_function(0)
{
  this->parent_tree_[0] = tree1;
  this->parent_tree_[1] = tree2;
  this->model_tree_ = model_tree;
  this->dart_corr_parent_to_model_[0] = correspondence1;
  this->dart_corr_parent_to_model_[1] = correspondence2;
  this->scurve_matching_R_ = scurve_matching_R;
  this->xgraph_model_ = xgraph_model;

  if (xgraph_model_)
  {
    vnl_cost_function::set_number_of_unknowns(xgraph_model_->dim());
  }
  else
  {
    vcl_cerr << "\nERROR: xgraph_model is not set.\n";
  }
  return;
}







//------------------------------------------------------------------------------
//: Main function
double dbsksp_average_two_xgraphs_cost_function::
f(const vnl_vector<double >& x)
{
  vnl_vector<double > fx(3, 0);

  // Update the shock graph with `x'
  this->xgraph_model_->set_xgraph_state(x);

  // check validity of the xgraph
  dbsksp_xshock_graph_sptr xgraph = this->xgraph_model_->xgraph();
  bool is_valid = true;

  //a) check the nodes
  for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin(); vit !=
    xgraph->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = (*vit);
    is_valid = is_valid && (xv->radius() >= 0);
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit !=
      xv->edges_end(); ++eit)
    {
      dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(*eit);
      is_valid = is_valid && (xdesc->phi() >= 0);
    }

    if (!is_valid)
      return vnl_numeric_traits<double >::maxval;
  }

  //b) check the edges
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
    xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    if (xe->is_terminal_edge())
      continue;

    dbsksp_xshock_node_descriptor start = *(xe->parent_node()->descriptor(xe));
    dbsksp_xshock_node_descriptor end = xe->child_node()->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(start, end);
    is_valid = is_valid && xfrag.is_legal();

    if (!is_valid)
      return vnl_numeric_traits<double >::maxval;
  }
  
  // check curvature for each fragment. Enforce between two original xgraphs
  double total_kdiff_cost = 0;
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
    xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    if (xe->is_terminal_edge())
      continue;

    dbsksp_xshock_node_descriptor start = *(xe->parent_node()->descriptor(xe));
    dbsksp_xshock_node_descriptor end = xe->child_node()->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(start, end);

    total_kdiff_cost += this->compute_kdiff_cost(xe->id(), xfrag);
  }
  fx(2) = total_kdiff_cost;
  
  
  // edit-cost wrt two original graphs
  this->model_tree_->clear_dart_path_scurve_map();

  for (int i =0; i < 2; ++i)
  {
    dbsksp_edit_distance edit_distance1;
    edit_distance1.set_scurve_matching_R(this->scurve_matching_R_);
    edit_distance1.set_tree1(this->parent_tree_[i]);
    edit_distance1.set_tree2(this->model_tree_);
    edit_distance1.save_path(true);

    fx[i] = edit_distance1.get_deform_cost(this->dart_corr_parent_to_model_[i]);
  }
  return fx.magnitude();
}




//------------------------------------------------------------------------------
//: Compute cache data for min and max of kdiff
// Assumption: the two original graphs have exact same topology
void dbsksp_average_two_xgraphs_cost_function::
compute_kdiff_min_max()
{
  this->map_model_xfrag_properties_.clear();
  dbsksp_xshock_graph_sptr xgraph = this->model_tree_->xgraph();
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
    xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr model_xe = *eit;

    if (model_xe->is_terminal_edge())
      continue;

    dbsksp_xshock_node_sptr xv_parent = model_xe->parent_node();
    dbsksp_xshock_node_sptr xv_child = model_xe->child_node();

    dbsksp_xshock_edge_sptr xgraph1_xe = this->edge_map_model_to_parent_[0][model_xe];
    dbsksp_xshock_edge_sptr xgraph2_xe = this->edge_map_model_to_parent_[1][model_xe];

    // Compute kdiff for xgraph1
    double kdiff1 = vnl_numeric_traits<double >::maxval;
    {
      dbsksp_xshock_edge_sptr xe = xgraph1_xe;
      dbsksp_xshock_node_descriptor start = *(xe->parent_node()->descriptor(xe));
      dbsksp_xshock_node_descriptor end = xe->child_node()->descriptor(xe)->opposite_xnode();
      dbsksp_xshock_fragment xfrag(start, end);

      dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
      dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

      double kdiff = vnl_math_sqr(left_bnd.k1() - left_bnd.k2()) + 
        vnl_math_sqr(right_bnd.k1() - right_bnd.k2());

      kdiff1 = kdiff;
    }

    double kdiff2 = vnl_numeric_traits<double >::maxval;
    {
      dbsksp_xshock_edge_sptr xe = xgraph2_xe;
      dbsksp_xshock_node_descriptor start = *(xe->parent_node()->descriptor(xe));
      dbsksp_xshock_node_descriptor end = xe->child_node()->descriptor(xe)->opposite_xnode();
      dbsksp_xshock_fragment xfrag(start, end);

      dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
      dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

      double kdiff = vnl_math_sqr(left_bnd.k1() - left_bnd.k2()) + 
        vnl_math_sqr(right_bnd.k1() - right_bnd.k2());

      kdiff2 = kdiff;
    }

    xfrag_properties p;
    p.kdiff_max = vnl_math_max(kdiff1, kdiff2);
    p.kdiff_min = vnl_math_min(kdiff1, kdiff2);

    this->map_model_xfrag_properties_.insert(vcl_make_pair(model_xe->id(), p));
  }
  return;
}





//------------------------------------------------------------------------------
//: Compute kdiff cost for a fragment, given the id of the edge
double dbsksp_average_two_xgraphs_cost_function::
compute_kdiff_cost(unsigned eid, const dbsksp_xshock_fragment& xfrag)
{
  dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
  dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

  double kdiff = vnl_math_sqr(left_bnd.k1() - left_bnd.k2()) + 
    vnl_math_sqr(right_bnd.k1() - right_bnd.k2());

  // compute cost if the sum_kdiff is not within range set by the two original graph
  xfrag_properties p = this->map_model_xfrag_properties_[eid];
  double min_kdiff = p.kdiff_min;
  double max_kdiff = p.kdiff_max;

  double kdiff_cost = 0;
  kdiff_cost = (kdiff < min_kdiff) ? (vcl_exp((min_kdiff - kdiff))-1) : kdiff_cost;
  kdiff_cost = (kdiff > max_kdiff) ? (vcl_exp((kdiff - max_kdiff))-1) : kdiff_cost;

  return kdiff_cost;
}








