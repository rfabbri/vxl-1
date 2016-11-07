// This is brcv/shp/dbsk2d/algo/dbsk2d_prune_ishock.h
#ifndef dbsk2d_prune_ishock_h_
#define dbsk2d_prune_ishock_h_
//:
// \file
// \brief Algorith to prune intrinsic shock graphs
// \author Amir Tamrakar
// \date 06/20/05
// 
// This file contains the function to prune an instrinsic shock graph
// up to the specified saliency threshold and output a coarse level
// shock graph correspoding to the remaining shock topology

// \verbatim
//  Modifications
//   Amir Tamrakar 06/20/2005    Initial version.
//
//   Amir Tamrakar 07/07/2005    Moved to dbsk2d/algo
//
//   Ozge C. Ozcanli 04/20/2005  Added intrinsic edge pruning given a set of boundaries
//
// \endverbatim 

#include <vcl_map.h>

#include <dbsk2d/dbsk2d_ishock_elm.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>

#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <dbsk2d/dbsk2d_shock_ishock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_grouped_ishock_edge_sptr.h>

#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>

//: Shock Pruning algorithm
// \relates dbsk2d_ishock_graph
class dbsk2d_prune_ishock
{
protected:

  //: parameters needed to compute shock saliency
  struct saliency_params {
    double dOC;
    double dNC;
    double dPnCost;
  };

  //: time ordered shock list 
  vcl_multimap<vcl_pair<double, int>, dbsk2d_ishock_elm*> ordered_shock_list;
  
  //: dbsk2d_ishock_elm ID to saliency_params mapping
  vcl_map <int, saliency_params> shock_saliency_map;

  //: dbsk2d_ishock_node ID to dbsk2d_shock_node mapping
  vcl_map <int, dbsk2d_shock_node_sptr> ishock_to_shock_node_map;

  //: dbsk2d_ishock_edge ID to dbsk2d_shock_edge mapping
  vcl_map <int, dbsk2d_shock_edge_sptr> ishock_to_shock_edge_map;

  dbsk2d_ishock_graph_sptr ishock_graph;
  dbsk2d_shock_graph_sptr shock_graph;

public:

  //: Constructor
  dbsk2d_prune_ishock(dbsk2d_ishock_graph_sptr intrinsic_shock_graph, dbsk2d_shock_graph_sptr coarse_shock_graph) :
    ishock_graph(intrinsic_shock_graph), shock_graph(coarse_shock_graph){}

  //: Destructor
  virtual ~dbsk2d_prune_ishock() {}


  double splice_cost(dbsk2d_ishock_elm* selm);
  
  //: return the intrinsic shock graph
  dbsk2d_ishock_graph_sptr intrinsic_shock_graph() { return ishock_graph; }

  //: return the coarse shock graph
  dbsk2d_shock_graph_sptr coarse_shock_graph() { return shock_graph; }

  //: compute the salency of this shock element (edge/node)
  void compute_shock_saliency(dbsk2d_ishock_elm* selm);

  //: prune the shock graph
  void prune(double thresh);

  //: prune all the intrinsic shock edges which has samples not supported by the given set of boundaries
  void prune_based_on_support(vcl_vector<vsol_polyline_2d_sptr>& rbs, vsol_box_2d_sptr bbox, int pixel_range_in_mask_image = 2);

  //: compile the coarse shock graph from the pruned shock graph
  void compile_coarse_shock_graph();

  //: compile all the valid remaining nodes into the coarse graph nodes
  void compile_nodes();

  //: Compile all the remaining edges into coarse shock edges
  void compile_edges();
  void compile_edges_of_node(dbsk2d_shock_node_sptr node);

  //: after the nodes and edges have been compiled
  //  a second pass is needed to assign the intrinsic information at the nodes
  void update_intrinsic_information_at_the_nodes();

  //: assing the intrinsic information at the given node
  void update_intrinsic_information_at_the_node(dbsk2d_shock_node_sptr node);

  //: trace from a given ishock edge to the next coarse level node while
  // compiling a list of edges that it traversed through
  dbsk2d_shock_node_sptr trace_to_target_node(dbsk2d_ishock_edge* shock_edge, 
    vcl_list<dbsk2d_ishock_edge*>& shock_edges);

  //: return cost for a certain id
  double dOC(int id){return shock_saliency_map[id].dOC;}
  double dNC(int id){return shock_saliency_map[id].dNC;}
  double dPnCost(int id){return shock_saliency_map[id].dPnCost;}
 
};

#endif //dbsk2d_ishock_prune_h_
