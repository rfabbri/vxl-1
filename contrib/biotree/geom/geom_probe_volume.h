#if !defined(GEOM_PROBE_VOLUME_H_)
#define GEOM_PROBE_VOLUME_H_

#include <bio_defs.h>
#include <vgl/vgl_box_3d.h>
#include "geom_voxel_enumerator_sptr.h"
#include <vbl/vbl_ref_count.h>

/* Base class for objects by which you can query a geometric index structure
   Philip Klein
 */

class geom_probe_volume : public vbl_ref_count {
 public:
  ~geom_probe_volume(){}
  virtual bool contains(worldpt pt) = 0;
  virtual geom_voxel_enumerator_sptr grid_cells(const vgl_box_3d<double> & grid_box, double spacing) = 0;
};

#endif
