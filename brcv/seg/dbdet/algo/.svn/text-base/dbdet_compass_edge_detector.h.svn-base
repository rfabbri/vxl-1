// This is brcv/seg/dbdet/algo/dbdet_compass_edge_detector.h
#ifndef dbdet_compass_edge_detector_h
#define dbdet_compass_edge_detector_h
//:
//\file
//\brief An implementation of the Compass Edge detector By Ruzon and Tomasi, PAMI 01
//       For Grayscale edge detection
//\author Amir Tamrakar
//\date 08/26/07
//
//\verbatim
//  Modifications
//\endverbatim

#include <vil/vil_image_view.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

//: Function to find edges using a compass operator
dbdet_edgemap_sptr 
dbdet_detect_compass_edges(vil_image_view<vxl_byte>& image, int spacing, int n_wedges, int weight_type, double sigma, 
                           int dist_op, bool SG_filter, double threshold, bool third_order, vil_image_view<double>& hist_grad,
                           bool output_orientation_map);

//: Compute intensity histogram gradient using the compass operator
void dbdet_compute_compass_gradient( vil_image_view<vxl_byte>& image, int spacing, int n_wedges, 
                                     int weight_type, double sigma, int dist_op, bool SG_filter,
                                     vil_image_view<double>& hist_grad,
                                     vil_image_view<double>& hist_ori);


//supporting functions
inline double CArea(double Xhigh, double Xlow, double Y, double r);

double* MakeQtrMask(double r, int n_wedges);

//: compute the optimal orientation and edge strength from the vector of edge strengths
//  at computed at discrete orientations
void compute_strength_and_orient(double* dist, int n_orient, double& strength, double& orientation);

#endif // dbdet_compass_edge_detector_h
