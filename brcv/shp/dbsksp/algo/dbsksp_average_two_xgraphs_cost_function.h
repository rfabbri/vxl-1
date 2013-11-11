// This is shp/dbsksp/algo/dbsksp_average_two_xgraphs_cost_function.h
#ifndef dbsksp_average_two_xgraphs_cost_function_h_
#define dbsksp_average_two_xgraphs_cost_function_h_

//:
// \file
// \brief Cost function to compute average of two shock graphs
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Mar 3, 2010
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsksp/algo/dbsksp_xgraph_model.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree_sptr.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>
#include <dbsksp/dbsksp_xshock_fragment.h>

#include <vnl/vnl_cost_function.h>
#include <vcl_map.h>
#include <vcl_vector.h>


//==============================================================================
// dbsksp_average_two_xgraphs_cost_function
//==============================================================================


//: Cost function for a hypothesized set of A12 shock nodes
class dbsksp_average_two_xgraphs_cost_function : public vnl_cost_function
{
public:
  //: Constructor
  dbsksp_average_two_xgraphs_cost_function(const dbsksp_xshock_directed_tree_sptr& tree1, // original tree1
    const dbsksp_xshock_directed_tree_sptr& tree2, // original tree2
    const dbsksp_xshock_directed_tree_sptr& model_tree, // model tree, to be optimized
    const vcl_vector<pathtable_key >& correspondence1, // correspondence between tree1 and model tree
    const vcl_vector<pathtable_key >& correspondence2, // correspondence between tree2 and model tree
    float scurve_matching_R, // parameter R in shock matching
    dbsksp_xgraph_model_using_L_alpha_phi_radius* const xgraph_model // a model to modify the xgraph inside model_tree
    );

  //: Destructor
  virtual ~dbsksp_average_two_xgraphs_cost_function(){};

  //: Set edge correspondence
  void set_edge_correspondence(const vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr >& edge_map_xgraph1_to_model,
    const vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr >& edge_map_model_to_xgraph1,
    const vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr >& edge_map_xgraph2_to_model,
    const vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr >& edge_map_model_to_xgraph2)
  {
    this->edge_map_parent_to_model_[0] = edge_map_xgraph1_to_model;
    this->edge_map_model_to_parent_[0] = edge_map_model_to_xgraph1;
    this->edge_map_parent_to_model_[1] = edge_map_xgraph2_to_model;
    this->edge_map_model_to_parent_[1] = edge_map_model_to_xgraph2;
  }

  //: Main function
  virtual double f(const vnl_vector<double >& x);


  //: Compute cache data for min and max of kdiff
  // Assumption: the two original graphs have exact same topology
  void compute_kdiff_min_max();


protected:
  //: Compute kdiff cost for a fragment, given the id of the edge
  double compute_kdiff_cost(unsigned eid, const dbsksp_xshock_fragment& xfrag);



  // Original two xgraphs
  dbsksp_xshock_directed_tree_sptr parent_tree_[2];
  dbsksp_xshock_directed_tree_sptr model_tree_; // model tree, to be optimized

  // correspondence between parent tree and the model tree
  vcl_vector<pathtable_key > dart_corr_parent_to_model_[2]; // correspondence between tree1 and model tree

  float scurve_matching_R_; // parameter R in scurve matching
  dbsksp_xgraph_model_using_L_alpha_phi_radius* xgraph_model_; // a model to modify the xgraph inside model_tree

  // mapping edges between the parent and the model xgraphs
  vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr > edge_map_parent_to_model_[2];
  vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr > edge_map_model_to_parent_[2];
  
  // cache data for properties of the two original graphs
  struct xfrag_properties
  {
    double kdiff_min;
    double kdiff_max;
  };
  vcl_map<unsigned, xfrag_properties> map_model_xfrag_properties_; 
};



#endif // shp/dbsksp/algo/dbsksp_average_two_xgraphs_cost_function.h









