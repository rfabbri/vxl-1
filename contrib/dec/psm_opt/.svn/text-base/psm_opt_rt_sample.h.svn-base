#ifndef psm_opt_rt_sample_h_
#define psm_opt_rt_sample_h_

#include <vil/vil_rgb.h>
#include <psm/psm_aux_traits.h>

template<class OBS_T>
class psm_opt_rt_sample
{
public:
  psm_opt_rt_sample() : obs_(0), pre_(0.0f), vis_(0.0f), PI_(0.0f), seg_len_(0.0f), weighted_seg_len_(0.0f), updated_alpha_sum_(0.0f) {}

  OBS_T obs_;
  float pre_;
  float vis_;
  float PI_;
  float seg_len_;
  float weighted_seg_len_;
  float updated_alpha_sum_;
};

//: traits for a grey optimization sample
template<>
class psm_aux_traits<PSM_AUX_OPT_RT_GREY>
{
public:
  typedef psm_opt_rt_sample<float> sample_datatype;

  static vcl_string storage_subdir() { return "opt_rt_grey_work"; }

};

//: traits for an rgb optimization sample
template<>
class psm_aux_traits<PSM_AUX_OPT_RT_RGB>
{
public:
  typedef psm_opt_rt_sample<vil_rgb<float> > sample_datatype;

  static vcl_string storage_subdir() { return "opt_rt_rgb_work"; }

};


#endif

