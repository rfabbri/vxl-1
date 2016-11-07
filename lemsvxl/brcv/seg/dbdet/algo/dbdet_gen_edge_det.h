// This is brcv/seg/dbdet/algo/dbdet_gen_edge_det.h
#ifndef dbdet_gen_edge_det_h
#define dbdet_gen_edge_det_h
//:
//\file
//\brief A Generic (Canny-like) edge detector but on subpixel grids
//\author Amir Tamrakar
//\date 09/09/06
//
//\verbatim
//  Modifications
//\endverbatim


#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <dbdet/edge/dbdet_gaussian_kernel.h>
#include <dbdet/edge/dbdet_interp_kernel.h>
#include <dbdet/algo/dbdet_nms.h>
#include <dbdet/algo/dbdet_subpix_convolution.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/algo/dbdet_edge_appearance_util.h>

//: function to compute generic edges
dbdet_edgemap_sptr dbdet_detect_generic_edges(vil_image_view<vxl_byte> image, double sigma, double threshold, int N)
{
  //convert to grayscale
  vil_image_view<vxl_byte> greyscale_view;
  if(image.nplanes() == 3){
    vil_convert_planes_to_grey(image, greyscale_view );
  }
  else if (image.nplanes() == 1){
    greyscale_view = image;
  }

  //compute image gradients before performing nonmax suppression
  vil_image_view<double> grad_x, grad_y, grad_mag;

  //Gaussian  
  int scale = (int) vcl_pow(2.0, N);

  //compute gradients
  dbdet_subpix_convolve_2d(greyscale_view, grad_x, dbdet_Gx_kernel(sigma), double(), N);
  dbdet_subpix_convolve_2d(greyscale_view, grad_y, dbdet_Gy_kernel(sigma), double(), N);

  //compute gradient magnitude
  grad_mag.set_size(grad_x.ni(), grad_x.nj());

  //get the pointers to the memory chunks
  double *gx  =  grad_x.top_left_ptr();
  double *gy  =  grad_y.top_left_ptr();
  double *g_mag  =  grad_mag.top_left_ptr();

  //compute the gradient magnitude
  for(unsigned long i=0; i<grad_mag.size(); i++)
    g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

  //Now call the nms code to get the subpixel edge tokens
  vcl_vector<vgl_point_2d<double> > loc;
  vcl_vector<double> orientation, mag;

  dbdet_nms NMS(dbdet_nms_params(threshold, dbdet_nms_params::PFIT_3_POINTS), grad_x, grad_y, grad_mag);
  NMS.apply(true, loc, orientation, mag);
  
  //create a new edgemap from the tokens collected from NMS
  dbdet_edgemap_sptr edge_map = new dbdet_edgemap(greyscale_view.ni(), greyscale_view.nj());

  for (unsigned i=0; i<loc.size(); i++){
    vgl_point_2d<double> pt(loc[i].x()/scale, loc[i].y()/scale);
    edge_map->insert(new dbdet_edgel(pt, orientation[i], mag[i]));
  }

  //add intensity appearance to the edges
  dbdet_add_intensity_app(greyscale_view, edge_map, sigma, 1); //opt: 0: original , 1: smoothed, 2: Half gaussian

  return edge_map;
}

#endif // dbdet_gen_edge_h
