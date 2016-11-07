// This is lemsvxl/contrib/dec/psm/psm_mog_grey_processor.h
#ifndef psm_mog_grey_processor_h_
#define psm_mog_grey_processor_h_
//:
// \file
// \brief A class for a grey-scale-mixture-of-gaussian processor, shamelessly lifted from bvxm_mog_grey_processor
//
// \author Daniel Crispell
// \date 09/19/2008
// \verbatim
//  Modifications
//  
// \endverbatim


#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>


#include "psm_apm_traits.h"


class  psm_mog_grey_processor
{
protected:
  // convienance typedefs
  typedef psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype apm_datatype;
  typedef psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype obs_datatype;
  typedef psm_apm_traits<PSM_APM_MOG_GREY>::obs_mathtype obs_mathtype;

public:

  // all methods are static - no constructor needed
  //psm_mog_grey_processor(){};

  static float prob_density(apm_datatype const& appear, obs_datatype const& obs);

  static float prob_range(apm_datatype const& appear, obs_datatype const& obs_min, obs_datatype const& obs_max);

  static bool update( apm_datatype &appear, obs_datatype const& obs, float const& weight);

  static psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype expected_color(psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype const& appear);

  static psm_apm_traits<PSM_APM_MOG_GREY>::obs_mathtype most_probable_color(psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype const& appear);

  static void init_appearance(psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype const& mean, float variance, psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype &model);

  static void compute_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype> const& obs, vcl_vector<float> const& pre, vcl_vector<float> const& vis, psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype &model, float min_sigma = 0.01f);

  static void update_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype &model, float min_sigma = 0.01f);

  static void finalize_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_MOG_GREY>::apm_datatype &model, float min_sigma = 0.01f);

private:

  static void compute_gaussian_params(vcl_vector<psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype &mean, float &variance);

  static float sigma_norm_factor(unsigned int nobs);

  static float sigma_norm_factor(float nobs);


};

#endif // psm_mog_grey_processor_h_
