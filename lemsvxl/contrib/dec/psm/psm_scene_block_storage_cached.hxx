#ifndef psm_scene_block_storage_cached_txx_
#define psm_scene_block_storage_cached_txx_

#include "psm_scene_block_storage.h"
#include "psm_scene_block_storage_cached.h"

#include <iostream>
#include <fstream>
#include <ios>
#include <string>

#include <map>
#include <utility>
#include <deque>
#include <algorithm>

#include <vul/vul_file.h>
#include <vgl/vgl_point_3d.h>
#include <hsds/hsds_fd_tree.h>


//: destructor
template <class T>
psm_scene_block_storage_cached<T>::~psm_scene_block_storage_cached()
{
  put_blocks();
}

template <class T>
//: retrieve a block from storage
hsds_fd_tree<T,3>& psm_scene_block_storage_cached<T>::get_block(vgl_point_3d<int> block_index)
{
  typename std::map<vgl_point_3d<int>, hsds_fd_tree<T,3>, vgl_point_3d_cmp<int> >::iterator map_it;

  // check if block_index is already in lru_
  typename std::deque<vgl_point_3d<int> >::iterator it = find(lru_.begin(), lru_.end(), block_index);
  if (it == lru_.end()) {
    //std::cout << "miss" << std::endl;
    // cache miss
    if (lru_.size() >= max_blocks_) {
      // need to remove an item from the cache
      vgl_point_3d<int> back_index = lru_.back();
      typename std::map<vgl_point_3d<int>, hsds_fd_tree<T,3>, vgl_point_3d_cmp<int> >::iterator map_it =
        blocks_.find(back_index);
      if (map_it == blocks_.end()) {
        // this should not happen.
        std::cerr << "error: psm_scene_block_storage_cached: index is in lru_ but not blocks_!" << std::endl;
      }
      // write to disk
     write_block(map_it);
      // remove from map
      blocks_.erase(map_it);
      // remove from lru
      lru_.erase(lru_.end() - 1);
    }
    // insert new block into the cache
    lru_.insert(lru_.begin(),block_index);
    blocks_.insert(std::make_pair<vgl_point_3d<int>,hsds_fd_tree<T,3> >(block_index, hsds_fd_tree<T,3>()));
    map_it = blocks_.find(block_index);
    std::string fname = this->storage_filename(block_index.x(),block_index.y(),block_index.z());
    // check if file exists already or not
    if (vul_file::exists(fname))  {
      std::ifstream is(fname.c_str(),std::ios::binary);
      if (!is.good()) {
        std::cerr << "error opening file " << fname << " for read! " << std::endl;
      }
      else {
        // file exists: read from disk.
        map_it->second.b_read(is);
      }
    }
  }
  else {
    //std::cout << "hit" << std::endl;
    // block is in cache
    // move index to back of deque
    lru_.erase(it);
    lru_.insert(lru_.begin(), block_index);
    // find block in map
    map_it = blocks_.find(block_index);
    if (map_it == blocks_.end()) {
      // this should not happen.
      std::cerr << "error: psm_scene_block_storage_cached: block_index is in lru_ but not blocks_!" << std::endl;
    }
  }
  return map_it->second;

}


//: return block to storage
template <class T>
void psm_scene_block_storage_cached<T>::put_block() {}

template <class T>
void psm_scene_block_storage_cached<T>::put_blocks()
{
  typename std::map<vgl_point_3d<int>, hsds_fd_tree<T,3>, vgl_point_3d_cmp<int> >::iterator it = blocks_.begin();
  for (; it != blocks_.end(); ++it) {
    write_block(it);
  }
  return;
}

template <class T>
void psm_scene_block_storage_cached<T>::write_block(typename std::map<vgl_point_3d<int>, hsds_fd_tree<T,3>, vgl_point_3d_cmp<int> >::iterator it)
{
  vgl_point_3d<int> block_idx = it->first;
  std::string fname = this->storage_filename(block_idx.x(),block_idx.y(),block_idx.z());
  std::ofstream os(fname.c_str(),std::ios::binary);
  it->second.b_write(os);
}


#define PSM_SCENE_BLOCK_STORAGE_CACHED_INSTANTIATE(T) \
  template class psm_scene_block_storage_cached<T >

#endif
