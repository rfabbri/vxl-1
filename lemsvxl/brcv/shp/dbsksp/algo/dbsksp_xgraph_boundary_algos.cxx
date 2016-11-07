// This is /lemsvxl/brcv/shp/dbsksp/algo/dbsksp_xgraph_boundary_algos.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Jan 19, 2011
//

#include "dbsksp_xgraph_boundary_algos.h"
#include <dbsksp/algo/dbsksp_trace_boundary.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vgl/vgl_polygon.h>
#include <bsol/bsol_algs.h>

//Test if the boundary intersects itself
bool dbsksp_test_boundary_self_intersection(dbsksp_xshock_graph_sptr xgraph, int num_points)
{
    double approx_ds = 1.0;
    vsol_polygon_2d_sptr query_poly = dbsksp_trace_xgraph_boundary_as_polygon(xgraph, approx_ds);
    query_poly = dbsksp_trace_xgraph_boundary_as_polygon(xgraph, double(query_poly->size()*approx_ds) / num_points);
    vgl_polygon<double > query_vgl_poly = bsol_algs::vgl_from_poly(query_poly);
    vcl_vector<vcl_pair<unsigned,unsigned> > e1;
    vcl_vector<vcl_pair<unsigned,unsigned> > e2;
    vcl_vector<vgl_point_2d<double> > ip;
    vgl_selfintersections(query_vgl_poly, e1, e2, ip);
    return ip.size() > 0;
}

//Get the list of points lying on the boundary
vcl_vector<vsol_point_2d_sptr > dbsksp_get_boundary_point_list(dbsksp_xshock_graph_sptr xgraph, int num_points)
{
    double approx_ds = 1.0;
    vcl_vector<vsol_point_2d_sptr > list_;
    vsol_polygon_2d_sptr query_poly = dbsksp_trace_xgraph_boundary_as_polygon(xgraph, approx_ds);
    query_poly = dbsksp_trace_xgraph_boundary_as_polygon(xgraph, double(query_poly->size()*approx_ds) / num_points);
    for(int i = 0; i < query_poly->size(); i++)
    {
        list_.push_back(query_poly->vertex(i));
    }
    return list_;
}
