#ifndef bvam_voxel_storage_mem_txx_
#define bvam_voxel_storage_mem_txx_

#include "bvam_voxel_storage.h"
#include "bvam_voxel_storage_mem.h"
#include "bvam_memory_chunk.h"

#include <vgl/vgl_vector_3d.h>

template <class T>
bvam_voxel_storage_mem<T>::bvam_voxel_storage_mem(vgl_vector_3d<unsigned int> grid_size) : bvam_voxel_storage(grid_size)
{
  mem_ = new bvam_memory_chunk(grid_size.x() * grid_size.y() * grid_size.z() * sizeof(T));

}

template <class T>
bool bvam_voxel_storage_mem<T>::initialize_data(const T& value)
{
  // interpret entire grid as a slab and fill with data.
  bvam_voxel_slab<T> grid_slab(grid_size_.x(),grid_size_.y(),grid_size_.z(),mem_,reinterpret_cast<T*>(mem_->data()));
  grid_slab.fill(value);
  nobservations_ = 0;

  return true;
}


template <class T>
bvam_voxel_slab<T> bvam_voxel_storage_mem<T>::get_slab(unsigned slice_idx, unsigned slab_thickness)
{
  unsigned long slice_size = grid_size_.x() * grid_size_.y();
  unsigned long mem_offset = slice_size * slice_idx;

  T* first_voxel = static_cast<T*>(mem_->data()) + mem_offset;

  unsigned slab_thickness_actual = slab_thickness;
  if (slice_idx + slab_thickness > (unsigned)(grid_size_.z() - 1)) {
    slab_thickness_actual = grid_size_.z() - slice_idx;
  }
  bvam_voxel_slab<T> slab(grid_size_.x(),grid_size_.y(),slab_thickness_actual,mem_, first_voxel);

  
  return slab;
}

template <class T>
void bvam_voxel_storage_mem<T>::put_slab()
{
  // nothing to do here since we are operating on the voxel memory itself.
  return;
}




#define BVAM_VOXEL_STORAGE_MEM_INSTANTIATE(T)\
template class bvam_voxel_storage_mem<T >; 

#endif

