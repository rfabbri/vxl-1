#ifndef psm_ray_probe_h_
#define psm_ray_probe_h_

#include <vector>
#include <psm/psm_scene.h>
#include <psm/psm_cell_id.h>
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
#include <psm/psm_cell_id.h>

#include "psm_raytrace_function.h"


template<psm_apm_type APM>
class psm_ray_probe_functor
{
public:

  //: default constructor
  psm_ray_probe_functor(vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& img, vgl_point_3d<double> camera_center, std::vector<float> &depth_vals, std::vector<float> &alpha_vals, std::vector<float> &appearance_prob_vals, std::vector<psm_cell_id> &cell_ids) : depth_vals_(depth_vals), alpha_vals_(alpha_vals), appearance_prob_vals_(appearance_prob_vals), cell_ids_(cell_ids), camera_center_(camera_center), img_(img)
  {
    appearance_prob_vals_.clear();
    alpha_vals_.clear();
    depth_vals_.clear();
    cell_ids_.clear();
  }

  //: accumulate 
  inline bool step_cell(float i, float j, vgl_point_3d<double> p0, vgl_point_3d<double> p1, psm_sample<APM> const& cell_value, typename psm_aux_traits<PSM_AUX_NULL>::sample_datatype &aux_value, psm_cell_id const& cell_index)
  {

    const float s0 = (float)(camera_center_ - p0).length();

    depth_vals_.push_back(s0);
    alpha_vals_.push_back(cell_value.alpha);
    const float cell_PI = psm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance, img_((unsigned int)i,(unsigned int)j));
    appearance_prob_vals_.push_back(cell_PI);
    cell_ids_.push_back(cell_index);

    //std::cout << "s0 = " << s0 << std::endl;
    //std::cout << "s1 = " << (float)(camera_center_ - p1).length() << std::endl;

    return true;
  }

private:

  vgl_point_3d<double> camera_center_;
  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& img_;

  std::vector<float> &alpha_vals_;
  std::vector<float> &depth_vals_;
  std::vector<float> &appearance_prob_vals_;
  std::vector<psm_cell_id> &cell_ids_;


};


template <psm_apm_type APM>
void psm_ray_probe(psm_scene<APM> &scene, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& img, const vpgl_camera<double>* cam, float i, float j, std::vector<float> &depth_vals, std::vector<float> &alpha_vals, std::vector<float> &appearance_prob_vals, std::vector<psm_cell_id> &cell_ids)
{
  if (i < 0.0f) {
    std::cerr << " error psm_ray_probe: i = " << i << std::endl;
    return;
  }
  if (j < 0.0f) {
    std::cerr << " error: psm_ray_probe: j = " << j << std::endl;
    return;
  }

  psm_raytrace_function<psm_ray_probe_functor<APM>, APM> raytrace_fn(scene, cam, 1, 1, false, (unsigned int)i, (unsigned int)j);
  if (const vpgl_perspective_camera<double>* pcam = dynamic_cast<const vpgl_perspective_camera<double>*>(cam)) {
    vgl_point_3d<double> cam_center(pcam->camera_center());
    psm_ray_probe_functor<APM> functor(img, cam_center, depth_vals, alpha_vals, appearance_prob_vals, cell_ids);
    raytrace_fn.run_single(functor,i,j);
  }
  else {
    std::cerr << "ERROR: only perspective cameras supported at this time!" << std::endl;
  }

  return;
}



#endif

