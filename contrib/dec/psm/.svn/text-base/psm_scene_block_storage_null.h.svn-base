#ifndef psm_scene_block_storage_null_h_
#define psm_scene_block_storage_null_h_

#include "psm_scene_block_storage.h"

#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <hsds/hsds_fd_tree.h>

template <class T>
class psm_scene_block_storage_null : public psm_scene_block_storage<T>
{
public:
  //: constructor
  psm_scene_block_storage_null() : psm_scene_block_storage<T>() , null_block_(){}

  //: retrieve a block from storage
  hsds_fd_tree<T,3>& get_block(vgl_point_3d<int> block_index){ return null_block_; }

  //: return block to storage
  virtual void put_block(){}

private:
  hsds_fd_tree<T,3> null_block_;

};
                            
#endif
