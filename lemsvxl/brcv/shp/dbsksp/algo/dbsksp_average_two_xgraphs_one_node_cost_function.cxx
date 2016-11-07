// This is file shp/dbsksp/algo/dbsksp_average_two_xgraphs_cost_function.cxx

//:
// \file

#include "dbsksp_average_two_xgraphs_one_node_cost_function.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>

#include <vnl/vnl_math.h>
#include <vcl_set.h>






//==============================================================================
// dbsksp_average_two_xgraphs_one_node_cost_function
//==============================================================================

//------------------------------------------------------------------------------
//: Constructor
dbsksp_average_two_xgraphs_one_node_cost_function::
dbsksp_average_two_xgraphs_one_node_cost_function(const dbsksp_xshock_directed_tree_sptr& tree1, // original tree1
                                                  const dbsksp_xshock_directed_tree_sptr& tree2, // original tree2
                                                  const dbsksp_xshock_directed_tree_sptr& model_tree, // model tree, to be optimized
                                                  const vcl_vector<pathtable_key >& correspondence1, // correspondence between tree1 and model tree
                                                  const vcl_vector<pathtable_key >& correspondence2, // correspondence between model tree and tree2
                                                  float scurve_matching_R, // parameter R in shock matching
                                                  dbsksp_xgraph_model_using_L_alpha_phi_radius* const xgraph_model, // a model to modify the xgraph inside model_tree
                                                  unsigned active_vid,
                                                  float distance_tree1_to_tree2,
                                                  float distance_parent1_to_tree1,
                                                  float distance_parent2_to_tree2):
  dbsksp_average_two_xgraphs_cost_function(tree1, tree2, model_tree, correspondence1, correspondence2, scurve_matching_R, xgraph_model),
  active_vid_(active_vid), 
  distance_parent1_to_parent2_(distance_tree1_to_tree2)
{
  //1) assign distance to ancestor graphs
  this->distance_ancestor_to_parent_[0] = distance_parent1_to_tree1;
  this->distance_ancestor_to_parent_[1] = distance_parent2_to_tree2;

  //2) check validity of the selected node
  this->active_xv_ = this->xgraph_model_->xgraph()->node_from_id(this->active_vid_);
  if (!this->active_xv_)
  {
    vcl_cout << "\nERROR: active node not defined.\n";
    return;
  }


  if (this->active_xv_->degree() == 2)
  {
    vcl_cout << "\nERROR: node degree==2. This cost function does not apply for degree-2 node.\n";
    return;
  }

  //3) Determine active nodes and edges

  vcl_set<vcl_pair<int, int > > affected_dart_pairs;
  for (int idx = 0; idx < 2; ++idx)
  {
    vcl_vector<pathtable_key >& corr = this->dart_corr_parent_to_model_[idx];
    this->active_corr_[idx].resize(corr.size(), false);

    // determine parts of the model and cost affected in this cost
    for (unsigned k =0; k < corr.size(); ++k)
    {
      pathtable_key key = corr[k];

      int tree1_d1 = key.first.first;
      int tree1_d2 = key.first.second;

      int model_d1 = key.second.first;
      int model_d2 = key.second.second;

      // determine the edges and nodes involved
      vcl_vector<int > dart_path = this->model_tree_->get_dart_path(model_d1, model_d2);

      if (dart_path.empty())
        continue;

      vcl_vector<dbsksp_xshock_edge_sptr > list_xe;
      dbsksp_xshock_node_sptr start_xv;
      model_tree->get_edge_list(dart_path, start_xv, list_xe);

      // determine the last node in the path
      dbsksp_xshock_node_sptr end_xv = start_xv;
      for (unsigned i =0; i < list_xe.size(); ++i)
      {
        end_xv = list_xe[i]->opposite(end_xv);
      }

      // ignore all correspondence not containing the active node
      if (this->active_xv_ != start_xv && this->active_xv_ != end_xv)
        continue;

      // save the list of edge and the correspondence
      this->active_corr_[idx][k] = true;
      affected_dart_pairs.insert(vcl_pair<int, int >(model_d1, model_d2));
    }
  }

  // lists of edges and nodes affected in cost function
  vcl_set<dbsksp_xshock_edge_sptr > affected_edges;
  vcl_set<dbsksp_xshock_node_sptr > affected_nodes;
  if (this->active_xv_->degree() != 1)
  {
    affected_nodes.insert(this->active_xv_);
  }
  
  for (vcl_set<vcl_pair<int, int > >::iterator iter = affected_dart_pairs.begin();
    iter != affected_dart_pairs.end(); ++iter)
  {
    int model_d1 = iter->first;
    int model_d2 = iter->second;

    // determine the edges and nodes involved
    vcl_vector<int > dart_path = model_tree->get_dart_path(model_d1, model_d2);
    vcl_vector<dbsksp_xshock_edge_sptr > list_xe;
    dbsksp_xshock_node_sptr start_xv;
    model_tree->get_edge_list(dart_path, start_xv, list_xe);

    // list of affected edges
    for (unsigned i =0; i < list_xe.size(); ++i)
    {
      if (list_xe[i]->is_terminal_edge())
        continue;
      affected_edges.insert(list_xe[i]);
    }

    // list of affected nodes -- take all middle nodes, avoid end-nodes
    dbsksp_xshock_node_sptr temp_node = list_xe.front()->opposite(start_xv);
    for (unsigned i =1; i < list_xe.size(); ++i)
    {
      if (temp_node->degree() != 1)
      {
        affected_nodes.insert(temp_node);
      }
      temp_node = list_xe[i]->opposite(temp_node);
    }
  }

  // finalize list of active nodes and edges
  this->active_nodes_.clear();
  this->active_nodes_.insert(this->active_nodes_.end(), affected_nodes.begin(), affected_nodes.end());

  this->active_edges_.clear();
  this->active_edges_.insert(this->active_edges_.end(), affected_edges.begin(), affected_edges.end());

  //4) Determine pointers to the active parameters
  this->xgraph_model_->get_intrinsic_params_ptr(this->active_nodes_, this->active_edges_, this->x_ptr_);

  //5) save number of unknowns
  vnl_cost_function::set_number_of_unknowns(this->x_ptr_.size());

  //6) compute external costs
  for (int idx = 0; idx < 2; ++idx)
  {
    vcl_vector<pathtable_key>& corr = this->dart_corr_parent_to_model_[idx];
    vcl_vector<bool >& active_corr = this->active_corr_[idx];


    this->external_cost_[idx] = 0;
  
    // sum up deformation cost of INACTIVE darts
    dbsksp_edit_distance edit;
    edit.set_scurve_matching_R(this->scurve_matching_R_);
    edit.set_tree1(this->parent_tree_[idx]);
    edit.set_tree2(this->model_tree_);
    
    for (unsigned i =0; i < corr.size(); ++i)
    {
      // only add cost of inactive correspondence
      if (active_corr[i])
        continue;

      this->external_cost_[idx] += edit.get_deform_cost(corr[i]);
    }
  }

  return;
}


//------------------------------------------------------------------------------
//: Main function
double dbsksp_average_two_xgraphs_one_node_cost_function::
f(const vnl_vector<double >& x)
{
  // target distance is half of length of the geodesic between two PARENT trees
  double target_distance = 0.5* (this->distance_ancestor_to_parent_[0] + 
                                 this->distance_ancestor_to_parent_[1] + 
                                 this->distance_parent1_to_parent2_);

  // cost components
  vnl_vector<double > fx(3, 0);

  //1) Update parameters of model with `x'
  assert(x.size() == this->x_ptr_.size());

  for (unsigned i =0; i < x.size(); ++i)
  {
    *(this->x_ptr_[i]) = x[i];
  }

  //2) update model xgraph
  this->xgraph_model_->update_xgraph_from_param_maps();

  //3) Check validity

  dbsksp_xshock_graph_sptr xgraph = this->xgraph_model_->xgraph();
  //3a) check the nodes  
  for (unsigned i =0; i < this->active_nodes_.size(); ++i)
  {
    bool is_valid = true;

    dbsksp_xshock_node_sptr xv = this->active_nodes_[i];
    is_valid = is_valid && (xv->radius() >= 0);

    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != xv->edges_end(); ++eit)
    {
      dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(*eit);
      is_valid = is_valid && (xdesc->phi() >= 0);
    }
    if (!is_valid)
      return vnl_numeric_traits<double >::maxval;
  }

  //3b) check the edges
  for (unsigned i =0; i < this->active_edges_.size(); ++i)
  {
    
    dbsksp_xshock_edge_sptr xe = this->active_edges_[i];
    if (xe->is_terminal_edge())
      continue;

    bool is_valid = true;

    dbsksp_xshock_node_descriptor start = *(xe->parent_node()->descriptor(xe));
    dbsksp_xshock_node_descriptor end = xe->child_node()->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(start, end);
    is_valid = is_valid && xfrag.is_legal();

    if (!is_valid)
      return vnl_numeric_traits<double >::maxval;
  }
  
  
  //4) Compute deform cost
  this->model_tree_->clear_dart_path_scurve_map();

  for (int idx =0; idx < 2; ++idx)
  {
    double deform_cost = 0;

    //a) Deformation cost between parent and model
    dbsksp_edit_distance edit_distance1;
    edit_distance1.set_scurve_matching_R(this->scurve_matching_R_);
    edit_distance1.set_tree1(this->parent_tree_[idx]);
    edit_distance1.set_tree2(this->model_tree_);

    // compute deformation cost from active correspondence pairs
    vcl_vector<pathtable_key >& corr = this->dart_corr_parent_to_model_[idx];
    vcl_vector<bool >& active_corr = this->active_corr_[idx];
    for (unsigned i =0; i < corr.size(); ++i)
    {
      if (!active_corr[i])
        continue;
      deform_cost += edit_distance1.get_deform_cost(corr[i]);  
    }

    // add it with external costs
    fx[idx] = vnl_math::abs( (deform_cost + this->external_cost_[idx] + this->distance_ancestor_to_parent_[idx]) -
                            target_distance );
  }

  // 5) Curvature cost
  // Penalize when total curvature difference is not between two original xgraphs
  double total_kdiff_cost = 0;
  for (unsigned i =0; i < this->active_edges_.size(); ++i)
  {
    dbsksp_xshock_edge_sptr xe = this->active_edges_[i];

    if (xe->is_terminal_edge())
      continue;

    dbsksp_xshock_node_descriptor start = *(xe->parent_node()->descriptor(xe));
    dbsksp_xshock_node_descriptor end = xe->child_node()->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(start, end);

    total_kdiff_cost += this->compute_kdiff_cost(xe->id(), xfrag);
  }
  fx(2) = total_kdiff_cost;
  

  return fx.magnitude();
}





//------------------------------------------------------------------------------
//: Last value of x, retrieved from model
void dbsksp_average_two_xgraphs_one_node_cost_function::
last_x(vnl_vector<double >& x)
{
  this->xgraph_model_->update_param_maps_from_xgraph();
  x.set_size(this->x_ptr_.size());
  for (unsigned i =0; i < x.size(); ++i)
  {
    x[i] = *(this->x_ptr_[i]);
  }
}

