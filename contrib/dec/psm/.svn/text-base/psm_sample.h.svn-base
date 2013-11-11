#ifndef psm_sample_h_
#define psm_sample_h_

#include "psm_apm_traits.h"



template <psm_apm_type APM_MODEL>
class psm_sample
{
public:
  static const psm_apm_type apm_type = APM_MODEL;
  typedef typename psm_apm_traits<APM_MODEL>::apm_datatype apm_datatype;
  typedef typename psm_apm_traits<APM_MODEL>::obs_datatype obs_datatype;
  typedef typename psm_apm_traits<APM_MODEL>::obs_mathtype obs_mathtype;

  //: default constructor
  psm_sample() : alpha(0.001f), appearance() {}
  //: alpha-only constructor
  psm_sample(float alpha_val) : alpha(alpha_val), appearance() {}
  //: full constructor
  psm_sample(float alpha_val, apm_datatype apm_val): alpha(alpha_val), appearance(apm_val) {}

  //: the occlusion density at the sample point
  float alpha;
  //: the appearance model at the sample point
  typename psm_apm_traits<APM_MODEL>::apm_datatype appearance;

};


#endif

