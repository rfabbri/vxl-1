// This is shp/dbsksp/algo/dbsksp_simplify.h
#ifndef dbsksp_simplify_h_
#define dbsksp_simplify_h_

//:
// \file
// \brief A class to simplify the shock graphs while keeping their essential shape properties
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Jan 24, 2009
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>


//: Convert an A12_Ainfty node to an A12 node whenever possible, i.e., the
// discrepancy between the shape resulted from this conversion and the original
// shape boundary is less than a threshold
bool dbsksp_reduce_A12_Ainfty_nodes(const dbsksp_xshock_graph_sptr& xgraph, 
                                    const vsol_polygon_2d_sptr& boundary,
                                    double eps = 1,
                                    double sampling_ds = 1);







#endif // shp/dbsksp/dbsksp_simplify.h









