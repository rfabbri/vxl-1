// This is shp/dbsksp/algo/dbsksp_build_from_xshock_graph.h
#ifndef dbsksp_build_from_xshock_graph_h_
#define dbsksp_build_from_xshock_graph_h_

//:
// \file
// \brief Construct a generative shock graph from an extrinsic shock graph (xshock)
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date April 7, 2007
//
// \verbatim
//  Modifications
// \endverbatim



#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <vcl_map.h>

#include <dbskr/dbskr_scurve_sptr.h>


// ============================================================================
// dbsksp_build_from_xshock_graph
// ============================================================================

//: A class to build a shock graph
class dbsksp_build_from_xshock_graph
{
public:

  // --------------------------------------------------------------------------
  // CONSTRUCTORS / DESTRUCTORS
  // --------------------------------------------------------------------------

  //: Constructor
  dbsksp_build_from_xshock_graph(double scurve_sample_ds = 1, 
    double shock_interp_sample_ds = 5): 
      scurve_sample_ds_(scurve_sample_ds),
      shock_interp_sample_ds_(shock_interp_sample_ds){};
  
  //: Destructor
  virtual ~dbsksp_build_from_xshock_graph(){};

  // --------------------------------------------------------------------------
  // DATA ACCESS
  // --------------------------------------------------------------------------

  //: Get and set the shock matching tree
  dbskr_tree_sptr skr_tree() const { return this->skr_tree_; }
  void set_skr_tree(const dbskr_tree_sptr& tree) {this->skr_tree_ = tree; }
  
  //: Get and set the generative shock graph
  dbsksp_shock_graph_sptr gshock() const {return this->gshock_; }
  void set_gshock(const dbsksp_shock_graph_sptr& s){ this->gshock_= s; }

  //: Get and set the extrinsic shock graph
  dbsk2d_shock_graph_sptr xshock() const {return this->xshock_; }
  void set_xshock(const dbsk2d_shock_graph_sptr& xshock) 
  { this->xshock_ = xshock; }

  double scurve_sample_ds() const {return this->scurve_sample_ds_; }
  void set_scurve_sample_ds(double ds) {this->scurve_sample_ds_ = ds; }

  double shock_interp_sample_ds() const {return this->shock_interp_sample_ds_; }
  void set_shock_interp_sample_ds(double ds) {this->shock_interp_sample_ds_ = ds; }

  // --------------------------------------------------------------------------
  // UTILITIES
  // --------------------------------------------------------------------------

  //: Method to build from extrinsic shock graph
  dbsksp_shock_graph_sptr build_from_xshock_graph_using_skr_tree(
    const dbsk2d_shock_graph_sptr& xshock);

  ////: Method to build from a shock matching tree
  //dbsksp_shock_graph_sptr build_from_skr_tree(
  //  const dbskr_tree_sptr& tree);

  ////: Convert a shock curve, typically from a dart, into a sequence of nodes
  //// and edges in the generative shock graph
  //void interpolate_shock_curve(const dbskr_scurve_sptr& sc, 
  //  const dbsksp_shock_node_sptr& start_node,
  //  const dbsksp_shock_node_sptr& end_node,
  //  vcl_vector<dbsksp_shock_node_sptr >& ordered_nodes,
  //  vcl_vector<dbsksp_shock_edge_sptr >& ordered_edges);

  //: Interpolate an order list of xnodes with a sequence of shock edges
  void interpolate_xnodes(
     const vcl_vector<dbsksp_xshock_node_descriptor >& xshock_node_list,
     const dbsksp_shock_node_sptr& start_node,
     const dbsksp_shock_node_sptr& end_node,
     vcl_vector<dbsksp_shock_node_sptr >& ordered_nodes,
     vcl_vector<dbsksp_shock_edge_sptr >& ordered_edges);


  // =======================================================================

  //: Method to build from a shock matching tree and the number of segments
  // corresponding to each dart
  // Due to interpolation, each segment in the shock matching tree will 
  // results in two shock edges
  dbsksp_shock_graph_sptr build_from_skr_tree(
    const dbskr_tree_sptr& tree,
    const vcl_vector<int >& num_segments);

  

  //: Convert a shock curve, typically from a dart, consisting of ``num_segments''
  // into a sequence nodes and edges in the generative shock graph
  void interpolate_shock_curve(const dbskr_scurve_sptr& sc, 
    const dbsksp_shock_node_sptr& start_node,
    const dbsksp_shock_node_sptr& end_node,
    int num_segments,
    vcl_vector<dbsksp_shock_node_sptr >& ordered_nodes,
    vcl_vector<dbsksp_shock_edge_sptr >& ordered_edges);


  //: Compute number of segments for each dart in the tree given a sampling rate
  static void compute_num_segments_for_darts(const dbskr_tree_sptr& tree,
    double segment_ds, 
    vcl_vector<int >& num_segments);







  //: Close the boundary at degree-one nodes
  void close_boundary_at_degree_one_nodes();


  //: Retrieve the list of edges corresponding to a dart
  vcl_vector<dbsksp_shock_edge_sptr > get_shock_edges_of_dart(int dart);

  
  //: Retrieve the list of edges corresponding to a dart
  vcl_vector<dbsksp_shock_node_sptr > get_shock_nodes_of_dart(int dart);

  // for debugging purpose only
  vcl_vector<dbsksp_shapelet_sptr > shapelets;


protected:
  
  dbskr_tree_sptr skr_tree_; // shock matching tree
  dbsk2d_shock_graph_sptr xshock_; // extrinsic shock graph
  dbsksp_shock_graph_sptr gshock_; // generative shock graph

  double scurve_sample_ds_; // sampling rate of the scurve
  double shock_interp_sample_ds_; // sampling rate to interpolate the scurves

  // keeping track of the nodes of the coarse graph
  // map<coarse_node_id, node_sptr >
  vcl_map<int, dbsksp_shock_node_sptr > coarse_graph_nodes_map_;

  // nodes of the shock graph being constructed
  vcl_map<int, dbsksp_shock_node_sptr > nodes_map_;

  // edges of the shock graph being constructed
  vcl_map<int, dbsksp_shock_edge_sptr > edges_map_;

  // mapping from the darts of the tree to their correponding shock edges and nodes
  // map<dart_id, ordered list of shock edges>
  vcl_map<int, vcl_vector<dbsksp_shock_edge_sptr > > dart_to_edges_map_;
  // map<dart_id, ordered list of nodes >
  vcl_map<int, vcl_vector<dbsksp_shock_node_sptr > > dart_to_nodes_map_;

  
  
};




#endif // shp/dbsksp/dbsksp_build_from_xshock_graph.h









