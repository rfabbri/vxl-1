#ifndef psm_aux_scene_txx_
#define psm_aux_scene_txx_

#include <vbl/vbl_bounding_box.h>
#include <vul/vul_file_iterator.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>

#include <hsds/hsds_fd_tree.h>
#include <hsds/hsds_fd_tree_incremental_reader.h>

#include "psm_aux_traits.h"
#include "psm_aux_scene.h"



template <psm_aux_type AUX_T>
hsds_fd_tree<typename psm_aux_scene<AUX_T>::sample_datatype,3>& psm_aux_scene<AUX_T>::get_block(vgl_point_3d<int> block_idx)
{
  return block_storage_->get_block(block_idx);
}

//: get an incremental reader for a scene block
template <psm_aux_type AUX_T> 
void psm_aux_scene<AUX_T>::get_block_incremental(vgl_point_3d<int> block_idx, hsds_fd_tree_incremental_reader<typename psm_aux_scene<AUX_T>::sample_datatype,3>& reader)
{
  vcl_string block_fname = block_storage_->storage_filename(block_idx.x(), block_idx.y(), block_idx.z());
  reader.init(block_fname);
}

template <psm_aux_type AUX_T>
void psm_aux_scene<AUX_T>::set_block(vgl_point_3d<int> block_idx, hsds_fd_tree<typename psm_aux_traits<AUX_T>::sample_datatype ,3> &data)
{
  hsds_fd_tree<typename psm_aux_traits<AUX_T>::sample_datatype ,3> &block = block_storage_->get_block(block_idx);
  block = data;
  block_storage_->put_block();
}

template <psm_aux_type AUX_T>
void psm_aux_scene<AUX_T>::init_block(vgl_point_3d<int> block_idx, unsigned int block_level)
{
  hsds_fd_tree<sample_datatype ,3> &block = block_storage_->get_block(block_idx);
  // initialize bounding box
  vbl_bounding_box<double,3> bbox;
  bbox.update(origin_.x() + block_idx.x() * block_len_,
              origin_.y() + block_idx.y() * block_len_,
              origin_.z() + block_idx.z() * block_len_);
  bbox.update(origin_.x() + (block_idx.x()+1) * block_len_,
              origin_.y() + (block_idx.y()+1) * block_len_,
              origin_.z() + (block_idx.z()+1) * block_len_);
  block = hsds_fd_tree<sample_datatype ,3>(bbox, block_level, sample_datatype());
  block_storage_->put_block();
  valid_blocks_.insert(block_idx);
}

template<psm_aux_type AUX_T>
void psm_aux_scene<AUX_T>::discover_blocks()
{
  vcl_string storage_directory = block_storage_->storage_directory();
  
  vcl_stringstream block_glob;
  vcl_string fname_prefix = "block_";
  block_glob << storage_directory << '/' << fname_prefix << "*.fd3";

    // traverse glob entries and parse filename
  for (vul_file_iterator file_it = block_glob.str().c_str(); file_it; ++file_it)
  {
    int x_idx, y_idx, z_idx;
    vcl_stringstream x_idx_str, y_idx_str, z_idx_str;

    // parse x index
    vcl_string match_str = file_it.filename();
    unsigned x_idx_start = match_str.find("_",0) + 1;
    unsigned x_idx_end = match_str.find("_",x_idx_start);
    x_idx_str << match_str.substr(x_idx_start,x_idx_end - x_idx_start);
    x_idx_str >> x_idx;
    // parse y index
    unsigned y_idx_start = match_str.find("_",x_idx_end) + 1;
    unsigned y_idx_end = match_str.find("_",y_idx_start);
    y_idx_str << match_str.substr(y_idx_start,y_idx_end - y_idx_start);
    y_idx_str >> y_idx;
    // parse z index
    unsigned z_idx_start = match_str.find("_",y_idx_end) + 1;
    unsigned z_idx_end = match_str.find("_",z_idx_start);
    z_idx_str << match_str.substr(z_idx_start,z_idx_end - z_idx_start);
    z_idx_str >> z_idx;

    vcl_cout << "found block " << x_idx << ", " << y_idx << ", " << z_idx << vcl_endl;
    set_block_valid(vgl_point_3d<int>(x_idx,y_idx,z_idx),true);

  }
}


#define PSM_AUX_SCENE_INSTANTIATE(T) \
template class psm_aux_scene<T>


#endif
