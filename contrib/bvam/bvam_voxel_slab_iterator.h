#ifndef bvam_voxel_slab_iterator_h_
#define bvam_voxel_slab_iterator_h_

#include <vcl_iterator.h>
#include <vcl_string.h>

#include <vgl/vgl_vector_3d.h>

#include "bvam_voxel_storage.h"
#include "bvam_voxel_slab.h"


//: The base class for the iterators. Not an iterator itself since it does not implement ++, --, etc.
template <class T>
class bvam_voxel_slab_iterator_base
{
public:
  bvam_voxel_slab_iterator_base() : storage_(0), slice_idx_(0), slab_thickness_(0) {};
  bvam_voxel_slab_iterator_base(bvam_voxel_storage<T> *storage, vgl_vector_3d<unsigned int> grid_size, unsigned slice_idx, unsigned slab_thickness);

  ~bvam_voxel_slab_iterator_base(){};

protected:
  bvam_voxel_storage<T> *storage_;

  unsigned slab_thickness_;
  int slice_idx_; // signed so we can move one past the beginning
 
  bvam_voxel_slab<T> slab_;
  const bvam_voxel_slab<T> end_slab_;
  vgl_vector_3d<unsigned int> grid_size_;

};

template <class T>
class bvam_voxel_slab_iterator : public bvam_voxel_slab_iterator_base<T>, public vcl_iterator<vcl_bidirectional_iterator_tag, T>
{
public:
  bvam_voxel_slab_iterator() : bvam_voxel_slab_iterator_base() {};
  bvam_voxel_slab_iterator(bvam_voxel_storage<T> *storage, vgl_vector_3d<unsigned int> grid_size, unsigned slice_idx, unsigned slab_thickness)
    : bvam_voxel_slab_iterator_base(storage,grid_size,slice_idx,slab_thickness){};

  ~bvam_voxel_slab_iterator(){/*storage_->put_slab();*/};

  bvam_voxel_slab_iterator& operator=(const bvam_voxel_slab_iterator& that);

  bool operator==(const bvam_voxel_slab_iterator& that);

  bool operator!=(const bvam_voxel_slab_iterator& that);

  bvam_voxel_slab_iterator& operator++();
  //bvam_voxel_slab_iterator& operator++(int); // postfix version

  bvam_voxel_slab_iterator& operator--();
  //bvam_voxel_slab_iterator& operator--(int); // postfix version

  bvam_voxel_slab<T>& operator*(){return slab_;}

  bvam_voxel_slab<T>* operator->(){return &slab_;}

private:


};


template <class T>
class bvam_voxel_slab_const_iterator : public bvam_voxel_slab_iterator_base<T>, public vcl_iterator<vcl_bidirectional_iterator_tag, T>
{
public:
  bvam_voxel_slab_const_iterator() : bvam_voxel_slab_iterator_base(){};

  bvam_voxel_slab_const_iterator(bvam_voxel_storage<T> *storage, vgl_vector_3d<unsigned int> grid_size, unsigned slice_idx, unsigned slab_thickness)
    : bvam_voxel_slab_iterator_base(storage,grid_size,slice_idx,slab_thickness){};

  bvam_voxel_slab_const_iterator(bvam_voxel_slab_iterator<T> &non_const_it);

  ~bvam_voxel_slab_const_iterator(){};

  bvam_voxel_slab_const_iterator& operator=(const bvam_voxel_slab_const_iterator& that);

  bool operator==(const bvam_voxel_slab_const_iterator& that);

  bool operator!=(const bvam_voxel_slab_const_iterator& that);

  bvam_voxel_slab_const_iterator& operator++();
  //bvam_voxel_slab_iterator& operator++(int); // postfix version

  bvam_voxel_slab_const_iterator& operator--();
  //bvam_voxel_slab_iterator& operator--(int); // postfix version

  bvam_voxel_slab<T> const& operator*(){return slab_;}

  bvam_voxel_slab<T> const* operator->(){return &slab_;}
  
private:


};

#endif

