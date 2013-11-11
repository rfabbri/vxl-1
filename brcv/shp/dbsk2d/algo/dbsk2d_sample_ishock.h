// This is brcv/shp/dbsk2d/dbsk2d_sample_ishock.h
#ifndef dbsk2d_sample_ishock_h_
#define dbsk2d_sample_ishock_h_
//:
// \file
// \brief Sample the shock graph and form an extrinsic shock graph
// \author Amir Tamrakar
// \date 06/20/05
// 
// This file contains the function to sample a pruned (coarse level)
// shock graph and output an extrinsic shock graph

// \verbatim
//  Modifications
//   Amir Tamrakar 06/20/2005    Initial version.
//
// \endverbatim 

#include <vcl_map.h>

#include <dbsk2d/dbsk2d_ishock_node.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>

#include <dbsk2d/dbsk2d_ishock_contact.h>             
#include <dbsk2d/dbsk2d_ishock_pointpoint.h>         
#include <dbsk2d/dbsk2d_ishock_pointline.h>          
#include <dbsk2d/dbsk2d_ishock_pointarc.h>           
#include <dbsk2d/dbsk2d_ishock_lineline.h>           
#include <dbsk2d/dbsk2d_ishock_linearc.h>            
#include <dbsk2d/dbsk2d_ishock_arcarc.h>             
#include <dbsk2d/dbsk2d_ishock_lineline_thirdorder.h>
#include <dbsk2d/dbsk2d_ishock_arcarc_thirdorder.h> 
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>

#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/dbsk2d_xshock_node.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

//useful typedefs
typedef vcl_list<dbsk2d_xshock_sample_sptr> sample_list;

//: This class contains the algorithm to sample the coarse shock graph
//  into an extrinsic shock graph
// \relates dbsk2d_ishock_graph \relates dbsk2d_shock_graph
class dbsk2d_sample_ishock
{
protected:
  dbsk2d_shock_graph_sptr shock_graph; ///< The coarse intrinsic shock graph
  dbsk2d_shock_graph_sptr xshock_graph; ///< The sample extrinsic shock graph

  //: mapping from node/edge ids to the connected component labels
  vcl_map<int, int> cc_label_map;

  //: mapping from a connected component label to the vector of node/edges that
  //  belong to the component
  vcl_map<int, vcl_vector<int> > cc_elms;

  //: largest connected component: defined to be the inside shock graph
  int largest_component_id;

  //: mapping from node ids to the nodes of the extrinsic shock graph
  vcl_map<int, dbsk2d_shock_node_sptr> nodes_map;

  //: mapping from edge ids to the edges of the extrinsic shock graph
  vcl_map<int, dbsk2d_shock_edge_sptr> edges_map;

  int next_available_sample_id; ///< next available id for a shock sample

  double delta_sample; ///< sampling resolution

public:
  //: Constructor
  dbsk2d_sample_ishock(dbsk2d_shock_graph_sptr coarse_shock_graph);

  //: Destructor
  virtual ~dbsk2d_sample_ishock();

  //: set sample size
  void set_sample_resolution(double resolution){delta_sample=resolution;}

  //: return the extrinsic shock graph
  dbsk2d_shock_graph_sptr extrinsic_shock_graph() { return xshock_graph; }

  int new_sample_id() { return next_available_sample_id++; }

  //: sample the coarse shock graph
  void sample(double resolution, int option=INSIDE);

  //: mark the shocks as inside and outside
  void mark_inside_outside();

  //: sample all the nodes (degenerate nodes)
  void sample_all_nodes(int option);

  //: sample each shock node
  void sample_shock_node(dbsk2d_shock_node_sptr snode, 
                         dbsk2d_xshock_node* new_xnode);
  
  //: sample all the shock edges
  void sample_all_edges(int option);
  
  //: sample the different kinds of intrinsic shock edges
  void sample_ishock_edge(dbsk2d_ishock_pointpoint* spp, dbsk2d_xshock_edge* new_xedge);
  void sample_ishock_edge(dbsk2d_ishock_pointline* spl, dbsk2d_xshock_edge* new_xedge);
  void sample_ishock_edge(dbsk2d_ishock_lineline* sll, dbsk2d_xshock_edge* new_xedge);
  void sample_ishock_edge(dbsk2d_ishock_lineline_thirdorder* slto, dbsk2d_xshock_edge* new_xedge);
  void sample_ishock_edge(dbsk2d_ishock_pointarc* spa, dbsk2d_xshock_edge* new_xedge);
  void sample_ishock_edge(dbsk2d_ishock_linearc* sla, dbsk2d_xshock_edge* new_xedge);
  void sample_ishock_edge(dbsk2d_ishock_arcarc* saa, dbsk2d_xshock_edge* new_xedge);
  void sample_ishock_edge(dbsk2d_ishock_arcarc_thirdorder* sato, dbsk2d_xshock_edge* new_xedge);

  void sample_A1_Ainf_node(dbsk2d_ishock_node* a1ainf, dbsk2d_xshock_edge* new_xedge);

  //: Add the edge adjacency info to the nodes (respecting the ordering)
  void add_edge_adjacency_info(int option);

};

#endif //dbsk2d_ishock_sample_h_
