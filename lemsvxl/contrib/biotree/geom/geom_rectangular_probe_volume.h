#if !defined(GEOM_RECTANGULAR_PROBE_VOLUME_H_)
#define GEOM_RECTANGULAR_PROBE_VOLUME_H_

#include "geom_probe_volume.h"


class geom_rectangular_probe_volume : public geom_probe_volume {
 private:
  vgl_box_3d<double> box_;
 public:
  geom_rectangular_probe_volume(vgl_box_3d<double> box) : box_(box) {}
  bool contains(worldpt pt){
    return box_.contains(pt);
  }
  geom_voxel_enumerator_sptr grid_cells(const vgl_box_3d<double> & grid_box, double spacing);
};

#endif
