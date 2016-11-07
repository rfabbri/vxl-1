// This is shp/dbsksp/algo/dbsksp_avg_xgraph_cost_function_utils.h
#ifndef dbsksp_avg_xgraph_cost_function_utils_h_
#define dbsksp_avg_xgraph_cost_function_utils_h_

//:
// \file
// \brief Cost function to compute average of two shock graphs
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Mar 30, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree_sptr.h>
#include <dbsksp/dbsksp_xshock_fragment.h>


//: Compute max-curvature cost - penalize boundary curvature higher than max
// curvature dictated by radius at the node
double dbsksp_compute_kmax_cost(const dbsksp_xshock_fragment& xfrag);



//------------------------------------------------------------------------------
//: Return true if parameter values of xgraph at active nodes and active edges
// are within allowed range
bool dbsksp_is_valid_xgraph(const dbsksp_xshock_graph_sptr& xgraph,
                            const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
                            const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges);


//: Compute root node and edge for a given xgraph tree
bool dbsksp_compute_xgraph_root_node(const dbsksp_xshock_directed_tree_sptr& tree,
                                     dbsksp_xshock_node_sptr& root_node, 
                                     dbsksp_xshock_edge_sptr& pseudo_parent_edge);


//: Compute coarse Euler tour for an xgraph (ignore degree-2 nodes)
bool dbsksp_compute_coarse_euler_tour(const dbsksp_xshock_graph_sptr& model_xgraph, 
    vcl_vector<dbsksp_xshock_node_sptr >& coarse_euler_tour);


#endif // shp/dbsksp/algo/dbsksp_avg_xgraph_cost_function_utils.h









