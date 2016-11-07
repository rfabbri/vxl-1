#ifndef psm_aux_scene_h_
#define psm_aux_scene_h_

#include <vcl_string.h>
#include <vcl_set.h>
#include <vgl/vgl_point_3d.h>
#include <hsds/hsds_fd_tree.h>
#include <hsds/hsds_fd_tree_incremental_reader.h>

//#include "psm_scene.h"
#include "psm_aux_scene_base.h"
#include "psm_apm_traits.h"
#include "psm_aux_traits.h"

#include "psm_scene_block_storage.h"
#include "psm_scene_block_storage_single.h"
#include "psm_scene_block_storage_null.h"



template <psm_aux_type AUX_T>
class psm_aux_scene : public psm_aux_scene_base
{
public:

  typedef typename psm_aux_traits<AUX_T>::sample_datatype sample_datatype;

  //: default constructor
  psm_aux_scene() : block_storage_(new psm_scene_block_storage_null<sample_datatype>()) {}

  //: destructor
  ~psm_aux_scene() { delete block_storage_; }

  //: constructor
  psm_aux_scene(vgl_point_3d<double> origin, double block_len, vcl_string storage_dir)
    : psm_aux_scene_base(origin, block_len), block_storage_(new psm_scene_block_storage_single<sample_datatype>(storage_dir)) 
  { 
    // discover blocks already on disk
    discover_blocks(); 
  }

  //: get a scene block
  hsds_fd_tree<sample_datatype,3>& get_block(vgl_point_3d<int> block_idx);

  //: get an incremental reader for a scene block
  void get_block_incremental(vgl_point_3d<int> block_idx, hsds_fd_tree_incremental_reader<sample_datatype,3> &reader);

  //: set a particular block to the specified block
  void set_block(vgl_point_3d<int> block_idx, hsds_fd_tree<typename psm_aux_traits<AUX_T>::sample_datatype ,3> &data);

  void init_block(vgl_point_3d<int> block_idx, unsigned int block_level);

  //: access storage dir
  vcl_string storage_dir(){return block_storage_->storage_directory();}

  //: return auxillary sample type
  virtual psm_aux_type aux_type() const {return AUX_T;}

private:
  //: discover already existing blocks on disk
  void discover_blocks();

  psm_scene_block_storage<sample_datatype > *block_storage_;


};


#endif
