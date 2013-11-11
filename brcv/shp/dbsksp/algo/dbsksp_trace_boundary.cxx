// This is file shp/dbsksp/dbsksp_trace_boundary.cxx

//:
// \file

#include "dbsksp_trace_boundary.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>

#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vnl/vnl_math.h>



//: Trace boundary of an xshock graph as an ordered-list of biarcs
bool dbsksp_trace_xgraph_boundary_as_biarc_spline(const dbsksp_xshock_graph_sptr& xgraph, 
                                                  vcl_vector<dbgl_biarc >& boundary)
{
  // Preliminary checks
  boundary.clear();
  boundary.reserve(2*xgraph->number_of_edges());

  if (!xgraph)
    return false;
  
  // Euler tour on the shock graph
  dbsksp_xshock_node_sptr xv0 = *(xgraph->vertices_begin());
  dbsksp_xshock_edge_sptr xe0 = *(xv0->edges_begin());

  dbsksp_xshock_node_sptr cur_xv = xv0;
  dbsksp_xshock_edge_sptr cur_xe = xe0;
  do
  {
    // node descriptors of the xfrag (along the flowing direction)
    dbsksp_xshock_node_descriptor xdesc_start = *cur_xv->descriptor(cur_xe);
    dbsksp_xshock_node_descriptor xdesc_end = cur_xe->opposite(cur_xv)->descriptor(cur_xe)->opposite_xnode();

    // form the fragment 
    dbsksp_xshock_fragment xfrag(xdesc_start, xdesc_end);

    // take the right boundary contour
    dbgl_biarc right_biarc = xfrag.bnd_right_as_biarc();

    // push the boundary contour to the ordered list
    boundary.push_back(right_biarc);

    // move to the next edge
    cur_xv = cur_xe->opposite(cur_xv);
    cur_xe = xgraph->cyclic_adj_succ(cur_xe, cur_xv);
  }
  while (cur_xv != xv0 || cur_xe != xe0);

  return true;
}



// -----------------------------------------------------------------------------
//: Trace boundary of an xshock graph as a polygon, 
// Paramter: "approx_ds": approximate distance between adjacent vertices
bool dbsksp_trace_xgraph_boundary_as_polygon(const dbsksp_xshock_graph_sptr& xgraph,
                                             vcl_vector<vgl_point_2d<double > >& polygon,
                                             double approx_ds)
{
  // first thing first
  polygon.clear();

  if (approx_ds < 0)
  {
    vcl_cout << "\nERROR: approx_ds needs to be a positive number.\n";
    return false;
  }

  // a) Acquire the boundary as an ordered set of biarcs
  vcl_vector<dbgl_biarc > biarc_list;
  if (!dbsksp_trace_xgraph_boundary_as_biarc_spline(xgraph, biarc_list))
  {
    return false;
  }

  // b) sample each biarc to acquire the sample points for the polygon
  for (unsigned idx =0; idx < biarc_list.size(); ++idx)
  {
    dbgl_biarc& biarc = biarc_list[idx];
    int num_segs = vnl_math_rnd(biarc.len() / approx_ds);

    // force at least 1 segment per biarc
    if (num_segs == 0)
      num_segs = 1;

    // Compute the samples, leave the last one out (because it will be included 
    // in the next biarc)
    double biarc_ds = biarc.len() / num_segs;
    for (int i =0; i < num_segs; ++i)
    {
      polygon.push_back(biarc.point_at(i*biarc_ds));
    }
  }
  return true;
}






// -----------------------------------------------------------------------------
//: Trace boundary of an xshock graph as a polygon, 
// Parameter: "approx_ds": approximate distance between adjacent vertices
vsol_polygon_2d_sptr dbsksp_trace_xgraph_boundary_as_polygon(const dbsksp_xshock_graph_sptr& xgraph,
                                                             double approx_ds)
{
  vcl_vector<vgl_point_2d<double > > pts;
  if (!dbsksp_trace_xgraph_boundary_as_polygon(xgraph, pts, approx_ds))
    return 0;

  // convert to vsol_polygon_2d
  vcl_vector<vsol_point_2d_sptr > vertices(pts.size(), 0);
  for (unsigned i =0; i < pts.size(); ++i)
  {
    vertices[i] = new vsol_point_2d(pts[i]);
  }
  return new vsol_polygon_2d(vertices);
}



