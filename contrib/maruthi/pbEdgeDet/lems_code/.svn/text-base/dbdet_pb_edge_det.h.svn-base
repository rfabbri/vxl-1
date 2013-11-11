// This is brcv/seg/dbdet/algo/dbdet_pb_edge_det.h
#ifndef dbdet_pb_edge_det_h
#define dbdet_pb_edge_det_h
//:
//\file
//\brief A wrapper around the pb Edge Detector
// Based on the Paper "Learning to Detect Natural Image Boundaries, Using 
//                     Local Brightness, Color, & Texture Cues", Martin & all
//\author Maruthi Narayanan
//\date 05/28/09
//
//\verbatim
//  Modifications
//\endverbatim

#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <dbdet/edge/dbdet_gaussian_kernel.h>
#include <dbdet/edge/dbdet_interp_kernel.h>
#include <dbdet/algo/dbdet_nms.h>
#include <dbdet/algo/dbdet_subpix_convolution.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_sel_utils.h>

//: function to compute generic edges
dbdet_edgemap_sptr dbdet_detect_pb_edges(vil_image_view<vxl_byte> image, 
                                         double sigma, double threshold, 
                                         int N, unsigned parabola_fit=0, 
                                         unsigned grad_op=0, unsigned conv_algo=0, 
                                         bool use_adaptive_thresh=true,
                                         bool interp_grid=false,
                                         bool reduce_tokens=false);


#endif // dbdet_pb_edge_det_h
