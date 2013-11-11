#ifndef psm_render_expected_h_
#define psm_render_expected_h_

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


template <psm_apm_type APM>
class psm_render_expected_functor
{
public:

  //: default constructor
  psm_render_expected_functor(vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &expected, vil_image_view<float> &mask) : expected_(expected), vis_prob_(mask), alpha_integral_(expected.ni(),expected.nj()) 
  {
    alpha_integral_.fill(0.0f);
    vis_prob_.fill(1.0f);
    expected_.fill(typename psm_apm_traits<APM>::obs_datatype(0));
  }

  //: accumulate 
  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, psm_sample<APM> &cell_value, typename psm_aux_traits<PSM_AUX_NULL>::sample_datatype &aux_value, psm_cell_id const& cell_id)
  {
    if (cell_value.alpha <= 0.0f) {
      return true;
    }

    float cell_len = (float)(s1 - s0).length();

    // update expected value of imaged pixel
    typename psm_apm_traits<APM>::obs_datatype cell_expected = 
      //psm_apm_traits<APM>::apm_processor::most_probable_color(cell_value.appearance);
      psm_apm_traits<APM>::apm_processor::expected_color(cell_value.appearance);

    alpha_integral_(i,j) += cell_value.alpha * cell_len;
    float vis_prob_end = vcl_exp(-alpha_integral_(i,j));

    float weight = vis_prob_(i,j) - vis_prob_end;

    expected_(i,j) += cell_expected * weight;
    //vcl_cout << "cell_len = " << cell_len << "alpha = " << cell_value.alpha << " vis_prob = " << vis_prob << " weight = " << weight << vcl_endl;

    // update visibility probabilty
    vis_prob_(i,j) = vis_prob_end;

    return (vis_prob_end > 1e-4);
  }

  //typename psm_apm_traits<APM>::obs_datatype expected(){ return expected_;}
  //float mask_val(){ return vis_prob_;}


private:
 
  vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &expected_;
  vil_image_view<float> alpha_integral_;
  vil_image_view<float> &vis_prob_;

};


//: Functor class to normalize expected image
template<psm_apm_type APM>
class normalize_expected_functor
{
public:
  normalize_expected_functor(bool use_black_background) : use_black_background_(use_black_background) {}

  void operator()(float mask, typename psm_apm_traits<APM>::obs_datatype &pix) const 
  {
    if (!use_black_background_) {
      pix += mask*0.5f;
    }
  }
  bool use_black_background_;
};


template <psm_apm_type APM>
void psm_render_expected(psm_scene<APM> &scene, const vpgl_camera<double>* cam, vil_image_view<typename psm_apm_traits<APM>::obs_datatype> &expected, vil_image_view<float> &mask, bool use_black_background)
{

  //psm_scene<p> aux_scene;
  psm_raytrace_function<psm_render_expected_functor<APM>, APM> raytrace_fn(scene, cam, expected.ni(), expected.nj(), false);
  psm_render_expected_functor<APM> functor(expected,mask);
  raytrace_fn.run(functor);

  // normalize image
  normalize_expected_functor<APM> norm_fn(use_black_background);
  vil_transform2<float,typename psm_apm_traits<APM>::obs_datatype, normalize_expected_functor<APM> >(mask,expected,norm_fn);

  return;
}



#endif

