//---------------------------------------------------------------------
// This is brcv/rec/dbasnh/dbasnh_hypg_algos.h
//:
// \file
// \brief  The dbasn_hypg algorithms.
//
// \author
//  Ming-Ching Chang - Jan 6, 2008
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef dbasnh_hypg_algos_h_
#define dbasnh_hypg_algos_h_

#include <dbasn/dbasnh_hypg.h>

//: Contruct a random generated graph with # of nodes 'nN' and connectivity 'con'.
void generate_random_hypg (dbasnh_hypg* G, const float corner_con);

//: Generates a subgrah of the current graph by deleting the given percentage 
//  of nodes. labels will hold the correct matching.
void generate_subhypg (dbasnh_hypg* g, dbasnh_hypg* inputG, int nN, int *labels); 

// if (links) add unit normal noise to link weights
// if (attrbs) add unit normal noise to attrbs of nodes
// range is the range of uniform noise added
void generate_noisy_subhypg (dbasnh_hypg* g, dbasnh_hypg* inputG, 
                             int *labels, double dNoise,
                             float percent_remove_C); 

int  load_GA_hypg_file (dbasnh_hypg* G, const vcl_string& filename);
void save_GA_hypg_file (dbasnh_hypg* G, const vcl_string& filename);

#endif
