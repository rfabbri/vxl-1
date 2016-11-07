#ifndef psm_update_dcdf_implicit_h_
#define psm_update_dcdf_implicit_h_

#include <vcl_vector.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_mog_rgb_processor.h>
#include <psm/psm_simple_grey_processor.h>
#include <psm/psm_simple_rgb_processor.h>

#include <psm/algo/psm_raytrace_function.h>

#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/vil_save.h>
#include <vil/vil_crop.h>

#include <psm/psm_dcdf_implicit_sample.h>

template <psm_apm_type APM, psm_aux_type AUX>
class psm_update_dcdf_implicit_pass0_functor
{
public:
  psm_update_dcdf_implicit_pass0_functor(vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& image)
    : obs_(image)
  {}

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, psm_sample<APM> &cell_value, typename psm_aux_traits<AUX>::sample_datatype &aux_value, psm_cell_id const& cell_id)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    aux_value.obs_sum_temp_ += obs_(i,j) * seg_len;
    aux_value.seg_len_sum_temp_ += seg_len;
    return true;
  }

private:

  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& obs_;

};


template <psm_apm_type APM, psm_aux_type AUX>
class psm_update_dcdf_implicit_pass1_functor
{
public:
  //: default constructor
  psm_update_dcdf_implicit_pass1_functor(vil_image_view<float> &pre_inf, vil_image_view<float> &vis_inf) 
    : pre_img_(pre_inf), vis_img_(vis_inf), alpha_integral_(pre_inf.ni(), pre_inf.nj(), 1)
  {
    alpha_integral_.fill(0.0f);
    pre_img_.fill(0.0f);
    vis_img_.fill(1.0f);
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, psm_sample<APM> &cell_value, typename psm_aux_traits<AUX>::sample_datatype &aux_value, psm_cell_id const& cell_id)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    // compute appearance probability of observation
    const float PI  = aux_value.PI_temp_;
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = vcl_exp(-alpha_integral_(i,j));
    // compute weight for this cell
    const float Omega = vis_img_(i,j) - vis_prob_end;
    // and update pre 
    pre_img_(i,j) +=  PI * Omega;
    // update visibility probabilty
    vis_img_(i,j) = vis_prob_end;

    return true;
  }

private:

  vil_image_view<float> &vis_img_;
  vil_image_view<float> &pre_img_;
  vil_image_view<float> alpha_integral_;

};

template <psm_apm_type APM, psm_aux_type AUX>
class psm_update_dcdf_implicit_pass2_functor
{
public:
  //: default constructor
  psm_update_dcdf_implicit_pass2_functor(vil_image_view<float> const& total_prob_img) 
    : total_prob_(total_prob_img), pre_img_(total_prob_img.ni(), total_prob_img.nj(), 1), vis_img_(total_prob_img.ni(), total_prob_img.nj(), 1), alpha_integral_(total_prob_img.ni(), total_prob_img.nj(), 1)
  {
    alpha_integral_.fill(0.0f);
    pre_img_.fill(0.0f);
    vis_img_.fill(1.0f);
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, psm_sample<APM> &cell_value, typename psm_aux_traits<AUX>::sample_datatype &aux_value, psm_cell_id const& cell_id)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    // compute appearance probability of observation
    const float PI = aux_value.PI_temp_;
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = vcl_exp(-alpha_integral_(i,j));
    // grab this cell's pre and vis value
    const float pre = pre_img_(i,j);
    const float vis = vis_img_(i,j);
    // compute weight for this cell
    const float Omega = vis - vis_prob_end;
    // update vis and pre
    pre_img_(i,j) +=  PI * Omega;
    vis_img_(i,j) = vis_prob_end;

    // compute depth cdf value at s0 (cell entry point)
    float depth_cdf = pre / total_prob_(i,j);

    // accumulate depth cdf value in aux cell
    aux_value.dcdf_sum_temp_ += depth_cdf * seg_len;

    return true;
  }

private:

  vil_image_view<float> const& total_prob_;

  vil_image_view<float> vis_img_;
  vil_image_view<float> pre_img_;
  vil_image_view<float> alpha_integral_;

};

template<class OBS_T>
void fill_PI_inf_black(vil_image_view<OBS_T> const& img, vil_image_view<float> &PI_inf, OBS_T background_mean, float background_std_dev);

template<>
void fill_PI_inf_black<float>(vil_image_view<float> const& img, vil_image_view<float> &PI_inf, float background_mean, float background_std_dev)
{

  psm_simple_grey background_apm(background_mean, background_std_dev);
  vil_image_view<float>::const_iterator img_it = img.begin();
  vil_image_view<float>::iterator PI_it = PI_inf.begin();
  for (; img_it != img.end(); ++img_it, ++PI_it) {
    *PI_it = psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_processor::prob_density(background_apm,*img_it);
  }
}

template<>
void fill_PI_inf_black<vil_rgb<float> >(vil_image_view<vil_rgb<float> > const& img, vil_image_view<float> &PI_inf, vil_rgb<float> background_mean, float background_std_dev)
{
  psm_simple_rgb background_apm;
  psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_processor::init_appearance(background_mean, vcl_sqrt(background_std_dev), background_apm);

  vil_image_view<vil_rgb<float> >::const_iterator img_it = img.begin();
  vil_image_view<float>::iterator PI_it = PI_inf.begin();
  for (; img_it != img.end(); ++img_it, ++PI_it) {
    *PI_it = psm_apm_traits<PSM_APM_SIMPLE_RGB>::apm_processor::prob_density(background_apm,*img_it);
  }
}

template <psm_apm_type APM, psm_aux_type AUX>
void psm_update_dcdf_implicit(psm_scene<APM> &scene, psm_aux_scene_base_sptr aux_scene_ptr, vpgl_camera<double> const* cam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> const& img, bool black_background = false)
{
 
  // 0th pass: traverse and compute mean cell observations.
  vcl_cout << "compute mean cell observations.." << vcl_endl;
  psm_raytrace_function<psm_update_dcdf_implicit_pass0_functor<APM,AUX>, APM, AUX> raytrace_fn0(scene, aux_scene_ptr, cam, img.ni(), img.nj(), false);
  psm_update_dcdf_implicit_pass0_functor<APM,AUX> pass0_functor(img);
  raytrace_fn0.run(pass0_functor);

  vcl_cout << "normalizing and computing PI values.." << vcl_endl;
  // normalize appearance values
  psm_aux_scene<AUX> *aux_scene = static_cast<psm_aux_scene<AUX>*>(aux_scene_ptr.ptr());
  typename psm_aux_scene<AUX>::block_index_iterator block_index_it = scene.block_index_begin();
  for (; block_index_it != scene.block_index_end(); ++block_index_it) {
    hsds_fd_tree<psm_sample<APM>,3> &block = scene.get_block(*block_index_it);
    hsds_fd_tree<typename psm_aux_traits<AUX>::sample_datatype,3> &aux_block = aux_scene->get_block(*block_index_it);
    // iterate through cells in block
    typename hsds_fd_tree<psm_sample<APM>,3>::iterator block_it = block.begin();
    typename hsds_fd_tree<typename psm_aux_traits<AUX>::sample_datatype,3>::iterator aux_block_it = aux_block.begin();
    for (; aux_block_it != aux_block.end(); ++aux_block_it, ++block_it) {
      const float total_seg_len = aux_block_it->second.seg_len_sum_temp_;
      if (total_seg_len > 0) {
        // this line will give a compile error if aux type and appearance type observation data types do not match up
        typename psm_apm_traits<APM>::obs_datatype obs  = aux_block_it->second.obs_sum_temp_ / total_seg_len;
        aux_block_it->second.PI_temp_ = psm_apm_traits<APM>::apm_processor::prob_density(block_it->second.appearance, obs);
        //aux_block_it->second.seg_len_ = 0;
      }
    }
  }

  // first pass: traverse in forward direction, accumulating vis_inf and pre_inf
  vcl_cout << "first pass.." << vcl_endl;
  vil_image_view<float> vis_inf(img.ni(), img.nj());
  vil_image_view<float> pre_inf(img.ni(), img.nj());
  psm_raytrace_function<psm_update_dcdf_implicit_pass1_functor<APM,AUX>, APM, AUX> raytrace_fn1(scene, aux_scene_ptr, cam, img.ni(), img.nj(), false);
  psm_update_dcdf_implicit_pass1_functor<APM,AUX> pass1_functor(pre_inf, vis_inf);
  raytrace_fn1.run(pass1_functor);

  vil_image_view<float> PI_inf(img.ni(), img.nj());
  if (black_background) {
    vcl_cout << "using black background" << vcl_endl;
    psm_apm_traits<APM>::obs_datatype black(0.0f);
    float background_std_dev = 8.0f/255;

    fill_PI_inf_black(img,PI_inf,black,background_std_dev);
  }
  else {
    PI_inf.fill(1.0f); // uniform distribution [0, 1]
  }

  vil_image_view<float> inf_term(img.ni(), img.nj());
  vil_math_image_product(vis_inf,PI_inf,inf_term);
  vil_image_view<float> total_prob_img(img.ni(), img.nj());
  vil_math_image_sum(pre_inf,inf_term, total_prob_img);

  vcl_cout << "second pass.." << vcl_endl;
  psm_raytrace_function<psm_update_dcdf_implicit_pass2_functor<APM,AUX>, APM, AUX> raytrace_fn2(scene, aux_scene_ptr, cam, img.ni(), img.nj(), false);
  psm_update_dcdf_implicit_pass2_functor<APM,AUX> pass2_functor(total_prob_img);
  raytrace_fn2.run(pass2_functor);

  // normalize dcdf values and compare to stored min, replacing if necessary

  for (block_index_it = scene.block_index_begin(); block_index_it != scene.block_index_end(); ++block_index_it) {
    hsds_fd_tree<psm_sample<APM>,3> &block = scene.get_block(*block_index_it);
    hsds_fd_tree<typename psm_aux_traits<AUX>::sample_datatype,3> &aux_block = aux_scene->get_block(*block_index_it);
    // iterate through cells in block
    typename hsds_fd_tree<psm_sample<APM>,3>::iterator block_it = block.begin();
    typename hsds_fd_tree<typename psm_aux_traits<AUX>::sample_datatype,3>::iterator aux_block_it = aux_block.begin();
    for (; aux_block_it != aux_block.end(); ++aux_block_it, ++block_it) {
      const float total_seg_len = aux_block_it->second.seg_len_sum_temp_;
      if (total_seg_len > 0) {
        // this line will give a compile error if aux type and appearance type observation data types do not match up
        float dcdf  = aux_block_it->second.dcdf_sum_temp_ / total_seg_len;
        if (dcdf < aux_block_it->second.min_dcdf_) {
          aux_block_it->second.min_dcdf_ = dcdf;
        }
      }
    }
  }

  vcl_cout << "done." << vcl_endl;

  return;
}

#endif

