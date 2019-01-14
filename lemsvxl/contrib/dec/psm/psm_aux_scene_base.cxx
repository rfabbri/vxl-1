#include <set>
#include <cmath>
#include <iostream>

#include <vgl/vgl_point_3d.h>
#include <vbl/vbl_bounding_box.h>

#include "psm_aux_scene_base.h"





//: output description of voxel world to stream.
std::ostream&  operator<<(std::ostream& s, psm_aux_scene_base const& scene)
{
  s << "psm_aux_scene_base: aux_type = " << scene.aux_type() << " origin = " << scene.origin() <<", block_len = " << scene.block_len();

  return s;
}

