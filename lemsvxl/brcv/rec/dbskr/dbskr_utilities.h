// This is brcv/rec/dbskr/dbskr_utilities.h
#ifndef dbskr_utilities_h_
#define dbskr_utilities_h_
//:
// \file
// \brief Various utility methods used in recognition
// \author Ozge C. Ozcanli
//
// \verbatim
//  Modifications
//   O.C.Ozcanli  03/08/2007   
//
// \endverbatim 

#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_string.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>
#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>


#include <dbskr/dbskr_scurve_sptr.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>

#include <vil/vil_image_resource_sptr.h>

// length_thres            : prune out the polylines that are shorter in length than this threshold before shock computation, typically 2 pixels
// pruning_region_width    : the width of the region to consider on both sides of a polyline to determine the color contrast, typically 5 pixels
// pruning_color_threshold : the threshold for LAB space color contrast, typically 0.4
// rms                     : root mean square error used in line fitting process before shock computation, typically 0.05
dbsk2d_shock_graph_sptr extract_shock(vil_image_resource_sptr img_sptr,
                                      unsigned min_size_to_keep,
                                      double avg_grad_mag_threshold,
                                      double length_thresh, 
                                      double pruning_region_width, 
                                      double pruning_color_threshold, 
                                      double rms, unsigned smoothing_nsteps,
                                      double cont_t = 0.5, double app_t = 0.5,
                                      bool write_output = false,
                                      vcl_string out_shock_name = "", double edge_detection_sigma = 1.0); 

//vsol_polyline_2d_sptr fit_lines_to_contour(vsol_polyline_2d_sptr poly, double rms);
//vsol_polygon_2d_sptr fit_lines_to_contour(vsol_polygon_2d_sptr poly, double rms);

dbsk2d_shock_graph_sptr extract_shock_from_mask(vil_image_resource_sptr image_sptr, 
                                                bool write_output, vcl_string out_shock_name, int n_steps, double rms);


dbsk2d_shock_graph_sptr compute_shock (vsol_polygon_2d_sptr poly, float pruning_threshold);


#endif // dbskr_utilities_h_

