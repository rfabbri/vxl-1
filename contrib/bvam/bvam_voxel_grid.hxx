#ifndef bvam_voxel_grid_txx_
#define bvam_voxel_grid_txx_


#include "bvam_voxel_grid.h"
#include "bvam_voxel_slab.h"
#include "bvam_voxel_slab_iterator.h"


template <class T>
bvam_voxel_slab_iterator<T> bvam_voxel_grid<T>::slab_iterator(unsigned slice_idx, unsigned slab_thickness)
{
  bvam_voxel_slab_iterator<T> slab_it(storage_ ,grid_size_, slice_idx ,slab_thickness);

  return slab_it;
}

// note: since we usually want to traverse top down, begin() means the top-most slab
template <class T>
bvam_voxel_slab_iterator<T> bvam_voxel_grid<T>::begin(unsigned slab_thickness)
{
  bvam_voxel_slab_iterator<T> slab_it(storage_, grid_size_, 0, slab_thickness);

  return slab_it;
}

// note: traversing top-down, end() is one slab past the bottom.
template <class T>
bvam_voxel_slab_iterator<T> bvam_voxel_grid<T>::end(unsigned slab_thickness = 1)
{
  bvam_voxel_slab_iterator<T> slab_it(storage_, grid_size_, grid_size_.z(), slab_thickness);

  return slab_it;
}


#define BVAM_VOXEL_GRID_INSTANTIATE(T)\
template class bvam_voxel_grid<T >; 




#endif
