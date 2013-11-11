#ifndef psm_opt_optimizer_h_
#define psm_opt_optimizer_h_

#include <vcl_vector.h>
#include <vcl_string.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_scene.h>
//#include <psm/psm_cell_enumerator.h>

#include "psm_opt_sample.h"

template<psm_apm_type APM, psm_aux_type AUX>
class psm_opt_optimizer
{
public:

  psm_opt_optimizer(psm_scene<APM> &scene, vcl_vector<vcl_string> const& image_ids, double min_sigma);

  ~psm_opt_optimizer(){}

  bool optimize_cells();

protected:

  double min_sigma_;

  vcl_vector<vcl_string> image_ids_;

  psm_scene<APM> &scene_;

};





#endif

