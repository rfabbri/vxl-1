#include "geom_voxel_enumerator_from_box.h"
#include <vcl_cmath.h>
#include <vcl_algorithm.h>


long longmax(long a, long b){
  return a > b ? a : b;
}

long longmin(long a, long b){
  return a > b ? b : a;
}

unsigned long get_begin(double box_min, double grid_min, double spacing){
  long i = (long) ((box_min - grid_min)/spacing); //should use static_cast?
  return i < 0 ? 0 : i;
}

unsigned long get_end(double box_max, double grid_max, double grid_min, double spacing){
  long i = 1+ (long) vcl_ceil((box_max - grid_min)/spacing);
  long j = (long) ((grid_max - grid_min)/spacing);
  return longmax(0, longmin(i,j));
}

geom_voxel_enumerator_from_box::geom_voxel_enumerator_from_box(vgl_box_3d<double> volume_box, vgl_box_3d<double> grid_box, double spacing){
  x_begin_ = get_begin(volume_box.min_x(), grid_box.min_x(), spacing);
  y_begin_ = get_begin(volume_box.min_y(), grid_box.min_y(), spacing);
  z_begin_ = get_begin(volume_box.min_z(), grid_box.min_z(), spacing);
  x_index_ = x_begin_;
  y_index_ = y_begin_;
  z_index_ = z_begin_;
  x_end_ = get_end(volume_box.max_x(), grid_box.max_x(), grid_box.min_x(), spacing);
  y_end_ = get_end(volume_box.max_y(), grid_box.max_y(), grid_box.min_y(), spacing);
  z_end_ = get_end(volume_box.max_z(), grid_box.max_z(), grid_box.min_z(), spacing);
}

geom_voxel_specifier geom_voxel_enumerator_from_box::next(){
  geom_voxel_specifier to_return = geom_voxel_specifier(x_index_, y_index_, z_index_);
  ++x_index_;
  if (x_index_ == x_end_){
    ++y_index_;
    x_index_ = x_begin_;
    if (y_index_ == y_end_){
      ++z_index_;
      y_index_ = y_begin_;
    }
  }
  return to_return;
}

