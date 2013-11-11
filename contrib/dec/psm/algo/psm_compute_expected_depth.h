#ifndef psm_compute_expected_depth_h_
#define psm_compute_expected_depth_h_

#include <vcl_vector.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_image_sample.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>

#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_mog_rgb_processor.h>
#include <psm/psm_simple_rgb_processor.h>
#include <psm/psm_simple_grey_processor.h>

#include "psm_raytrace_function.h"


template<psm_apm_type APM>
class psm_compute_expected_depth_functor
{
public:

  //: default constructor
  psm_compute_expected_depth_functor(vgl_point_3d<double> camera_center, vil_image_view<float> &expected_depth, vil_image_view<float> &vis_inf_prob) : expected_depth_(expected_depth), vis_prob_(vis_inf_prob), alpha_integral_(expected_depth.ni(),expected_depth.nj()), camera_center_(camera_center)
  {
    alpha_integral_.fill(0.0f);
    vis_prob_.fill(1.0f);
    expected_depth_.fill(0.0f);
  }

  //: accumulate 
  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> p0, vgl_point_3d<double> p1, psm_sample<APM> &cell_value, typename psm_aux_traits<PSM_AUX_NULL>::sample_datatype &aux_value, psm_cell_id const& cell_id)
  {
    const float alpha = cell_value.alpha;

    if (alpha <= 0.0f) {
      return true;
    }

    const float s0 = (float)(camera_center_ - p0).length();
    const float s1 = (float)(camera_center_ - p1).length();

    const float cell_len = s1 - s0;

    const float pass_prob = vcl_exp(-alpha*cell_len);

    expected_depth_(i,j) +=  vis_prob_(i,j) / alpha * (1 + alpha*s0 - pass_prob*(alpha*s1 + 1));

    // update visibility probabilty
    alpha_integral_(i,j) += alpha * cell_len;
    vis_prob_(i,j) = vcl_exp(-alpha_integral_(i,j));

    return (vis_prob_(i,j) > 1e-5);
  }

private:

  vgl_point_3d<double> camera_center_;
  vil_image_view<float> &vis_prob_;
  vil_image_view<float> &expected_depth_;
  vil_image_view<float> alpha_integral_;


};

//: Functor class to normalize expected image
class normalize_expected_depth_functor
{
public:
  normalize_expected_depth_functor() {}

  void operator()(float vis_inf, float &depth) const 
  {
    if (vis_inf < 1.0f) {
      depth /= (1.0f - vis_inf);
    }
    else {
      depth = 0.0f;
    }
  }
};




template <psm_apm_type APM>
void psm_compute_expected_depth(psm_scene<APM> &scene, const vpgl_camera<double>* cam, vil_image_view<float> &expected_depth, vil_image_view<float> &vis_inf_prob)
{

  psm_raytrace_function<psm_compute_expected_depth_functor<APM>, APM> raytrace_fn(scene, cam, expected_depth.ni(), expected_depth.nj(), false);
  if (const vpgl_perspective_camera<double>* pcam = dynamic_cast<const vpgl_perspective_camera<double>*>(cam)) {
    vgl_point_3d<double> cam_center(pcam->camera_center());
    psm_compute_expected_depth_functor<APM> functor(cam_center,expected_depth,vis_inf_prob);
    raytrace_fn.run(functor);
  }
  else {
    vcl_cerr << "ERROR: only perspective cameras supported at this time!" << vcl_endl;
  }

  // normalize the depth values
 normalize_expected_depth_functor norm_fn;
  vil_transform2<float,float, normalize_expected_depth_functor>(vis_inf_prob, expected_depth, norm_fn);



  return;
}



#endif

