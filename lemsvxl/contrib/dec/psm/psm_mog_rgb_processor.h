// This is lemsvxl/contrib/dec/psm/psm_mog_rgb_processor.h
#ifndef psm_mog_rgb_processor_h_
#define psm_mog_rgb_processor_h_
//:
// \file
// \brief A class for a rgb-mixture-of-gaussian processor
//
// \author Daniel Crispell
// \date 02/19/2009
// \verbatim
//  Modifications
//  
// \endverbatim


#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>


#include "psm_apm_traits.h"


class  psm_mog_rgb_processor
{
protected:
  // convienance typedefs
  typedef psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype apm_datatype;
  typedef psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype obs_datatype;
  typedef psm_apm_traits<PSM_APM_MOG_RGB>::obs_mathtype obs_mathtype;

public:

  // all methods are static - no constructor needed
  //psm_mog_rgb_processor(){};

  static float prob_density(apm_datatype const& appear, obs_datatype const& obs);

  static float prob_range(apm_datatype const& appear, obs_datatype const& obs_min, obs_datatype const& obs_max);

  static bool update( apm_datatype &appear, obs_datatype const& obs, float const& weight);

  static psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype expected_color(apm_datatype const& appear);

  static psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype most_probable_color(apm_datatype const& appear);

  static void psm_mog_rgb_processor::init_appearance(psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype const& mean, float variance, psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype &model);

  static void compute_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> const& obs, vcl_vector<float> const& pre, vcl_vector<float> const& vis, psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype &model, float min_sigma = 0.1f);

  static void update_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype &model, float min_sigma = 0.1f);

  static void finalize_appearance(vcl_vector<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype &model, float min_sigma = 0.1f);

private:

  static void compute_gaussian_params(vcl_vector<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> const& obs, vcl_vector<float> const& weights, vnl_vector_fixed<float,3> &mean, vnl_vector_fixed<float,3> &covar);
 
  static float sigma_norm_factor(unsigned int nobs);

  static float sigma_norm_factor(float nobs);
};

#endif // psm_mog_rgb_processor_h_
