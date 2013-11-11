// This is lemsvxl/contrib/dec/psm/psm_simple_grey_processor.h
#ifndef psm_simple_grey_processor_h_
#define psm_simple_grey_processor_h_
//:
// \file
// \brief A class for processing simple, single valued grey-scale appearance models.
//  The represented distribution can be thought of as a delta function centered on the value. 
//
// \author Daniel Crispell
// \date 09/29/2008
// \verbatim
//  Modifications
//  
// \endverbatim



#include "psm_apm_traits.h"


class  psm_simple_grey_processor
{
protected:
  // convienance typedefs
  typedef psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_datatype apm_datatype;
  typedef psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_datatype obs_datatype;
  typedef psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype obs_mathtype;
  
  // one_over_sigma now a member of apm_datatype 
  //static const float one_over_sigma_;
public:

  // all methods are static - no constructor needed
  //psm_simple_grey_processor(){};

  static float prob_density(apm_datatype const& appear, obs_datatype const& obs);

  static float prob_range(apm_datatype const& appear, obs_datatype const& obs_min, obs_datatype const& obs_max);

  static float total_prob(apm_datatype const& appear);

  static bool update( apm_datatype &appear, obs_datatype const& obs, float const& weight);

  static psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_datatype expected_color(psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_datatype const& appear);

  static psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype most_probable_color(psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_datatype const& appear);

  static void init_appearance(psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_datatype const& mean, float variance, psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_datatype &model);

  static void compute_appearance(vcl_vector<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_datatype> const& obs, vcl_vector<float> const& pre, vcl_vector<float> const& vis, psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma = 0.01f);

  static void update_appearance(vcl_vector<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_datatype &model, float min_sigma = 0.01f);

  static void finalize_appearance(vcl_vector<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_datatype> const& obs, vcl_vector<float> const& weights, psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_datatype &model);

private:
  static void compute_gaussian_params(vcl_vector<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_datatype> obs, vcl_vector<float> weights, psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_datatype &mean, float &sigma);
 
  static float sigma_norm_factor(unsigned int nobs);

  static float sigma_norm_factor(float nobs);

};

vcl_ostream& operator<<(vcl_ostream &os, psm_simple_grey const& apm);


#endif // psm_simple_grey_processor_h_
