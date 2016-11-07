// This is brcv/dbsksp/algo/dbsksp_xshock_directed_tree.h

#ifndef dbsksp_xshock_directed_tree_h_
#define dbsksp_xshock_directed_tree_h_

//:
// \file 
// \brief A directed tree constructed from a dbsksp_shock_graph to use in 
//        tree-edit-distance algorithm 
// \author Nhon H. Trinh (ntrinh@lems.brown.edu)
//
// \date Nov 4, 2009
//
// \verbatim
// Modifications
//  Nhon Trinh      Nov 4, 2009    Initial version
//
// \endverbatim

#include <dbskr/dbskr_directed_tree.h>

#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>


//==============================================================================
// dbsksp_xshock_directed_tree
//==============================================================================


//: A directed tree built from a dbsk2d_shock_graph
class dbsksp_xshock_directed_tree : public dbskr_directed_tree
{
public:
  // Constructors / Destructors / Initializers----------------------------------
  
  //: Constructor
  dbsksp_xshock_directed_tree();

  //: Destructor;
  virtual ~dbsksp_xshock_directed_tree() 
  { this->clear(); }

  //: acquire tree from the given shock graph
  bool acquire(const dbsksp_xshock_graph_sptr& xgraph);

  
  // Data access----------------------------------------------------------------

  //: return the smart pointer to the underlying shock graph
  dbsksp_xshock_graph_sptr xgraph() const
  { return this->xgraph_; }

  //: Set shock grpah
  void set_xgraph(const dbsksp_xshock_graph_sptr& new_xgraph)
  { this->xgraph_ = new_xgraph; }


  //: sampling ds for scurve
  float scurve_sample_ds() const
  { return this->scurve_sample_ds_; }

  //: set sample ds
  void set_scurve_sample_ds(float ds)
  { this->scurve_sample_ds_ = ds; }

  //: return curve matching parameter R
  float scurve_matching_R() const
  {return this->scurve_matching_R_; }

  //: set parameter R for matching scurve
  void set_scurve_matching_R(float R)
  { this->scurve_matching_R_ = R;} 


  // Graph-related--------------------------------------------------------------

  

  // used in get_scurve(...)
  //: return a vector of pointers to the edges in underlying shock graph for the given dart list
  void get_edge_list(const vcl_vector<int>& dart_list,  
                 dbsksp_xshock_node_sptr& start_node,  
                 vcl_vector<dbsksp_xshock_edge_sptr>& path);

  ////: get the list of shock edges that are part of a particular dart
  //void get_edge_list(int dart_id, vcl_vector<dbsksp_xshock_edge_sptr>& path)
  //{ path = shock_edges_[dart_id]; return; }

  // Utilities------------------------------------------------------------------

  //: acquire tree topology from a shock graph
  bool acquire_tree_topology(const dbsksp_xshock_graph_sptr& xgraph);

  //: Compute delete and contract costs for individual dart. Only call this function
  // after acquire_tree_topology has been called and succeeded.
  void compute_delete_and_contract_costs();

  //: find and cache the shock curve for this pair of darts, if not already cached
  virtual dbskr_scurve_sptr get_curve(int start_dart, int end_dart, bool construst_circular_ends);

  //: returns both the coarse and dense version of shock curve
  virtual dbskr_sc_pair_sptr get_curve_pair(int start_dart, int end_dart, bool construct_circular_ends);


  //: Clear the cache of scurve's for given pairs of darts
  void clear_dart_path_scurve_map()
  {
    this->dart_path_scurve_map_.clear();
  }


  // File I/O-------------------------------------------------------------------




protected:
  // Reset / Initalize /////////////////////////////////////////////////////////

  //: Reset all member variables to default
  void clear(); 

  
protected:
  
  // Member variables-----------------------------------------------------------
  
  // Shock-graph related //////////////////////////////////////

  // The shock graph this tree is constructed from
  dbsksp_xshock_graph_sptr xgraph_;

  //: each dart has a list of pointers to the actual edges 
  //  on the corresponding shock branch of the graph (in the correct order)
  vcl_vector<vcl_vector<dbsksp_xshock_edge_sptr> > shock_edges_;

  //: each dart also has a list of start nodes for the underlying edge to determine direction 
  vcl_vector<dbsksp_xshock_node_sptr> starting_nodes_;


  // Cache data////////////////////////////////////////////////////////////

  //: cache the shock curves for future use for each path of darts
  //vcl_map<vcl_pair<int, int>, dbskr_scurve_sptr> dart_path_scurve_map_;
  vcl_map<vcl_pair<int, int>, dbskr_sc_pair_sptr> dart_path_scurve_map_;


  // Cost-related parameters /////////////////////////////////////////////
  // For now only, may change later

  
  // in get_curve we call the curve computation like this:
  // dbskr_compute_scurve(start_node, edges, leaf_[end_dart], true, true, vcl_min(scurve_sample_ds_, interpolate_ds_), scurve_sample_ds_);
  // hence the interpolate_ds is passed as given by interpolate_ds_ as long as scurve_sample_ds_ (i.e. subsample_ds) >= interpolate_ds_
  float scurve_sample_ds_;
  float interpolate_ds_;  // made this into a parameter as well, default is 0.5
  bool elastic_splice_cost_;
  float scurve_matching_R_;
};

#endif // dbsksp_xshock_directed_tree_h_
