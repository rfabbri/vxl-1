// This is shp/dbsksp/algo/dbsksp_weighted_average_two_xgraphs_cost_function.h
#ifndef dbsksp_weighted_average_two_xgraphs_cost_function_h_
#define dbsksp_weighted_average_two_xgraphs_cost_function_h_

//:
// \file
// \brief Cost function to compute average of two shock graphs
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Mar 13, 2010
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
// dbsksp_weighted_average_two_xgraphs_cost_function
//==============================================================================


//: Base class for cost function to compute average of two xgraphs
class dbsksp_weighted_average_two_xgraphs_cost_base
{
public:
  //: Constructor
  dbsksp_weighted_average_two_xgraphs_cost_base(
    const dbsksp_xshock_directed_tree_sptr& tree1,      // original tree1
    const dbsksp_xshock_directed_tree_sptr& tree2,      // original tree2
    const dbsksp_xshock_directed_tree_sptr& model_tree, // model tree, to be optimized
    const vcl_vector<pathtable_key >& correspondence1,  // correspondence between tree1 and model tree
    const vcl_vector<pathtable_key >& correspondence2,  // correspondence between tree2 and model tree
    float scurve_matching_R,                            // parameter R in shock matching
    dbsksp_xgraph_model_using_L_alpha_phi_radius* const xgraph_model // a model to modify the xgraph inside model_tree
    );

  //: Destructor
  virtual ~dbsksp_weighted_average_two_xgraphs_cost_base(){};

  // Utilities------------------------------------------------------------------

  //: Compute cache data for min and max of kdiff
  void compute_kdiff_min_max();

protected:
  //: Customized data type for storing properties of two original graphs
  struct xfrag_properties
  {
    double kdiff_min;
    double kdiff_max;
  };

  //: Compute kdiff cost for a fragment, given the id of the edge
  double compute_kdiff_cost(unsigned eid, const dbsksp_xshock_fragment& xfrag);

  //: Compute max-curvature cost - penalize boundary curvature higher than max
  // curvature dictated by radius at the node
  double compute_kmax_cost(unsigned eid, const dbsksp_xshock_fragment& xfrag);

  // Member variables------------------------------------------------------------------

  //: Parent xgraphs
  dbsksp_xshock_directed_tree_sptr parent_tree_[2];

  //: Model xgraph to optimize
  dbsksp_xshock_directed_tree_sptr model_tree_; // model tree, to be optimized

  //: Correspondence between parent tree and the model tree
  vcl_vector<pathtable_key > dart_corr_parent_to_model_[2]; // correspondence between tree1 and model tree

  //: Parameter R in shock matching
  float scurve_matching_R_;

  //: Model to generate the xgraph
  dbsksp_xgraph_model_using_L_alpha_phi_radius* xgraph_model_; // a model to modify the xgraph inside model_tree

  //: List of properties the model should obey
  vcl_map<unsigned, xfrag_properties> map_model_xfrag_properties_; 
};

#endif // shp/dbsksp/algo/dbsksp_weighted_average_two_xgraphs_cost_function.h









