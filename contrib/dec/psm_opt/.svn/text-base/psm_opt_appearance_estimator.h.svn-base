#ifndef psm_opt_appearance_estimator_h_
#define psm_opt_appearance_estimator_h_

#include <vcl_vector.h>
#include <psm/psm_apm_traits.h>

template <psm_apm_type APM>
class psm_opt_appearance_estimator
{
public:
  static void compute_appearance(vcl_vector<typename psm_apm_traits<APM>::obs_datatype> const& obs, vcl_vector<float> const& pre, vcl_vector<float> const& vis, typename psm_apm_traits<APM>::apm_datatype &model);

};


#endif

