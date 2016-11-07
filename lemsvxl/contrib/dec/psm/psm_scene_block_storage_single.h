#ifndef psm_scene_block_storage_single_h_
#define psm_scene_block_storage_single_h_

#include "psm_scene_block_storage.h"

#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <hsds/hsds_fd_tree.h>

template <class T>
class psm_scene_block_storage_single : public psm_scene_block_storage<T>
{
public:
  //: constructor
  psm_scene_block_storage_single(vcl_string storage_dir) : psm_scene_block_storage<T>(storage_dir), curr_block_valid_(false) {}

  //: destructor
  virtual ~psm_scene_block_storage_single();

  //: retrieve a block from storage
  virtual hsds_fd_tree<T,3>& get_block(vgl_point_3d<int> block_index);

  //: return block to storage
  virtual void put_block();

private:

  bool curr_block_valid_;
  vgl_point_3d<int> curr_block_;
  hsds_fd_tree<T,3> block_;


};
                            
#endif
