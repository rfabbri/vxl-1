#ifndef psm_scene_block_storage_cached_h_
#define psm_scene_block_storage_cached_h_

#include "psm_scene_block_storage.h"

#include <vcl_string.h>
#include <vcl_deque.h>
#include <vcl_map.h>

#include <vgl/vgl_point_3d.h>
#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene_base.h>

template <class T>
class psm_scene_block_storage_cached : public psm_scene_block_storage<T>
{
public:
  //: constructor
  psm_scene_block_storage_cached(vcl_string storage_dir, unsigned int max_blocks) : psm_scene_block_storage<T>(storage_dir), max_blocks_(max_blocks) {}

  //: destructor
  virtual ~psm_scene_block_storage_cached();

  //: retrieve a block from storage
  virtual hsds_fd_tree<T,3>& get_block(vgl_point_3d<int> block_index);

  //: return block to storage
  virtual void put_block();

  //: write all blocks to disk
  void put_blocks();

private:
  //: write a block to disk
  void write_block(typename vcl_map<vgl_point_3d<int>, hsds_fd_tree<T,3>, vgl_point_3d_cmp<int> >::iterator it);

  const unsigned int max_blocks_;

  vcl_map<vgl_point_3d<int>, hsds_fd_tree<T,3>, vgl_point_3d_cmp<int> > blocks_;
  vcl_deque<vgl_point_3d<int> > lru_;


};
                            
#endif
