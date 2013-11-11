#ifndef bvam_voxel_slab_iterator_txx_
#define bvam_voxel_slab_iterator_txx_

#include "bvam_voxel_slab_iterator.h"
#include "bvam_voxel_storage.h"
#include "bvam_voxel_slab.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include <vgl/vgl_vector_3d.h>

///////////////////// bvam_voxel_slab_iterator_base methods //////////////////////////////

template <class T>
bvam_voxel_slab_iterator_base<T>::bvam_voxel_slab_iterator_base(bvam_voxel_storage<T> *storage, vgl_vector_3d<unsigned int> grid_size, unsigned slice_idx, unsigned slab_thickness)
: storage_(storage), slab_thickness_(slab_thickness), slice_idx_(slice_idx), end_slab_(0,0,0,0,0), grid_size_(grid_size)
{
  if (slab_thickness != 1) {
    vcl_cerr << "error: only slabs of thickness 1 are currently supported.\n";
    assert(slab_thickness == 1);
  }
  if ( (slice_idx < 0) || (slice_idx >= grid_size_.z()) ) {
    slab_ = this->end_slab_;
  }
  else {
  slab_ = storage_->get_slab(slice_idx, slab_thickness);
  }
}


///////////////////// bvam_voxel_slab_iterator methods //////////////////////////////


template <class T>
bvam_voxel_slab_iterator<T>& bvam_voxel_slab_iterator<T>::operator=(const bvam_voxel_slab_iterator<T>& that)
{
  storage_ = that.storage_;
  slab_thickness_ = that.slab_thickness_;
  slab_ = that.slab_;
  grid_size_ = that.grid_size_;
  slice_idx_ = that.slice_idx_;

  return *this;
}

template <class T>
bool bvam_voxel_slab_iterator<T>::operator==(const bvam_voxel_slab_iterator<T>& that)
{
  return slab_ == that.slab_;
}


template <class T>
bool bvam_voxel_slab_iterator<T>::operator!=(const bvam_voxel_slab_iterator<T>& that)
{
  return slab_ != that.slab_;
}

template <class T>
bvam_voxel_slab_iterator<T>& bvam_voxel_slab_iterator<T>::operator++()
{
  // commit prev slice to memory
  storage_->put_slab();

  ++slice_idx_;
  if (slice_idx_ >= static_cast<int>(grid_size_.z())) {
    slab_ = this->end_slab_;
  }
  else {
    slab_ = storage_->get_slab(slice_idx_,slab_thickness_);
  }
  return *this;
}



template <class T>
bvam_voxel_slab_iterator<T>& bvam_voxel_slab_iterator<T>::operator--()
{
  // commit old slice to memory
  storage_->put_slab();

  --slice_idx_;
  if (slice_idx_ < 0) {
    slab_ = this->end_slab_;
  } else {
    slab_ = storage_->get_slab(slice_idx_,slab_thickness_);
  }
  return *this;
}
///////////////////// bvam_voxel_slab_const_iterator methods //////////////////////////////

template <class T>
bvam_voxel_slab_const_iterator<T>::bvam_voxel_slab_const_iterator(bvam_voxel_slab_iterator<T> &non_const_it)
: bvam_voxel_slab_iterator_base(static_cast<bvam_voxel_slab_iterator_base<T>&>(non_const_it))
{}
  



template <class T>
bvam_voxel_slab_const_iterator<T>& bvam_voxel_slab_const_iterator<T>::operator=(const bvam_voxel_slab_const_iterator<T>& that)
{
  storage_ = that.storage_;
  slab_thickness_ = that.slab_thickness_;
  slab_ = that.slab_;
  grid_size_ = that.grid_size_;
  slice_idx_ = that.slice_idx_;

  return *this;
}

template <class T>
bool bvam_voxel_slab_const_iterator<T>::operator==(const bvam_voxel_slab_const_iterator<T>& that)
{
  return slab_ == that.slab_;
}


template <class T>
bool bvam_voxel_slab_const_iterator<T>::operator!=(const bvam_voxel_slab_const_iterator<T>& that)
{
  return slab_ != that.slab_;
}

template <class T>
bvam_voxel_slab_const_iterator<T>& bvam_voxel_slab_const_iterator<T>::operator++()
{
  // no need to commit prev slice to memory
  //storage_->put_slab();

  ++slice_idx_;
  if (slice_idx_ >= static_cast<int>(grid_size_.z())) {
    slab_ = this->end_slab_;
  }
  else {
    slab_ = storage_->get_slab(slice_idx_,slab_thickness_);
  }
  return *this;
}



template <class T>
bvam_voxel_slab_const_iterator<T>& bvam_voxel_slab_const_iterator<T>::operator--()
{
  // no need to commit old slice to memory
  //storage_->put_slab();

  --slice_idx_;
  if (slice_idx_ < 0) {
    slab_ = this->end_slab_;
  } else {
    slab_ = storage_->get_slab(slice_idx_,slab_thickness_);
  }
  return *this;
}

#define BVAM_VOXEL_SLAB_ITERATOR_INSTANTIATE(T) \
template class bvam_voxel_slab_iterator<T >; \
template class bvam_voxel_slab_const_iterator<T>;







#endif
