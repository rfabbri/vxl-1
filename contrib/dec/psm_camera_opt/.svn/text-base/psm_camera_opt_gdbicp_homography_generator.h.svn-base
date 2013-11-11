#ifndef psm_camera_opt_gdbicp_homography_generator_h_
#define psm_camera_opt_gdbicp_homography_generator_h_

#include <vil/vil_image_view.h>
#include <vcl_string.h>
#include <vimt/vimt_transform_2d.h>

#include "psm_camera_opt_homography_generator.h"

template<class T>
class psm_camera_opt_gdbicp_homography_generator : public psm_camera_opt_homography_generator<T>
{
public:
   psm_camera_opt_gdbicp_homography_generator(vil_image_view<T> const& img0, vil_image_view<T> const& img1, vil_image_view<float> const& img1_mask)
    : psm_camera_opt_homography_generator<T>(img0,img1,img1_mask) {} 
  
  virtual vimt_transform_2d compute_homography();

private:
  vimt_transform_2d parse_gdbicp_output(vcl_string filename);


};




#endif
