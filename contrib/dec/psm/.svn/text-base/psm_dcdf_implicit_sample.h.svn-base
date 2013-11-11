#ifndef psm_dcdf_implicit_sample_h_
#define psm_dcdf_implicit_sample_h_

#include "psm_apm_traits.h"

template <class OBS_T>
class psm_dcdf_implicit_sample
{
public:
  //: default constructor
  psm_dcdf_implicit_sample() : min_dcdf_(1.0f), dcdf_sum_temp_(1.0f), obs_sum_temp_(0), PI_temp_(0.0f), seg_len_sum_temp_(0.0f) {}

  float min_dcdf_;

  // scratch variables for computing min_dcdf
  float PI_temp_;

  float dcdf_sum_temp_;
  OBS_T obs_sum_temp_;
  float seg_len_sum_temp_;

};

#endif

