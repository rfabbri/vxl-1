// This is shp/dbsksp/algo/dbsksp_trace_boundary.h
#ifndef dbsksp_trace_boundary_h_
#define dbsksp_trace_boundary_h_

//:
// \file
// \brief A set of functions to trace boundary of shock graphs
//        
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Jan 26, 2009
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbgl/algo/dbgl_biarc.h>
#include <vcl_vector.h>
#include <vsol/vsol_polygon_2d_sptr.h>


//: Trace boundary of an xshock graph as an ordered-list of biarcs
bool dbsksp_trace_xgraph_boundary_as_biarc_spline(const dbsksp_xshock_graph_sptr& xgraph, 
                                            vcl_vector<dbgl_biarc >& boundary);

//: Trace boundary of an xshock graph as a polygon, 
// Parameter: "approx_ds": approximate distance between adjacent vertices
bool dbsksp_trace_xgraph_boundary_as_polygon(const dbsksp_xshock_graph_sptr& xgraph,
                                             vcl_vector<vgl_point_2d<double > >& polygon,
                                             double approx_ds = 1);

//: Trace boundary of an xshock graph as a polygon, 
// Parameter: "approx_ds": approximate distance between adjacent vertices
vsol_polygon_2d_sptr dbsksp_trace_xgraph_boundary_as_polygon(const dbsksp_xshock_graph_sptr& xgraph,
                                                             double approx_ds = 1);

                                    







#endif // shp/dbsksp/dbsksp_trace_boundary.h









