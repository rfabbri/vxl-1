// This is dbsks/algo/dbsks_detect_xgraph_local_min_cost_fnt.cxx

//:
// \file

#include "dbsks_detect_xgraph_local_min_cost_fnt.h"

#include <dbsksp/dbsksp_xshock_graph.h>




//==============================================================================
// dbsks_detect_xgraph_local_min_one_node_cost_fnt
//==============================================================================


//------------------------------------------------------------------------------
  //: Constructor
dbsks_detect_xgraph_local_min_one_node_cost_fnt::
dbsks_detect_xgraph_local_min_one_node_cost_fnt()
{

}


//------------------------------------------------------------------------------

//: Main function
double dbsks_detect_xgraph_local_min_one_node_cost_fnt::
f(const vnl_vector<double >& x)
{
  //1) Update parameters of model with `x'
  assert(x.size() == this->x_ptr_.size());

  for (unsigned i =0; i < x.size(); ++i)
  {
    *(this->x_ptr_[i]) = x[i];
  }

  //2) update model xgraph
  //this->xgraph_model_->update_xgraph_from_param_maps();

  ////3) Check validity
  //if (!dbsksp_is_valid_xgraph(this->xgraph_model_->xgraph(), this->active_nodes_, this->active_edges_))
  //{
  //  return 1e6; //vnl_numeric_traits<double >::maxval;
  //};
  

  ////5) Total cost = deform cost + contract cost + splice_cost
  //for (int idx =0; idx < this->num_parents_; ++idx)
  //{    
  //  fx[idx] = deform_cost[idx] + this->parent_contract_and_splice_cost_[idx];
  //}

  // 6) Regularization:
  double total_kmax_cost  = 0;
  //fx[this->num_parents_] = total_kmax_cost;
  

  return 0;
}


//------------------------------------------------------------------------------
//: Current state of the xgraph
void dbsks_detect_xgraph_local_min_one_node_cost_fnt::
cur_x(vnl_vector<double >& x)
{
  return;
}


//------------------------------------------------------------------------------
//: Determine active nodes and edges
void dbsks_detect_xgraph_local_min_one_node_cost_fnt::
determine_active_nodes_and_edges()
{
  return;
}


//------------------------------------------------------------------------------
//: Compute deformation cost of inactive edges (dart paths)
void dbsks_detect_xgraph_local_min_one_node_cost_fnt::
compute_cost_of_inactive_edges()
{
  return;
}




