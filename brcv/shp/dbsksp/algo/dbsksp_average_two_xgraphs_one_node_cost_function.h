// This is shp/dbsksp/algo/dbsksp_average_two_xgraphs_one_node_cost_function.h
#ifndef dbsksp_average_two_xgraphs_one_node_cost_function_h_
#define dbsksp_average_two_xgraphs_one_node_cost_function_h_

//:
// \file
// \brief Cost function for one node during xgraph averaging
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Mar 5, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/algo/dbsksp_average_two_xgraphs_cost_function.h>

//==============================================================================
// dbsksp_average_two_xgraphs_one_node_cost_function
//==============================================================================

//: Cost function for varying a shock node
class dbsksp_average_two_xgraphs_one_node_cost_function: public dbsksp_average_two_xgraphs_cost_function
{
public:
  //: Constructor
  dbsksp_average_two_xgraphs_one_node_cost_function(const dbsksp_xshock_directed_tree_sptr& tree1, // original tree1
    const dbsksp_xshock_directed_tree_sptr& tree2, // original tree2
    const dbsksp_xshock_directed_tree_sptr& model_tree, // model tree, to be optimized
    const vcl_vector<pathtable_key >& correspondence1, // correspondence between tree1 and model tree
    const vcl_vector<pathtable_key >& correspondence2, // correspondence between tree2 and model tree
    float scurve_matching_R, // parameter R in shock matching
    dbsksp_xgraph_model_using_L_alpha_phi_radius* const xgraph_model, // a model to modify the xgraph inside model_tree
    unsigned active_vid,
    float geodesic_tree1_tree2,         //> geodesic distance between two original graphs
    float distance_parent1_to_tree1,    //> geodesic distance between parent of tree1 and tree1 (to add to overall distance)
    float distance_parent2_to_tree2);   //> geodesic distance between parent of tree2 and tree2 (to add to overall distance)

  //: Destructor
  ~dbsksp_average_two_xgraphs_one_node_cost_function(){}

  //: Main function
  virtual double f(const vnl_vector<double >& x);

  //: Last value of x, retrieved from model
  void last_x(vnl_vector<double >& x);

protected:
  // User input-----------------------------------------------------------------
  unsigned active_vid_;
  dbsksp_xshock_node_sptr active_xv_;


  // Intermediate variables ----------------------------------------------------
  
  // correspondence contributing to this cost function
  vcl_vector<bool > active_corr_[2];

  vcl_vector<dbsksp_xshock_node_sptr > active_nodes_;
  vcl_vector<dbsksp_xshock_edge_sptr > active_edges_;

  // geodesic distance between two original shapes
  float distance_parent1_to_parent2_;

  // distance between tree1/tree2 and its parent (the real original shape)
  float distance_ancestor_to_parent_[2];


  // cache variables for fast access
  vcl_vector<double* > x_ptr_;

  // sum of costs external to this function's scope
  float external_cost_[2];  
};



#endif // shp/dbsksp/algo/dbsksp_average_two_xgraphs_one_node_cost_function.h









