// This is brcv/shp/dbsksp/algo/dbsksp_compute_scurve.h

#ifndef dbsksp_compute_scurve_h_
#define dbsksp_compute_scurve_h_

//:
//: \file
//: \brief  Contains code to compute shock curves from shock graph paths
//: \author Nhon Trinh (ntrinh@lems.brown.edu)
//: \date   Nov 5, 2009
// 
//: \verbatim
//  Modifications
//      Nhon Trinh     Nov 5, 2009     Initial version
//
//: \endverbatim


#include <dbskr/dbskr_scurve_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <vcl_vector.h>

#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsksp/algo/dbsksp_shock_path_sptr.h>


//: \relates dbsksp_xshock_graph
//: Compute a shock curve from a shock graph path
dbsksp_shock_path_sptr dbsksp_compute_uniform_shock_path(const dbsksp_xshock_node_sptr& start_node,
                                       const vcl_vector<dbsksp_xshock_edge_sptr>& path,
                                       double sample_ds = 1.0f);


//: \relates dbsksp_xshock_graph
//: Compute a shock curve from a shock graph path
dbskr_scurve_sptr dbsksp_compute_scurve(const dbsksp_xshock_node_sptr& start_node,
                                       const vcl_vector<dbsksp_xshock_edge_sptr>& path,
                                       double sample_ds = 1.0f);

#endif //dbsksp_compute_scurve_h_
