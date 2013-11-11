// This is brcv/shp/dbsksp/algo/dbsksp_average_two_xgraphs.h

#ifndef dbsksp_average_two_xgraphs_h_
#define dbsksp_average_two_xgraphs_h_


//:
// \file
// \brief A class to compute the average of two shock graphs
//
// \author Nhon H. Trinh (ntrinh@lems.brown.edu)
// \date Nov 12, 2009
// \verbatim
//   Modifications
//     Nhon Trinh     Nov 12, 2009     Initial version
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
// dbsksp_average_two_xgraphs
//==============================================================================

//: A class to compute an average of two shock graphs
class dbsksp_average_two_xgraphs
{
public:
  // Constructors / Destructors--------------------------------------------------

  //: Constructor
  dbsksp_average_two_xgraphs();


  //: Constructor
  dbsksp_average_two_xgraphs(const dbsksp_xshock_graph_sptr& xgraph1,
    const dbsksp_xshock_graph_sptr& xgraph2,
    float scurve_matching_R = 6.0f,
    float scurve_sample_ds = 3.0f,
    double relative_error_tol = 0.01,
    const vcl_string& debug_base_name = "");


  //: Destructor
  virtual ~dbsksp_average_two_xgraphs() {}

  // Data access----------------------------------------------------------------

  //: Return parent xgraph, index =0 or =1
  dbsksp_xshock_graph_sptr parent_xgraph(int index) const
  {
    return this->parent_xgraph_[index];
  }

  //: Set parent xgraph, index = 0 or = 1
  void set_parent_xgraph(int index, const dbsksp_xshock_graph_sptr& new_xgraph)
  {
    this->parent_xgraph_[index] = new_xgraph;
  }

  //: Return scurve_matching_R
  float scurve_matching_R() const
  { return this->scurve_matching_R_; }

  //: Set curve matching R
  void set_scurve_matching_R(float R)
  { this->scurve_matching_R_ = R; }

  //: Return sampling param
  float scurve_sample_ds() const
  {
    return this->scurve_sample_ds_;
  }

  //: Set sample ds
  void set_scurve_sample_ds(float ds)
  {
    this->scurve_sample_ds_ = ds;
  }

  //: Set tolerance for relative error (ratio err / original distance)
  void set_relative_error_tol(double tol)
  {
    this->relative_error_tol_ = tol;
  }


  //: Get distance between parent shapes
  double distance_btw_parents() const
  {
    return this->distance_btw_parents_;
  }
  
  //: Set distance between parent shapes
  void set_distance_btw_parents(double distance)
  {
    this->distance_btw_parents_ = distance;
  }


  //: Get distance to parent shapes
  double distance_to_parent(unsigned index) const
  {
    return this->distance_to_parent_[index];
  }


  //: Set distance from `this' average xgraph to parent xgraph
  void set_distance_to_parent(unsigned index, double distance)
  {
    this->distance_to_parent_[index] = distance;
  }

  //: Set base name to save intermediate data
  void set_base_name(const vcl_string& base_name)
  { this->base_name_ = base_name; }

  //: Flag to print intermediate results
  bool debugging() const
  {
    return this->base_name_ != "";
  }


  //: Print debug info to a file
  void print_debug_info(const vcl_string& str) const;



  //: Get the common xgraph (edited from the parent xgraph to shared graph topology)
  dbsksp_xshock_graph_sptr get_common_xgraph(int index) const;
  

  // UTILITIES------------------------------------------------------------------

  //: Execute the averaging function
  bool compute();
  
  //: Average xgraph
  dbsksp_xshock_graph_sptr average_xgraph() const
  {
    return this->average_xgraph_;
  }

protected:
  //: Initialize distances
  void init();

  //: Compute average of the trimmed versions of the two original xgraphs
  bool compute_average_of_common_xgraphs();

  //: Compute distance between two xgraphs - keeping the intermediate work
  double compute_edit_distance(const dbsksp_xshock_graph_sptr& xgraph1, 
    const dbsksp_xshock_graph_sptr& xgraph2,
    dbsksp_edit_distance& work);

  //: Compute distance between two xgraphs - keeping the intermediate work
  double compute_edit_distance(const dbsksp_xshock_directed_tree_sptr& tree1,
    const dbsksp_xshock_directed_tree_sptr& tree2,
    dbsksp_edit_distance& work);

  //: Resampled two xgraphs with common COARSE topology so that their fine 
  // topologies are also the same
  bool resample_to_match_fine_graph_topology(const dbsksp_xshock_graph_sptr& coarse_matching_xgraph1,
    const dbsksp_xshock_graph_sptr& coarse_matching_xgraph2,
    dbsksp_xshock_graph_sptr& fine_matching_xgraph1,
    dbsksp_xshock_graph_sptr& fine_matching_xgraph2);

  //: Compute vertex- and edge-correspondence given their dart correspondence
  // Assumption: the two trees have EXACT same topology
  bool compute_vertex_and_edge_correspondence(const dbsksp_xshock_directed_tree_sptr& tree1,
    const dbsksp_xshock_directed_tree_sptr& tree2,
    const vcl_vector<pathtable_key >& dart_correspondence_tree1_to_tree2,
    vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr >& node_map_xgraph1_to_xgraph2,
    vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr >& node_map_xgraph2_to_xgraph1,
    vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr >& edge_map_xgraph1_to_xgraph2,
    vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr >& edge_map_xgraph2_to_xgraph1);

  //: Compute root node and edge for a given xgraph tree
  bool compute_xgraph_root_node(const dbsksp_xshock_directed_tree_sptr& tree,
    dbsksp_xshock_node_sptr& root_node, dbsksp_xshock_edge_sptr& pseudo_parent_edge);

  //: Compute coarse Euler tour for an xgraph (ignore degree-2 nodes)
  bool compute_coarse_euler_tour(const dbsksp_xshock_graph_sptr& xgraph, 
    vcl_vector<dbsksp_xshock_node_sptr >& coarse_euler_tour) const;

  //: Update distance values between model and common_xgraph (topology-matching xgraph)
  bool update_relative_error();

  double relative_error() const
  { return this->relative_error_; }

  

protected:
  //: Compute deformation cost of between two trees
  double compute_deform_cost(const dbsksp_xshock_directed_tree_sptr& tree1,
    const dbsksp_xshock_directed_tree_sptr& tree2,
    const vcl_vector<pathtable_key >& correspondence);


  // Member variables
protected:
  
  // User-input-----------------------------------------------------------------

  //: The two parent xgraphs
  dbsksp_xshock_graph_sptr parent_xgraph_[2];

  //: sampling params
  float scurve_sample_ds_;

  //: weight param
  float scurve_matching_R_;

  //: Tolerance for relative error
  double relative_error_tol_;

  // Ouput----------------------------------------------------------------------

  //: Average xgraph
  dbsksp_xshock_graph_sptr average_xgraph_;

  //: Distance the two original xgraph
  double distance_to_parent_[2];

  //: Relative error (ratio error / distance between two parent shapes)
  double relative_error_;


  // Intermediate results-------------------------------------------------------

  // base filename to save relevant data (mostly for debugging purpose)
  vcl_string base_name_;


  //: Distance between the two original xgraphs
  double distance_btw_parents_;


  // edited version of the parent xgraphs to that they have the same graph topology
  dbsksp_xshock_directed_tree_sptr trimmed_tree_[2];
  dbsksp_xshock_directed_tree_sptr common_tree_[2];
  dbsksp_xshock_directed_tree_sptr model_tree_;
  unsigned model_root_vid_;
  unsigned model_pseudo_parent_eid_;

  // dart-correspondence between common_xgraph and model tree
  vcl_vector<pathtable_key > corr_common_xgraph_to_model_[2];

  double distance_common_xgraph_to_model_[2];
  double distance_parent_to_common_xgraph_[2];
  double distance_parent_to_trimmed_xgraph_[2];
  double distance_btw_common_xgraphs_;

  vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr > node_map_common_xgraph_to_model_[2];
  vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr > node_map_model_to_common_xgraph_[2];
  vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr > edge_map_common_xgraph_to_model_[2];
  vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr > edge_map_model_to_common_xgraph_[2];
};

#endif // dbsksp_average_two_xgraphs_h_

