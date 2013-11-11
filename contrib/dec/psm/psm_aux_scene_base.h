#ifndef psm_aux_scene_base_h_
#define psm_aux_scene_base_h_

#include <vcl_set.h>
#include <vbl/vbl_bounding_box.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

#include <vgl/vgl_point_3d.h>

#include "psm_scene_base.h"
#include "psm_apm_traits.h"
#include "psm_aux_traits.h"

//: The untemplated psm_scene base class.
class psm_aux_scene_base : public psm_scene_base
{
public:

  //: return the appearance model type
  virtual psm_apm_type appearance_model_type() const { return PSM_APM_NA; }

  virtual psm_aux_type aux_type() const = 0;

protected:

  //: default constuctor
  psm_aux_scene_base() {}

  //: standard constructor
  psm_aux_scene_base(vgl_point_3d<double> origin, double block_len) : psm_scene_base(origin, block_len) {}


};

typedef vbl_smart_ptr<psm_aux_scene_base> psm_aux_scene_base_sptr;

//: output description of voxel world to stream.
vcl_ostream&  operator<<(vcl_ostream& s, psm_aux_scene_base const& scene);


#endif
