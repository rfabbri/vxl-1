#ifndef psm_opt_bayesian_optimizer_h_
#define psm_opt_bayesian_optimizer_h_

#include <vector>
#include <string>

#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_scene.h>

#include "psm_opt_sample.h"

template<psm_apm_type APM, psm_aux_type AUX>
class psm_opt_bayesian_optimizer
{
public:

  psm_opt_bayesian_optimizer(psm_scene<APM> &scene, std::vector<std::string> const& image_ids);

  ~psm_opt_bayesian_optimizer(){}

  bool optimize_cells(double damping_factor);

protected:

  std::vector<std::string> image_ids_;

  psm_scene<APM> &scene_;

  const float max_cell_P_;
  const float min_cell_P_;

};















#endif
