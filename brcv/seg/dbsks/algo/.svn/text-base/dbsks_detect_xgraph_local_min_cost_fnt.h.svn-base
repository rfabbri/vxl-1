// This is dbsks/algo/dbsks_detect_xgraph_local_min_cost_fnt.h
#ifndef dbsks_detect_xgraph_local_min_cost_fnt_h_
#define dbsks_detect_xgraph_local_min_cost_fnt_h_

//:
// \file
// \brief Cost function to optimize an xgraph locally to match to an image
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date April 11, 2010
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include <dbsks/dbsks_biarc_sampler_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>

#include <vnl/vnl_cost_function.h>
#include <vcl_vector.h>
#include <vcl_string.h>



//==============================================================================
// dbsks_detect_xgraph_local_min_cost_fnt_base
//==============================================================================

//: Cost function to optimize an xgraph locally to match to an image
class dbsks_detect_xgraph_local_min_cost_fnt_base
{
public:
  //: Constructor---------------------------------------------------------------
  dbsks_detect_xgraph_local_min_cost_fnt_base(){};

  //: Destructor----------------------------------------------------------------
  ~dbsks_detect_xgraph_local_min_cost_fnt_base(){};

public:

};



//==============================================================================
// dbsks_detect_xgraph_local_min_one_node_cost_fnt
//==============================================================================

//: Cost function for varying a shock node
class dbsks_detect_xgraph_local_min_one_node_cost_fnt: 
  public dbsks_detect_xgraph_local_min_cost_fnt_base,
  public vnl_cost_function
{
public:
  //: Constructor
  dbsks_detect_xgraph_local_min_one_node_cost_fnt();

  //: Destructor
  ~dbsks_detect_xgraph_local_min_one_node_cost_fnt(){}

  //: Main function
  virtual double f(const vnl_vector<double >& x);

  //: Current state of the xgraph
  void cur_x(vnl_vector<double >& x);

  // Support functions-------------------------------------------------------

protected:
  
  //: Determine active nodes and edges
  void determine_active_nodes_and_edges();

  //: Compute deformation cost of inactive edges (dart paths)
  void compute_cost_of_inactive_edges();

  // User input-----------------------------------------------------------------
  
  ////: Model to generate the xgraph
  //dbsksp_xgraph_model_using_L_alpha_phi_radius* xgraph_model_; // a model to modify the xgraph inside model_tree

  unsigned active_vid_;
  dbsksp_xshock_node_sptr active_xv_;


  // Intermediate variables ----------------------------------------------------
  vcl_vector<dbsksp_xshock_node_sptr > active_nodes_; //set
  vcl_vector<dbsksp_xshock_edge_sptr > active_edges_; //set

  // cache variables for fast access
  vcl_vector<double* > x_ptr_;                        //set
  
  //// correspondence contributing to this cost function
  //vcl_vector<vcl_vector<bool > > active_corr_;       //set
  
  //: costs that are not affected by node
  vcl_vector<float > fixed_deform_cost_; //set
};





#endif

