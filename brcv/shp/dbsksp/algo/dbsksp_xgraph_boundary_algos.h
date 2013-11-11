// This is /lemsvxl/brcv/shp/dbsksp/algo/dbsksp_xgraph_boundary_algos.h.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Jan 19, 2011
//

#ifndef DBSKSP_XGRAPH_BOUNDARY_ALGOS_H_
#define DBSKSP_XGRAPH_BOUNDARY_ALGOS_H_

#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <vcl_vector.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

//Test if the boundary intersects itself
bool dbsksp_test_boundary_self_intersection(dbsksp_xshock_graph_sptr xgraph, int num_points);

//Get the list of points lying on the boundary
vcl_vector<vsol_point_2d_sptr> dbsksp_get_boundary_point_list(dbsksp_xshock_graph_sptr xgraph, int num_points);

#endif /* DBSKSP_XGRAPH_BOUNDARY_ALGOS_H_ */
