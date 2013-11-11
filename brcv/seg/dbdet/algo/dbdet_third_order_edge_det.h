// This is brcv/seg/dbdet/algo/dbdet_third_order_edge_det.h
#ifndef dbdet_third_order_edge_det_h
#define dbdet_third_order_edge_det_h
//:
//\file
//\brief An edge detector that first computes edge locations and then optimizes the orientations using the third order operator
//\author Amir Tamrakar
//\date 09/09/06
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
dbdet_edgemap_sptr dbdet_detect_third_order_edges(vil_image_view<vxl_byte> image, double sigma, double threshold, 
                                                  int N, unsigned parabola_fit=0, unsigned grad_op=0, unsigned conv_algo=0, 
                                                  bool use_adaptive_thresh=true,
                                                  bool interp_grid=false,
                                                  bool reduce_tokens=false);


#endif // dbdet_third_order_edge_det_h
