#ifndef psm_camera_opt_homography_generator_h_
#define psm_camera_opt_homography_generator_h_


#include <vimt/vimt_transform_2d.h>
#include <vil/vil_image_view.h>

template<class T>
class psm_camera_opt_homography_generator
{
public:

  psm_camera_opt_homography_generator(const vil_image_view<T> img0, const vil_image_view<T> img1) : img0_(img0), img1_(img1), use_mask0_(false), use_mask1_(false), compute_projective_(false) {};
  psm_camera_opt_homography_generator(const vil_image_view<T> img0, const vil_image_view<T> img1, vil_image_view<float> const& mask1) : img0_(img0), img1_(img1), mask1_(mask1), use_mask0_(false), use_mask1_(true), compute_projective_(false) {};

  //void set_image0(vil_image_view<T> const& img0) { img0_ = img0; }
  //void set_image1(vil_image_view<T> const& img1) { img1_ = img1; }
  //void set_mask0(vil_image_view<float> const& mask0) { mask0_ = mask0; use_mask0_ = true; }
  //void set_mask1(vil_image_view<float> const& mask1) { mask1_ = mask1; use_mask1_ = true; }
  //: set generator to compute a projective transformation.  default is affine.
  void set_projective(bool use_projective) {compute_projective_ = use_projective; }

  virtual vimt_transform_2d compute_homography() = 0;

protected:
  bool use_mask0_;
  bool use_mask1_;
  bool compute_projective_;

  const vil_image_view<T> img0_;
  const vil_image_view<T> img1_;
  const vil_image_view<float> mask0_;
  const vil_image_view<float> mask1_;

};




#endif
