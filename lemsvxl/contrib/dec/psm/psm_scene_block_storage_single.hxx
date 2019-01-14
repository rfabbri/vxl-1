#ifndef psm_scene_block_storage_single_txx_
#define psm_scene_block_storage_single_txx_

#include "psm_scene_block_storage_single.h"

#include <iostream>
#include <fstream>
#include <ios>
#include <string>

#include <vul/vul_file.h>
#include <vgl/vgl_point_3d.h>
#include <hsds/hsds_fd_tree.h>


//: destructor
template <class T>
psm_scene_block_storage_single<T>::~psm_scene_block_storage_single()
{
  put_block();
}

template <class T>
//: retrieve a block from storage
hsds_fd_tree<T,3>& psm_scene_block_storage_single<T>::get_block(vgl_point_3d<int> block_index)
{
  if ( curr_block_valid_ && (curr_block_ == block_index) ) {
    return block_;
  }
  if (curr_block_valid_) {
    put_block();
  }
  curr_block_ = block_index;
  std::string fname = this->storage_filename(block_index.x(),block_index.y(),block_index.z());
  // check if file exsist already or not
  if (vul_file::exists(fname))  {
    std::ifstream is(fname.c_str(),std::ios::binary);
    if (!is.good()) {
      std::cerr << "error opening file " << fname << " for read! " << std::endl;
    }
    else {
      block_.b_read(is);
    }
  } else {
    // file does not exist - create new block
    block_ = hsds_fd_tree<T,3>();
  }

  curr_block_valid_ = true;
  return block_;
}

//: return block to storage
template <class T>
void psm_scene_block_storage_single<T>::put_block()
{
  if (curr_block_valid_) {
    std::string fname = this->storage_filename(curr_block_.x(),curr_block_.y(),curr_block_.z());
    std::ofstream os(fname.c_str(),std::ios::binary);
    block_.b_write(os);
  }
  return;
}

#define PSM_SCENE_BLOCK_STORAGE_SINGLE_INSTANTIATE(T) \
template class psm_scene_block_storage_single<T >

#endif
