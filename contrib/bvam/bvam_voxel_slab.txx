#ifndef bvam_voxel_slab_txx_
#define bvam_voxel_slab_txx_
//:
// \file
// \brief A 3-D slab of data of type T.  Based loosely on vil_image_view.
// \author Daniel Crispell (dec@lems.brown.edu)
// \date 1/22/2008
//


#include "bvam_voxel_slab.h"
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_double_3.h>

#include <vcl_cassert.h>
#include <vcl_cstring.h> // for memcpy
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>


//=======================================================================

//: Constructor allocating new memory
template<class T>
bvam_voxel_slab<T>::bvam_voxel_slab(unsigned nx, unsigned ny, unsigned nz)
: bvam_voxel_slab_base(nx,ny,nz)
{
  mem_ = new bvam_memory_chunk(nx*ny*nz*sizeof(T));
  first_voxel_ = static_cast<T*>(mem_->data());
}

//: Copy constructor
template<class T>
bvam_voxel_slab<T>::bvam_voxel_slab(const bvam_voxel_slab<T>& that)
: mem_(0)
{
  operator=( that );
}



//: Fill view with given value
template<class T>
void bvam_voxel_slab<T>::fill(T const& value)
{
  vcl_fill(begin(), end(), value);
  return;
}


//: True if they share same memory
template<class T>
bool bvam_voxel_slab<T>::operator==(const bvam_voxel_slab &rhs) const
{
  
  if (this == &rhs) return true;

  return    mem_ == rhs.mem_ &&
    first_voxel_ == rhs.first_voxel_ &&
            nx_  == rhs.nx_ &&
            ny_  == rhs.ny_ &&
            nz_  == rhs.nz_;
}

//: deep copy data in slab
template<class T>
void bvam_voxel_slab<T>::deep_copy(bvam_voxel_slab<T> const& src)
{
  set_size(src.nx(),src.ny(),src.nz());

  vcl_memcpy(this->first_voxel_,src.first_voxel(),src.size()*sizeof(T));
  return;
}

//: change the size of the slab
template<class T>
void bvam_voxel_slab<T>::set_size(unsigned nx, unsigned ny, unsigned nz)
{
  if ((nx_ == nx) && (ny_ == ny) && (nz_ == nz)) {
    return;
  }
  release_memory();
  mem_ = new bvam_memory_chunk(nx*ny*nz*sizeof(T));
  first_voxel_ = static_cast<T*>(mem_->data());
  nx_ = nx;
  ny_ = ny;
  nz_ = nz;

  return;
}


#define BVAM_VOXEL_SLAB_INSTANTIATE(T) \
template class bvam_voxel_slab<T >; 

#endif // vil_image_view_txx_

