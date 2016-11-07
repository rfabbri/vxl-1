#include "bioproc_tests_modified_no_noise_filter_3d.h"

 vgl_box_2d<int> bioproc_tests_modified_no_noise_filter_3d::
  get_box_2d(xmvg_perspective_camera<double>  cam,
          vgl_box_3d<double> const & box){
   return vgl_box_2d<int>(0, (cam.get_img_dim()[0])-1, 0, (cam.get_img_dim()[1])-1);
 }

bioproc_tests_modified_no_noise_filter_3d::bioproc_tests_modified_no_noise_filter_3d(xmvg_no_noise_filter_descriptor& descriptor)
  : xmvg_no_noise_filter_3d(descriptor) {}
    
