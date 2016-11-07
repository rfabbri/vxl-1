#include "geom_rectangular_probe_volume.h"
#include "geom_voxel_enumerator_from_box.h"


geom_voxel_enumerator_sptr geom_rectangular_probe_volume::grid_cells(const vgl_box_3d<double> & grid_box, double spacing){
  return new geom_voxel_enumerator_from_box(box_, grid_box, spacing);
}

    
