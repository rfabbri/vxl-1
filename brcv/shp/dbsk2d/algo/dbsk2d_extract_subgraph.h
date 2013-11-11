// This is brcv/shp/dbsk2d/algo/dbsk2d_extract_subgraph.h
#ifndef dbsk2d_extract_subgraph_h_
#define dbsk2d_extract_subgraph_h_
//:
// \file
// \brief This utility extracts the subgraph from the given node of a shock graph
// \author Ozge C Ozcanli
// \date Dec 19, 06
// 
// \verbatim
//  Modifications
//
// \endverbatim

#include <vcl_vector.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

bool get_other_edge(dbsk2d_shock_node_sptr& next_node, dbsk2d_shock_edge_sptr& current_edge); 

//: take a shock graph, and a pointer to one of its node, and return the subgraph at the given depth 
//  so that the given node is the root
void
dbsk2d_extract_subgraph (dbsk2d_shock_graph_sptr shock, dbsk2d_shock_edge_sptr parent_edge, dbsk2d_shock_node_sptr node, int depth = 1);

#endif //dbsk2d_extract_subgraph_h_
