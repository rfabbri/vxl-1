#ifndef psm_camera_opt_lm_direct_homography_generator_h_
#define psm_camera_opt_lm_direct_homography_generator_h_


#include <vil/vil_image_view.h>
#include <vimt/vimt_transform_2d.h>

#include "psm_camera_opt_homography_generator.h"

template<class T>
class psm_camera_opt_lm_direct_homography_generator : public psm_camera_opt_homography_generator<T>
{
public:
  psm_camera_opt_lm_direct_homography_generator(const vil_image_view<T> img0, const vil_image_view<T> img1, const vil_image_view<float> img1_mask)
    : psm_camera_opt_homography_generator<T>(img0,img1,img1_mask) {} 
  
  virtual vimt_transform_2d compute_homography();


};




#endif
