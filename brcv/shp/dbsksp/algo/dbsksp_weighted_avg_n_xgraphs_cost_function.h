// This is shp/dbsksp/algo/dbsksp_weighted_avg_n_xgraphs_cost_function.h
#ifndef dbsksp_weighted_avg_n_xgraphs_cost_function_h_
#define dbsksp_weighted_avg_n_xgraphs_cost_function_h_

//:
// \file
// \brief Cost function for one node during xgraph averaging
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Mar 29, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/algo/dbsksp_weighted_average_two_xgraphs_cost_function.h>
#include <vnl/vnl_cost_function.h>


//==============================================================================
// dbsksp_weighted_avg_n_xgraphs_cost_base
//==============================================================================

//: Base class for cost function to compute average of n xgraphs
class dbsksp_weighted_avg_n_xgraphs_cost_base
{
public:
  //: Constructor
  dbsksp_weighted_avg_n_xgraphs_cost_base(
    const vcl_vector<dbsksp_xshock_directed_tree_sptr >& parent_tree,
    const vcl_vector<double >& parent_weight,
    const dbsksp_xshock_directed_tree_sptr& model_tree,
    const vcl_vector<vcl_vector<pathtable_key > >& dart_corr_parent_to_model,
    float scurve_matching_R):
  parent_tree_(parent_tree), model_tree_(model_tree),
    dart_corr_parent_to_model_(dart_corr_parent_to_model),
    scurve_matching_R_(scurve_matching_R)
  {
    
  }

  //: Destructor
  virtual ~dbsksp_weighted_avg_n_xgraphs_cost_base(){};

  // Utilities------------------------------------------------------------------

protected:

  // Member variables------------------------------------------------------------------

  //: Parent xgraphs
  vcl_vector<dbsksp_xshock_directed_tree_sptr > parent_tree_;
  vcl_vector<double > parent_weight_;

  //: Model xgraph to optimize
  dbsksp_xshock_directed_tree_sptr model_tree_; // model tree, to be optimized

  //: Correspondence between parent tree and the model tree
  vcl_vector<vcl_vector<pathtable_key > >dart_corr_parent_to_model_; // correspondence between tree1 and model tree

  //: Parameter R in shock matching
  float scurve_matching_R_;
};




//==============================================================================
// dbsksp_weighted_avg_n_xgraphs_one_node_cost_function
//==============================================================================

//: Cost function for varying a shock node
class dbsksp_weighted_avg_n_xgraphs_one_node_cost_function: 
  public dbsksp_weighted_avg_n_xgraphs_cost_base,
  public vnl_cost_function
{
public:
  //: Constructor
  dbsksp_weighted_avg_n_xgraphs_one_node_cost_function(
    const dbsksp_xshock_directed_tree_sptr&              model_tree,                  // model tree, to be optimized
    const vcl_vector<dbsksp_xshock_directed_tree_sptr >& parent_trees, 
    const vcl_vector<double >& parent_weights,
    const vcl_vector<vcl_vector<pathtable_key > >&       dart_corr_parent_to_model_,  // correspondence between parent trees and model tree
    const vcl_vector<double >&                           parent_contract_and_splice_cost,         // contract cost of tree1 (in optimal matching)
    float scurve_matching_R,                                                          // parameter R in shock matching
    dbsksp_xgraph_model_using_L_alpha_phi_radius*        xgraph_model,                 // a model to modify the xgraph inside model_tree
    unsigned active_vid                               // id of active node
    );

  //: Destructor
  ~dbsksp_weighted_avg_n_xgraphs_one_node_cost_function(){}

  //: Main function
  virtual double f(const vnl_vector<double >& x);


  //: Current state of the xgraph
  void cur_x(vnl_vector<double >& x);

  ////: Return a list of xshock nodes corresponding to a given value of the unknown "x"
  //bool x_to_xsamples(const vnl_vector<double >& x, 
  //  vcl_vector<dbsksp_xshock_node_descriptor >& xsamples);


  // Support functions-------------------------------------------------------

protected:
  
  //: Determine active nodes and edges
  void determine_active_nodes_and_edges();

  //: Compute deformation cost of inactive edges (dart paths)
  void compute_deform_cost_of_inactive_edges();

  // User input-----------------------------------------------------------------

  //: Contract costs
  vcl_vector<double > parent_contract_and_splice_cost_;
  
  //: Model to generate the xgraph
  dbsksp_xgraph_model_using_L_alpha_phi_radius* xgraph_model_; // a model to modify the xgraph inside model_tree

  unsigned active_vid_;
  dbsksp_xshock_node_sptr active_xv_;




  // Intermediate variables ----------------------------------------------------

  int num_parents_;

  vcl_vector<dbsksp_xshock_node_sptr > active_nodes_; //set
  vcl_vector<dbsksp_xshock_edge_sptr > active_edges_; //set

  // cache variables for fast access
  vcl_vector<double* > x_ptr_;                        //set
  
  // correspondence contributing to this cost function
  vcl_vector<vcl_vector<bool > > active_corr_;       //set

  
  //: costs that are not affected by node
  vcl_vector<float > fixed_deform_cost_; //set
};



#endif // shp/dbsksp/algo/dbsksp_weighted_avg_n_xgraphs_one_node_cost_function.h









