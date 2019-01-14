#ifndef psm_opt_appearance_estimator_h_
#define psm_opt_appearance_estimator_h_

#include <vector>
#include <psm/psm_apm_traits.h>

template <psm_apm_type APM>
class psm_opt_appearance_estimator
{
public:
  static void compute_appearance(std::vector<typename psm_apm_traits<APM>::obs_datatype> const& obs, std::vector<float> const& pre, std::vector<float> const& vis, typename psm_apm_traits<APM>::apm_datatype &model);

};


#endif

