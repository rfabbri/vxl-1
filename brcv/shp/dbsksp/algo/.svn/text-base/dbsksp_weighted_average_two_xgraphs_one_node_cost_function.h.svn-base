// This is shp/dbsksp/algo/dbsksp_weighted_average_two_xgraphs_one_node_cost_function.h
#ifndef dbsksp_weighted_average_two_xgraphs_one_node_cost_function_h_
#define dbsksp_weighted_average_two_xgraphs_one_node_cost_function_h_

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

#include <dbsksp/algo/dbsksp_weighted_average_two_xgraphs_cost_function.h>
#include <vnl/vnl_cost_function.h>

//==============================================================================
// dbsksp_weighted_average_two_xgraphs_one_node_cost_function
//==============================================================================

//: Cost function for varying a shock node
class dbsksp_weighted_average_two_xgraphs_one_node_cost_function: 
  public dbsksp_weighted_average_two_xgraphs_cost_base,
  public vnl_cost_function
{
public:
  //: Constructor
  dbsksp_weighted_average_two_xgraphs_one_node_cost_function(
    const dbsksp_xshock_directed_tree_sptr& tree1, // original tree1
    const dbsksp_xshock_directed_tree_sptr& tree2,      // original tree2
    const dbsksp_xshock_directed_tree_sptr& model_tree, // model tree, to be optimized
    const vcl_vector<pathtable_key >& correspondence1,  // correspondence between tree1 and model tree
    const vcl_vector<pathtable_key >& correspondence2,  // correspondence between tree2 and model tree
    float scurve_matching_R,                            // parameter R in shock matching
    dbsksp_xgraph_model_using_L_alpha_phi_radius* const xgraph_model, // a model to modify the xgraph inside model_tree
    unsigned active_vid,                                // id of active node
    double target_distance_tree1_to_model,              // desired distance from model to tree1
    double target_distance_tree2_to_model,              // desired distance from model to tree1
    double contract_cost_tree1,                         // contract cost of tree1 (in optimal matching)
    double contract_cost_tree2                          // contract cost of tree2 (in optimal matching)
    );

  //: Destructor
  ~dbsksp_weighted_average_two_xgraphs_one_node_cost_function(){}

  //: Main function
  virtual double f(const vnl_vector<double >& x);

  //: Last value of x, retrieved from model
  void last_x(vnl_vector<double >& x);

  // Support functions-------------------------------------------------------

protected:
  
  //: Determine active nodes and edges
  void determine_active_nodes_and_edges();

  //: Compute deformation cost of inactive edges (dart paths)
  void compute_deform_cost_of_inactive_edges();

  //: Return true if parameter values of xgraph are within allowed range
  bool is_valid(const dbsksp_xshock_graph_sptr& xgraph);


  // User input-----------------------------------------------------------------
  unsigned active_vid_;
  dbsksp_xshock_node_sptr active_xv_;


  // Intermediate variables ----------------------------------------------------
  
  // correspondence contributing to this cost function
  vcl_vector<bool > active_corr_[2];

  vcl_vector<dbsksp_xshock_node_sptr > active_nodes_;
  vcl_vector<dbsksp_xshock_edge_sptr > active_edges_;

  //: Contract costs
  double contract_cost_[2];

  // cache variables for fast access
  vcl_vector<double* > x_ptr_;

  // sum of costs external to this function's scope
  float external_cost_[2];  

  // Target distance from ancestor shape to model
  double target_distance_parent_to_model_[2];
};



#endif // shp/dbsksp/algo/dbsksp_weighted_average_two_xgraphs_one_node_cost_function.h









