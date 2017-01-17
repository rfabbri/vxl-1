// This is brcv/shp/dbsksp/algo/dbsksp_avg_n_xgraphs_grad_descent.h

#ifndef dbsksp_avg_n_xgraphs_grad_descent_h_
#define dbsksp_avg_n_xgraphs_grad_descent_h_


//:
// \file
// \brief A class to compute the average of N shock graphs
//
// \author Nhon H. Trinh (ntrinh@lems.brown.edu)
// \date Mar 30, 2010
// \verbatim
//   Modifications
//
// \endverbatim
//

#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree_sptr.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>
#include <vcl_vector.h>

//==============================================================================
// dbsksp_avg_n_xgraphs_grad_descent
//==============================================================================

//: A class to compute an average of two shock graphs
class dbsksp_avg_n_xgraphs_grad_descent
{
public:
  // Constructors / Destructors--------------------------------------------------

  //: Constructor
  dbsksp_avg_n_xgraphs_grad_descent();

  //: Destructor
  virtual ~dbsksp_avg_n_xgraphs_grad_descent() {}

  // Data access----------------------------------------------------------------

  //: Set list of parent xgraphs
  void set_parent_xgraphs(const vcl_vector<dbsksp_xshock_graph_sptr >& parent_xgraphs,
                          const vcl_vector<double >& parent_weights)
  {
    this->parent_xgraphs_ = parent_xgraphs;
    this->parent_weights_ = parent_weights;
    this->num_parents_ = this->parent_xgraphs_.size();
  }

  void set_init_model_xgraph(const dbsksp_xshock_graph_sptr& init_model)
  {
    this->init_model_xgraph_ = init_model;
  }


  //: Return scurve_matching_R
  float scurve_matching_R() const{ return this->scurve_matching_R_; }

  //: Set curve matching R
  void set_scurve_matching_R(float R){ this->scurve_matching_R_ = R; }

  //: Return sampling param
  float scurve_sample_ds() const {return this->scurve_sample_ds_;}

  //: Set sample ds
  void set_scurve_sample_ds(float ds) {this->scurve_sample_ds_ = ds;}

  //: Set base name to save intermediate data
  void set_base_name(const vcl_string& base_name) { this->base_name_ = base_name;}


  //: Average xgraph
  dbsksp_xshock_graph_sptr average_xgraph() const {return this->average_xgraph_;}

  ////: Computed distance to parent shapes
  //void get_distance_parents_to_average(vcl_vector<double >& distance_parents_to_average) const
  //{
  //  distance_parents_to_average = this->distance_parent_to_avg_;
  //}

  //void get_deform_cost_parents_to_average(vcl_vector<double >& deform_cost_parents_to_average) const
  //{
  //  deform_cost_parents_to_average = this->deform_cost_parent_to_avg_;
  //}



  //: Flag to print intermediate results
  bool debugging() const {return this->base_name_ != ""; }

  //: Print debug info to a file
  void print_debug_info(const vcl_string& str) const;
  

  // UTILITIES------------------------------------------------------------------

  //: Execute the averaging function
  bool compute();
  
protected:
  //: Initialize distances
  void init();

  //: Compute distance between two xgraphs - keeping the intermediate work
  double compute_edit_distance(const dbsksp_xshock_graph_sptr& xgraph1, 
    const dbsksp_xshock_graph_sptr& xgraph2,
    dbsksp_edit_distance& work);

  
  //: Compute distance between two xgraphs - keeping the intermediate work
  double compute_edit_distance(const dbsksp_xshock_directed_tree_sptr& tree1,
                      const dbsksp_xshock_directed_tree_sptr& tree2,
                      dbsksp_edit_distance& work);

  //: Update average distance from parent xgraphs to model xgraph
  void update_avg_distance();

  // Member variables
protected:
  
  // User-input-----------------------------------------------------------------

  //: The N parent xgraphs
  vcl_vector<dbsksp_xshock_graph_sptr > parent_xgraphs_;
  vcl_vector<double > parent_weights_;


  dbsksp_xshock_graph_sptr init_model_xgraph_;

  
  //: sampling params
  float scurve_sample_ds_;

  //: weight param
  float scurve_matching_R_;

  //: Stopping criteria for gradient-descent optimization
  // Algo stops when relative increment is smaller than relative_distance_;
  float distance_ratio_tol_;

  // base filename to save relevant data (mostly for debugging purpose)
  vcl_string base_name_;

  
  // Intermediate results-------------------------------------------------------

  int num_parents_;

  //: Tree constructed from the parent xgraph
  vcl_vector<dbsksp_xshock_directed_tree_sptr > parent_trees_;

  //: Model xgraph to be optimized
  dbsksp_xshock_graph_sptr model_xgraph_;
  dbsksp_xshock_directed_tree_sptr model_tree_;

  unsigned model_root_vid_;
  unsigned model_pseudo_parent_eid_;


  vcl_vector<vcl_vector<pathtable_key > > dart_corr_parent_to_model_;
  vcl_vector<double > parent_contract_and_splice_cost_;


  // Ouput----------------------------------------------------------------------

  //: Average xgraph
  dbsksp_xshock_graph_sptr average_xgraph_;

  //: Distance the two original xgraph
  vcl_vector<double > distance_parent_to_avg_;
  vcl_vector<double > deform_cost_parent_to_avg_;

  double init_avg_distance_;
  double cur_avg_distance_;
};

#endif // dbsksp_avg_n_xgraphs_grad_descent_h_

