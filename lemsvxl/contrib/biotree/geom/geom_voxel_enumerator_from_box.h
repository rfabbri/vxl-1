#if !defined(GEOM_VOXEL_ENUMERATOR_FROM_BOX_H)
#define GEOM_VOXEL_ENUMERATOR_FROM_BOX_H

#include "geom_voxel_enumerator.h"
#include <vgl/vgl_box_3d.h>

class geom_voxel_enumerator_from_box : public geom_voxel_enumerator {
 private:
  unsigned long x_index_, y_index_, z_index_,
    x_begin_, y_begin_, z_begin_,
    x_end_, y_end_, z_end_;
 public:
  geom_voxel_enumerator_from_box(vgl_box_3d<double> volume_box, vgl_box_3d<double> grid_box, double spacing);
  bool has_next(){
    return x_index_ < x_end_ && y_index_ < y_end_ && z_index_ < z_end_;
  }
  geom_voxel_specifier next();
};


#endif
