#ifndef psm_aux_traits_h_
#define psm_aux_traits_h_

#include <vcl_string.h>
#include "psm_vis_implicit_sample.h"
#include "psm_update_sample.h"
#include "psm_dcdf_implicit_sample.h"

enum psm_aux_type
{
  PSM_AUX_VIS_IMPLICIT,
  PSM_AUX_UPDATE,
  PSM_AUX_OPT_GREY,
  PSM_AUX_OPT_RT_GREY,
  PSM_AUX_OPT_RGB,
  PSM_AUX_OPT_RT_RGB,
  PSM_AUX_NULL,
  PSM_AUX_DCDF_IMPLICIT_GREY,
  PSM_AUX_DCDF_IMPLICIT_RGB,
  PSM_AUX_UNKNOWN
};

//: sample properties for templates.
template <psm_aux_type>
class psm_aux_traits;


//: traits for a visibility-based implicit surface sample
template<>
class psm_aux_traits<PSM_AUX_VIS_IMPLICIT>
{
public:
  typedef psm_vis_implicit_sample sample_datatype;

  static vcl_string storage_subdir() { return "vis_implicit"; }

};

template<>
class psm_aux_traits<PSM_AUX_DCDF_IMPLICIT_GREY>
{
public:
  typedef psm_dcdf_implicit_sample<float> sample_datatype;

  static vcl_string storage_subdir() { return "dcdf_implicit"; }

};

template<>
class psm_aux_traits<PSM_AUX_DCDF_IMPLICIT_RGB>
{
public:
  typedef psm_dcdf_implicit_sample<vil_rgb<float> > sample_datatype;

  static vcl_string storage_subdir() { return "dcdf_implicit"; }
};

template<>
class psm_aux_traits<PSM_AUX_UPDATE>
{
public:
  typedef psm_update_sample sample_datatype;

  static vcl_string storage_subdir() { return "update_work";}
};



//: traits for a null sample
template<>
class psm_aux_traits<PSM_AUX_NULL>
{
public:
  typedef float sample_datatype;

  static vcl_string storage_subdir() { return "null"; }

};


#endif

