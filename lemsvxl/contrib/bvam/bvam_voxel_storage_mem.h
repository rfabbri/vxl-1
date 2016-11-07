#ifndef bvam_voxel_storage_mem_h_
#define bvam_voxel_storage_mem_h_

#include "bvam_voxel_storage.h"

#include <vgl/vgl_vector_3d.h>

template <class T>
class bvam_voxel_storage_mem : public bvam_voxel_storage<T>
{
public:
  bvam_voxel_storage_mem(vgl_vector_3d<unsigned int> grid_size);

  virtual bool initialize_data(T const& value);
  virtual bvam_voxel_slab<T> get_slab(unsigned slice_idx, unsigned slab_thickness);
  virtual void put_slab();

  //: return number of observations
  virtual unsigned num_observations(){return nobservations_;}
  //: increment the number of observations
  virtual void increment_observations(){++nobservations_;}


private:

  bvam_memory_chunk_sptr mem_;
  unsigned nobservations_;

};
                          
                         
#endif
