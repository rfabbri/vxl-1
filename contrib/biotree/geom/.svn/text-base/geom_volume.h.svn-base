#ifndef GEOM_VOLUME_H_
#define GEOM_VOLUME_H_

#include <bio_defs.h>
#include "geom_voxel_enumerator_sptr.h"
#include <vsol/vsol_volume_3d_sptr.h>
#include "geom_probe_volume.h"

class geom_volume : public geom_probe_volume {
public:
  geom_volume(vsol_volume_3d_sptr v):volume_(v){}
  virtual ~geom_volume(){}
  bool contains(worldpt pt);
  geom_voxel_enumerator_sptr grid_cells(const vgl_box_3d<double> & grid_box, double spacing);
private:
  vsol_volume_3d_sptr volume_;
};

#endif
