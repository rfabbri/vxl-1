//---------------------------------------------------------------------
// This is brcv/rec/dbasn/dbasn_graph_algos.h
//:
// \file
// \brief  Some simple graph algorithms.
//
// \author
//  Ming-Ching Chang - Aug, 29, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef dbasn_graph_algos_h_
#define dbasn_graph_algos_h_

#include <dbasn/dbasn_graph.h>

//: Contruct a random generated graph with # of nodes 'nN' and connectivity 'con'.
void generate_random_graph (dbasn_graph* G, const int nN, const float con);

//: Generates a subgrah of the current graph by deleting the given percentage 
//  of nodes. labels will hold the correct matching.
dbasn_graph* generate_subgraph (dbasn_graph* inputG, int nN, int *labels); 

// if (links) add unit normal noise to link weights
// if (attrbs) add unit normal noise to attrbs of nodes
// range is the range of uniform noise added
void generate_noisy_subgraph (dbasn_graph* g, dbasn_graph* inputG, int nN, int *labels, 
                              bool b_links, bool b_attrbs, double range,
                              float percent_remove_L); 

int  load_GA_graph_file (dbasn_graph* G, const vcl_string& filename);
void save_GA_graph_file (dbasn_graph* G, const vcl_string& filename);

#endif
