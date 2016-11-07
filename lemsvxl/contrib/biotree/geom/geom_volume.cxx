#include "geom_volume.h"
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_box_3d_sptr.h>
#include <vgl/vgl_box_3d.h>
#include <vsol/vsol_volume_3d.h>
#include <vcl_algorithm.h>
#include "geom_voxel_enumerator_from_box.h"

bool geom_volume::contains(worldpt pt)
{
  return volume_->in(new vsol_point_3d(pt));
}

geom_voxel_enumerator_sptr 
geom_volume::grid_cells(const vgl_box_3d<double> & grid_box, double spacing)
{
  //vsol_box_3d<double> box = *(volume_->get_bounding_box());
  return new geom_voxel_enumerator_from_box(vgl_box_3d<double>(volume_->get_min_x(), volume_->get_min_y(), volume_->get_min_z(),
                                                               volume_->get_max_x(), volume_->get_max_y(), volume_->get_max_z()),
                                            grid_box, spacing);
}
