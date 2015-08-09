// This is seg/dbsks/dbsks_xshock_utils.h
#ifndef dbsks_xshock_utils_h_
#define dbsks_xshock_utils_h_

//:
// \file
// \brief Utilities used for both dbsks_xshock_detect
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Sep 13, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <dbsksp/algo/dbsksp_screenshot.h>

#include <vsol/vsol_polyline_2d_sptr.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vil/algo/vil_flood_fill.h>




////: Update descriptors around a degree-2 node, given the descriptor of the child edge
//void dbsks_update_degree2_node(const dbsksp_xshock_node_sptr& xv, 
//                               const dbsksp_xshock_edge_sptr& xe_child, 
//                               const dbsksp_xshock_node_descriptor& xdesc);


////: Update descriptors around a degree-3 node
//void dbsks_update_degree3_node(const dbsksp_xshock_node_sptr& xv, 
//                               const dbsksp_xshock_edge_sptr& xe_parent,
//                               const dbsksp_xshock_edge_sptr& xe_child1,
//                               const dbsksp_xshock_edge_sptr& xe_child2,
//                               const dbsksp_xshock_node_descriptor& xdesc_parent,
//                               double phi_child1);
//


// -----------------------------------------------------------------------------
//: Scale the edgemap, edgeorient, and contour map so that the xgraph scale is
// about the "standard scale"
bool dbsks_adjust_to_standard_scale(double standard_scale,
                               double cur_scale,
                               const vil_image_view<float >& cur_edgemap,
                               const vcl_vector<vsol_polyline_2d_sptr >& cur_polyline_list,
                               const vil_image_view<float >& cur_edge_angle,
                               double& scaled_up_factor,
                               vil_image_view<float >& new_edgemap,
                               vcl_vector<vsol_polyline_2d_sptr >& new_polyline_list,
                               vil_image_view<float >& new_edge_angle);



// -----------------------------------------------------------------------------
//: Scale the edgemap, edgeorient, and contour map so that the xgraph scale is
// about the "standard scale"
bool dbsks_adjust_to_standard_scale_sparse(double standard_scale,
                               double cur_scale,
                               const vil_image_view<float >& cur_edgemap,
                               const vcl_vector<vsol_polyline_2d_sptr >& cur_polyline_list,
                               const vil_image_view<float >& cur_edge_angle,
                               double& scaled_up_factor,
                               vil_image_view<float >& new_edgemap,
                               vcl_vector<vsol_polyline_2d_sptr >& new_polyline_list,
                               vil_image_view<float >& new_edge_angle);




                               
// -----------------------------------------------------------------------------
//: Scale the edgemap, edgeorient, and contour map so that the xgraph scale is
// about the "standard scale"
bool dbsks_adjust_to_standard_scale_dense(double standard_scale,
                               double cur_scale,
                               const vil_image_view<float >& cur_edgemap,
                               const vcl_vector<vsol_polyline_2d_sptr >& cur_polyline_list,
                               const vil_image_view<float >& cur_edge_angle,
                               double& scaled_up_factor,
                               vil_image_view<float >& new_edgemap,
                               vcl_vector<vsol_polyline_2d_sptr >& new_polyline_list,
                               vil_image_view<float >& new_edge_angle);


// -----------------------------------------------------------------------------
//: Scale the edgemap and edgeorient so that the xgraph scale is
// about the "standard scale"
bool dbsks_adjust_to_standard_scale(double standard_scale,
                               double cur_scale,
                               const vil_image_view<float >& cur_edgemap,
                               const vil_image_view<float >& cur_edge_angle,
                               double& scaled_up_factor,
                               vil_image_view<float >& new_edgemap,
                               vil_image_view<float >& new_edge_angle);


//------------------------------------------------------------------------------
//: min-max of a set of angles, noting angles are circular
vnl_vector<double > dbsks_compute_angle_minmax(const vnl_vector<double >& angles,
                          double& min_angle, double& max_angle);

//: fill the outline drawn from given xgraph. output all the interior points and binary shape map. 
bool dbsks_fill_in_silhouette(const dbsksp_xshock_graph_sptr& xgraph, const vil_image_view<vxl_byte >& image, vcl_vector<vgl_point_2d<int > >& points, vil_image_view<vxl_byte >& screenshot_image);

vcl_vector<vnl_matrix<double> > dbsks_compute_appearance_id_matrix(vcl_vector<vgl_point_2d<int > >& points, const vil_image_view<vxl_byte >& source_image);


double dbsks_shape_inconsistency(const dbsksp_xshock_graph_sptr& xgraph1, const dbsksp_xshock_graph_sptr& xgraph2);






#endif // shp/dbsks/dbsks_xshock_utils.h


