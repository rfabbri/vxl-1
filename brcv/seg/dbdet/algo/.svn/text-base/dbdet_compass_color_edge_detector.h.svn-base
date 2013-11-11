// This is brcv/seg/dbdet/algo/dbdet_compass_color_edge_detector.h
#ifndef dbdet_compass_color_edge_detector_h
#define dbdet_compass_color_edge_detector_h
//:
//\file
//\brief An implementation of the Compass Edge detector By Ruzon and Tomasi, PAMI 01
//       For Color edge detection
//
// Details -- The compass operator uses a circular window centered at a
//   junction where 4 pixel squares meet.  The needle is a diameter at a
//   given orientation.  The color distributions of the two semicircles 
//   are computed, and the distance between them is measured using the   
//   Earth Mover's Distance (EMD).  The maximum EMD over all orientations 
//   gives the edge strength and orientation at that point.
//
//\author Amir Tamrakar
//\date 08/26/07
//
//\verbatim
//  Modifications Adapted from the code from the authors
//
//\endverbatim

#include <vil/vil_image_view.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#define DEFAULTCLUSTERS 10
#define BLACK 40
#define SAMPLE_PERCENTAGE 1.0
#define PERCEPTUAL_THRESH 11.0


//: Function to find edges using a compass operator
dbdet_edgemap_sptr 
dbdet_detect_compass_color_edges(vil_image_view<vxl_byte>& image, int spacing, int n_wedges, int weight_type, double sigma, 
                           int dist_op, bool SG_filter, double threshold, vil_image_view<double>& hist_grad,
                           bool output_orientation_map);

//: Function to find edges using a compass operator
dbdet_edgemap_sptr 
dbdet_detect_compass_color_edges(vil_image_view<double>& L, 
                                 vil_image_view<double>& a,
                                 vil_image_view<double>& b, 
                                 int spacing, int n_wedges, int weight_type, double sigma, 
                                 int dist_op, bool SG_filter, double threshold, vil_image_view<double>& hist_grad,
                                 bool output_orientation_map);

//: Function to find edges using independent compass operators for L, a and b
dbdet_edgemap_sptr
dbdet_detect_compass_color_edges_independent(vil_image_view<double>& L, 
                                             vil_image_view<double>& a,
                                             vil_image_view<double>& b, 
                                             int spacing, int n_wedges, int weight_type, double sigma, 
                                             int dist_op, bool SG_filter, double threshold, vil_image_view<double>& hist_grad,
                                             bool output_orientation_map);

//: Compute color histogram gradient using the compass operator
void dbdet_compute_compass_color_gradient( vil_image_view<vxl_byte>& image, int spacing, int n_wedges, 
                                     int weight_type, double sigma, int dist_op, bool SG_filter,
                                     vil_image_view<double>& hist_grad,
                                     vil_image_view<double>& hist_ori);

//: Compute color histogram gradient using the compass operator
// Note: # of orientations = 2*n_wedges
void dbdet_compute_compass_color_gradient( vil_image_view<double>& L, 
                                           vil_image_view<double>& a,
                                           vil_image_view<double>& b,
                                           int spacing, int n_wedges, 
                                           int weight_type, double sigma, int dist_op, bool SG_filter,
                                           vil_image_view<double>& hist_grad,
                                           vil_image_view<double>& hist_ori);

#endif // dbdet_compass_color_edge_detector_h
