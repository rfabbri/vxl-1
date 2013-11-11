// This is brcv/shp/dbsksp/algo/dbsksp_edit_distance.h

#ifndef dbsksp_edit_distance_h_
#define dbsksp_edit_distance_h_


//:
// \file
// \brief Edit distance algorithm applied on dbsksp_xshock_graph  
//
// \author Nhon H. Trinh (ntrinh@lems.brown.edu)
// \date Nov 4, 2009
// \verbatim
//   Modifications
//     Nhon Trinh     Nov 4, 2009     Initial version
//
// \endverbatim
//


#include <dbskr/dbskr_edit_distance_base.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>

//==============================================================================
// dbsksp_edit_distance
//==============================================================================

//: A class to compute the minimum edit distance between two shock graphs (trees)
class dbsksp_edit_distance : public dbskr_edit_distance_base
{
public:
  // Constructors / Destructors--------------------------------------------------

  //: Constructor
  dbsksp_edit_distance();

  //: Destructor
  virtual ~dbsksp_edit_distance() {}

  // Data access----------------------------------------------------------------

  //: Set tree 1
  void set_tree1(const dbsksp_xshock_directed_tree_sptr& tree1);
  
  //: Get tree1
  dbsksp_xshock_directed_tree_sptr tree1() const;

  //: Set tree 2
  void set_tree2(const dbsksp_xshock_directed_tree_sptr& tree2);

  //: Get tree2
  dbsksp_xshock_directed_tree_sptr tree2() const;

  //: Return curve_matching_R parameter
  float scurve_matching_R() const
  { return this->scurve_matching_R_; }

  //: Set curve matching R
  void set_scurve_matching_R(float R)
  { this->scurve_matching_R_ = R; }


  //: set all params
  void set(const dbsksp_xshock_graph_sptr& xgraph1,
    const dbsksp_xshock_graph_sptr& xgraph2,
    float scurve_matching_R,
    float scurve_sample_ds);



  // UTILITIES------------------------------------------------------------------

  //: Construct a tree from an xgraph
  static dbsksp_xshock_directed_tree_sptr new_tree(const dbsksp_xshock_graph_sptr& xgraph,
    float scurve_matching_R,
    float scurve_sample_ds);

  // Get xgraph1 trimmed to match the common (coarse) topology
  // This tree can only computed after (optimal) edit distance has been computed
  dbsksp_xshock_graph_sptr edit_xgraph1_to_common_topology() const;

  // Get xgraph2 trimmed to match the common (coarse) topology
  // This tree can only computed after (optimal) edit distance has been computed
  dbsksp_xshock_graph_sptr edit_xgraph2_to_common_topology() const;


  // SHOULD MOVE TO dbskr_edit_distance_base
  
  //: Get final correspondence after computing optimal edit distance
  // Return a list of pathtable_key, which maps a pair of darts in tree1 to a 
  // a pair of darts in tree2. Each pair of darts, in return, represents a path
  // in the trees.
  void get_final_correspondence(vcl_vector<pathtable_key >& final_correspondence);

  //: Compute sum of deformation costs
  float get_deform_cost(const vcl_vector<pathtable_key >& correspondence);

  //: Compute deformation cost for a path correspondence
  float get_deform_cost(const pathtable_key& key);



protected:

  //: Trim either of the source tree to the common topology shared between the two
  // if `use_tree1' is false, tree2 is used as the original tree.
  // \note The current method does NOT work if there is a contract operation in order
  // to go from the original tree (tree1 or tree2) to the common graph. This will
  // result in a disconnected tree (per current method)
  // \todo Take care of contract operation.
  dbsksp_xshock_graph_sptr trim_source_tree_to_common_topology(bool use_tree1) const;

  //: Apply splice edits (after edit distance have been computed) on source tree
  // if `use_tree1' is false, tree2 is used as the original tree.
  dbsksp_xshock_graph_sptr apply_optimal_splice_edits(bool use_tree1) const;

  //: find and cache the cost of matching curve pair induced by given dart paths
  virtual float get_cost(int td1, int d1, int td2, int d2);
  
  // Member variables-----------------------------------------------------------
protected:
  float scurve_matching_R_;

};

#endif // dbsksp_edit_distance_h_

