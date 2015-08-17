// This is brcv/rec/dbskr/dbskr_tree.h

#ifndef dbskr_tree_h_
#define dbskr_tree_h_

//:
// \file
// \brief An ordered directed tree class which is used by edit distance algorithm
//        A shock graph should be converted into an "ordered tree"
//        via this class
// \author
//  O.C. Ozcanli - August 05, 2005
// \verbatim
// Modifications
//  O.C. Ozcanli - September 20, 2005    Enlarged the class to be a "Tree-fitted Shock Graph"
//                                       Degree 2 nodes of the shock graph are merged and
//                                       splice and branch costs are computed and cached
//                                       i.e. we get rid of .shg file reading part
//                                            we read the graph directly from an esf file
//                                            and prepare tree to be used in edit distance
//
//  Amir Tamrakar  -11/04/05             Added an extra member to define how finely the shock
//                                       the shock curves should be sampled from tree branch
//
//  Ozge C Ozcanli March 29, 07          Removed the scurve pair class and all related functionality
//                                       The dense one is almost always same as the coarse one
//                                       and the dense one is never used
//
//  Ozge C Ozcanli October 30, 07        Re-added scurve pair class, it is used when localized matching option is on in dbskr_tree_edit
//   
//  Nhon Trinh -   Oct 31, 2009          Separate the class into two layers - the parent layer does not depend on dbsk2d_shock_graph
//                                       The child layer contains info related to dbsk2d_shock_graph
// \endverbatim



// Unrooted Tree Edit Distance Algorithm will work with instances of this class


#include <dbskr/dbskr_directed_tree.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskr/dbskr_sc_pair_sptr.h>
#include <dbskr/dbskr_tree_sptr.h>


#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/dbskr_sc_pair.h>

#include <vsol/vsol_polygon_2d_sptr.h>

#include <vgl/vgl_polygon.h>

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_array_2d.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_utility.h>
#include <assert.h>




//: A directed tree built from a dbsk2d_shock_graph
class dbskr_tree : public dbskr_directed_tree
{
public:
  // Constructors / Destructors / Initializers----------------------------------
  
  //: Constructor
  dbskr_tree(float scurve_sample_ds=5.0f, float interpolate_ds=1.0f, float matching_R=6.0f);

  //: Constructor
  dbskr_tree(dbsk2d_shock_graph_sptr sg,
             bool mirror=false,
             double width=0.0,
             float scurve_sample_ds=5.0f, 
             float interpolate_ds=1.0f, 
             float matching_R=6.0f);

  //: Destructor;
  virtual ~dbskr_tree() 
  { this->clear(); }

  //: acquire tree from the given shock graph
  bool acquire(dbsk2d_shock_graph_sptr sg, bool elastic_splice_cost, bool construct_circular_ends, bool dpmatch_combined);


  //: acquire tree from the given input file
  bool acquire(vcl_string filename);

  //: acquire tree topology assumes sg set
  bool acquire_tree_topology();

  //: acquire tree from the given shock graph and prune the subtrees below the threshold
  bool acquire_and_prune(dbsk2d_shock_graph_sptr sg, double pruning_threshold, 
    bool elastic_splice_cost, bool construct_circular_ends, bool dpmatch_combined);

  //: compute delete contract costs
  void compute_delete_and_contract_costs(bool elastic_splice_cost,
                                         bool construct_circular_ends,
                                         bool dpmatch_combined);

  // Data access----------------------------------------------------------------

  //: return the smart pointer to the underlying shock graph
  dbsk2d_shock_graph_sptr get_shock_graph() 
  { return sg_; }


  //: Return scurve_sample_ds
  float scurve_sample_ds() const
  { return this->scurve_sample_ds_; }

  //: Return interpolate_ds
  float interpolate_ds() const
  { return this->interpolate_ds_; }


  //: Return true if using elastic_splice_cost
  bool elastic_splice_cost() const
  { return this->elastic_splice_cost_; }

  //: Returns whether has been mirrored
  bool mirror() const
  {return this->mirror_; }

  //: set area
  void set_area(double area){area_=area;}

  // get area
  double get_area(){return area_;}


  void set_R(double R){scurve_matching_R=R;}

  void set_ds(float scurve_sample_ds){scurve_sample_ds_=scurve_sample_ds;}

  void set_scale_ratio(double scale_ratio){scale_ratio_=scale_ratio;}

  void clear_scurve_cache(){  dart_path_scurve_map_.clear();};

  double get_scale_ratio(){return scale_ratio_;}

  double get_width(){return width_;}

  // Graph-related--------------------------------------------------------------

  

  // used in get_scurve(...)
  //: return a vector of pointers to the edges in underlying shock graph for the given dart list
  void edge_list(const vcl_vector<int>& dart_list,  
                 dbsk2d_shock_node_sptr& start_node,  
                 vcl_vector<dbsk2d_shock_edge_sptr>& path);

  //: get the list of shock edges that are part of a particular dart
  vcl_vector<dbsk2d_shock_edge_sptr>& get_shock_edges(int dart_id)
  { return shock_edges_[dart_id]; }


  // Geomtry--------------------------------------------------------------------

  
  //: return the total length of the reconstructed boundary with this tree
  //  (trees from the same shock graph may return different values since they may be constructed with different parameters,
  //   e.g. with our without circular completions at the leaves, etc.)
  float total_reconstructed_boundary_length() 
  { return total_reconstructed_boundary_length_; }

  //: uses the already existing scurves, so if circular_ends = true while acquiring 
  // the tree then the outline will have circular completions
  vsol_polygon_2d_sptr compute_reconstructed_boundary_polygon(bool construct_circular_ends);


  // Utilities------------------------------------------------------------------

  //: find and cache the shock curve for this pair of darts, if not already cached
  virtual dbskr_scurve_sptr get_curve(int start_dart, int end_dart, bool construst_circular_ends);

  //: returns both the coarse and dense version of shock curve
  virtual dbskr_sc_pair_sptr get_curve_pair(int start_dart, int end_dart, bool construct_circular_ends);

  // File I/O-------------------------------------------------------------------

  //Amir added this function for debug
  //: create and write .shg file to debug splice and contract costs
  bool create_shg(vcl_string fname);

  //Amir added this function for debug
  //: create and write .shgesg file to hold the correspondences between shock node ids and tree node ids
  bool create_shgesg(vcl_string fname);



protected:
  // Reset / Initalize /////////////////////////////////////////////////////////

  //: Reset all member variables to default
  void clear(); 

  // Geometry //////////////////////////////////////////////////////////////////
  
  //: Compute total length of the shape boundary
  // If "construct_circular_ends" is true, each branch is closed with a circular arc
  // (instead of being left open).
  float compute_total_reconstructed_boundary_length(bool construct_circular_ends);



  
  // Cost computation //////////////////////////////////////////////////////////
  
  
  
protected:
  
  // Member variables-----------------------------------------------------------
  
  // Shock-graph related ////////////////////////////////////////////////
  //: adding a smart pointer to the dbsk2d_shock_graph (extrinsic shock) instance to retain all information
  //  regarding input shock (.esf read/writes will be through this class)
  dbsk2d_shock_graph_sptr sg_;

  //: each dart has a list of pointers to the actual edges 
  //  on the corresponding shock branch of the graph (in the correct order)
  vcl_vector<vcl_vector<dbsk2d_shock_edge_sptr> > shock_edges_;

  //: each dart also has a list of start nodes for the underlying edge to determine direction 
  vcl_vector<dbsk2d_shock_node_sptr> starting_nodes_;

  //: cache the shock curves for future use for each path of darts
  //vcl_map<vcl_pair<int, int>, dbskr_scurve_sptr> dart_path_scurve_map_;
  vcl_map<vcl_pair<int, int>, dbskr_sc_pair_sptr> dart_path_scurve_map_;


  // Cost-related parameters /////////////////////////////////////////////

  float total_reconstructed_boundary_length_;

  
public:
  // this parameter corresponds to subsample_ds_ in dbskr_scurve, the default is 5.0
  // there is also interpolate_ds_ in dbskr_scurve for which the default was 1.0 but made to 0.5 by Amir Tamrakar
  // in get_curve we call the curve computation like this:
  // dbskr_compute_scurve(start_node, edges, leaf_[end_dart], true, true, vcl_min(scurve_sample_ds_, interpolate_ds_), scurve_sample_ds_);
  // hence the interpolate_ds is passed as given by interpolate_ds_ as long as scurve_sample_ds_ (i.e. subsample_ds) >= interpolate_ds_
  float scurve_sample_ds_;
  float interpolate_ds_;  // made this into a parameter as well, default is 0.5
  bool elastic_splice_cost_;
  float scurve_matching_R;
  bool mirror_;
  double scale_ratio_;
  double area_;
  double width_;
};

#endif // dbskr_tree_h_
