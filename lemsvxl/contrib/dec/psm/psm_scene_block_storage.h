#ifndef psm_scene_block_storage_h_
#define psm_scene_block_storage_h_

#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_3d.h>
#include <hsds/hsds_fd_tree.h>


template<class T>
class psm_scene_block_storage 
{
public:
  //: default constructor 
  psm_scene_block_storage() : storage_dir_("") {}

  //: destructor
  virtual ~psm_scene_block_storage(){}

  //: standard constructor
  psm_scene_block_storage(vcl_string storage_directory) : storage_dir_(storage_directory) {}

  //: retrieve a block from storage
  virtual hsds_fd_tree<T,3>& get_block(vgl_point_3d<int> block_index) = 0;
  //: return block to storage (does not gaurantee immediate write to disk)
  virtual void put_block() = 0;

  //: return the directory
  vcl_string storage_directory(){return storage_dir_;}

  //: return the filename of the octree at location i,j,k
  vcl_string storage_filename(int i, int j, int k) 
  {
    vcl_stringstream fname_ss;
    fname_ss << storage_dir_ << "/" << "block_" << i << "_" << j << "_" << k << ".fd3";
    return fname_ss.str();
  }

protected:
  vcl_string storage_dir_;


};



#endif
