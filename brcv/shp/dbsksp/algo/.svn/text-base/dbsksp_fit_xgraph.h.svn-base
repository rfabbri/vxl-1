// This is shp/dbsksp/algo/dbsksp_fit_xgraph.h
#ifndef dbsksp_fit_xgraph_h_
#define dbsksp_fit_xgraph_h_

//:
// \file
// \brief A class to fit a generative shock graph to an extrinsic shock graph detection
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Oct 28, 2009
//
// \verbatim
//  Modifications
// \endverbatim

//
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>

#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_xshock_edge_sptr.h>


#include <dbgl/algo/dbgl_circ_arc.h>

#include <vnl/vnl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>


//==============================================================================
// dbsksp_fit_xgraph
//==============================================================================

//: A class to fit an extrinsic generative shock graph (dbsksp_xshock_graph)
// to a detected extrinsic shock graph (dbsk2d_shock_graph)
class dbsksp_fit_xgraph
{
public:
  //: Constructor
  dbsksp_fit_xgraph(double distance_rms_error_threshold = 1):
      distance_rms_error_threshold_(distance_rms_error_threshold)
  {}

  //: Destructor
  virtual ~dbsksp_fit_xgraph(){};

  // Main functions-------------------------------------------------------------

  //: Fit a generative dbsksp_xshock_graph to an extrinsic dbsk2d_shock_graph
  dbsksp_xshock_graph_sptr fit_to(const dbsk2d_shock_graph_sptr& graph);


  //: Fit a generative dbsksp_xshock_graph
  void fit_to(const dbsksp_xshock_graph_sptr& old_xgraph,
    dbsksp_xshock_graph_sptr& new_xgraph,
    vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr >& map_new_node_to_old_node,
    double sample_ds = 1);



  

  // Support functions----------------------------------------------------------
protected:

  typedef vcl_map<dbsksp_xshock_edge_sptr, vcl_vector<dbsk2d_xshock_edge_sptr > > type_map_sksp_edge_to_sk2d_edges;
  typedef vcl_map<dbsksp_xshock_node_sptr, dbsk2d_shock_node_sptr > type_map_sksp_node_to_sk2d_node;
  typedef vcl_pair<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr> type_branch_key;
  typedef vcl_vector<dbsksp_xshock_edge_sptr > type_branch_edges;

  //: Convert the coarse shock graph, the topology is exact the same as in the sk2d shock graph
  // Depreciated
  dbsksp_xshock_graph_sptr convert_coarse_xgraph(const dbsk2d_shock_graph_sptr& graph,
    type_map_sksp_edge_to_sk2d_edges& map_xe_to_orig_edges,
    type_map_sksp_node_to_sk2d_node& map_sksp_node_to_sk2d_node);


  //: Convert a (coarse) extrinsic dbsk2d shock graph to a dbsksp_shock_graph by
  // using an Euler tour to explore the dbsks2d graph and construct new nodes and edges
  // as they are discovered.
  // If "keep_degree_2_nodes = false", all A12 nodes in the original dbsk2d shock graph will be ignored.
  // Only terminal nodes and junctions will be retained.
  // "map_xv_to_orig_node" maps each node in the new graph to a node in the original dbsk2d graph
  // "map_xe_to_orig_edges" maps each edge in the new graph to a list of edges
  // in the original dbsk2d graph that it is constructed from.
  dbsksp_xshock_graph_sptr convert_coarse_xgraph_using_euler_tour(const dbsk2d_shock_graph_sptr& graph,
    bool keep_degree_2_nodes,
    type_map_sksp_node_to_sk2d_node& map_xv_to_orig_node,
    type_map_sksp_edge_to_sk2d_edges& map_xe_to_orig_edges);

  //: Determine radius and position of a graph's shock nodes from their descriptors
  // Use averaging if there are differences among the descriptors
  void compute_node_radius_and_position_from_descriptors(const dbsksp_xshock_graph_sptr& xgraph);

  //: Iron out the differences in shock tangent and phi angles between the 
  // descriptors around the shock nodes in an xgraph
  // Add an A_infty branch when there gaps are too big.
  void make_consistent_node_tangent_and_phi(const dbsksp_xshock_graph_sptr& xgraph,
    double min_angle_gap_to_add_Ainfty_branch = 0.01);

  //: Compute the list of shock branches in a undirected shock graph
  void compute_shock_branches(const dbsksp_xshock_graph_sptr& xgraph,
    vcl_vector<type_branch_key >& list_branch_key,
    vcl_vector<type_branch_edges >& list_branch_edges);

  //: Comparison function for keys of shock branches
  // Two keys are the same if the two end nodes are the same, regardless of their order
  struct compare_unordered_node_pair
  {
    bool operator()(type_branch_key key1, type_branch_key key2) const;
  };

  
  // Member variables-----------------------------------------------------------

protected:
  double distance_rms_error_threshold_;

  // Intermediate variables
  

};



#endif // shp/dbsksp/dbsksp_fit_xgraph.h









