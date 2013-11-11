#ifndef psm_scene_txx_
#define psm_scene_txx_

#include <vbl/vbl_bounding_box.h>
#include <vul/vul_file_iterator.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>

#include <hsds/hsds_fd_tree.h>

#include "psm_scene.h"

template <psm_apm_type APM >
void psm_scene<APM>::set_block(vgl_point_3d<int> block_idx, psm_sample<APM>  data)
{
  vcl_cerr << "not implemented yet";
}


template <psm_apm_type APM>
hsds_fd_tree<psm_sample<APM >,3>& psm_scene<APM>::get_block(vgl_point_3d<int> block_idx)
{
  return block_storage_->get_block(block_idx);
}


 //: get a cell
template <psm_apm_type APM>
psm_sample<APM>& psm_scene<APM>::get_cell(vgl_point_3d<int> block_idx, hsds_fd_tree_node_index<3> cell_idx)
{
  return get_block(block_idx)[cell_idx];
}


template <psm_apm_type APM>
void psm_scene<APM>::set_block(vgl_point_3d<int> block_idx, hsds_fd_tree<psm_sample<APM> ,3> &data)
{
  hsds_fd_tree<psm_sample<APM> ,3> &block = block_storage_->get_block(block_idx);
  block = data;
  block_storage_->put_block();

}

template <psm_apm_type APM>
void psm_scene<APM>::init_block(vgl_point_3d<int> block_idx, unsigned int block_level, float init_alpha)
{

  hsds_fd_tree<psm_sample<APM> ,3> &block = block_storage_->get_block(block_idx);
  // initialize bounding box
  vbl_bounding_box<double,3> bbox;
  bbox.update(origin_.x() + block_idx.x() * block_len_,
    origin_.y() + block_idx.y() * block_len_,
    origin_.z() + block_idx.z() * block_len_);
  bbox.update(origin_.x() + (block_idx.x()+1) * block_len_,
    origin_.y() + (block_idx.y()+1) * block_len_,
    origin_.z() + (block_idx.z()+1) * block_len_);
  block = hsds_fd_tree<psm_sample<APM> ,3>(bbox, block_level, psm_sample<APM>(init_alpha), max_subdivision_levels_);
  block_storage_->put_block();
  valid_blocks_.insert(block_idx);
}

template<psm_apm_type APM>
void psm_scene<APM>::discover_blocks()
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

//: return the number of cells in the scene
template<psm_apm_type APM>
unsigned int psm_scene<APM>::num_cells()
{
  unsigned int total_size = 0;
  vcl_set<vgl_point_3d<int>, vgl_point_3d_cmp<int> >::iterator block_it = valid_blocks_.begin();
  for (; block_it != valid_blocks_.end(); ++block_it) {
    total_size += num_cells(*block_it);
  }
  return total_size;
}

//: return the number of cells in the given block
template<psm_apm_type APM>
unsigned int psm_scene<APM>::num_cells(vgl_point_3d<int> block_idx)
{
  hsds_fd_tree<psm_sample<APM>,3> &block =  get_block(block_idx);
  return block.size();
}

//: save the occlusion density in a format readable by Drishti volume renderer
template<psm_apm_type APM>
void psm_scene<APM>::save_alpha_raw(vcl_string filename, vgl_point_3d<int> block_idx, unsigned int resolution_level)
{
  hsds_fd_tree<psm_sample<APM>,3> &block = get_block(block_idx);
  vbl_bounding_box<double,3> block_bb = block.bounding_box();
  // get origin of block
  vnl_vector_fixed<double,3> block_og(block_bb.xmin(), block_bb.ymin(), block_bb.zmin());

  const unsigned int ncells = 1 << resolution_level;

  // assume that bounding box is a cube
  const double step_len = ((block_bb.xmax() - block_bb.xmin())/double(ncells));

  // origin should specify center of first cell
  vnl_vector_fixed<double,3> data_og = block_og + (step_len/2.0);

  vcl_ofstream ofs(filename.c_str(),vcl_ios::binary);
  if (!ofs.good()) {
    vcl_cerr << "error opening " << filename << " for write! " << vcl_endl;
    return;
  }

  float *data = new float[ncells*ncells*ncells];
  // init to zero
  for (float* dp = data; dp < data + ncells*ncells*ncells; ++dp) {
    *dp = 0.0f;
  }

  double out_cell_norm_volume = (block.max_level() - resolution_level + 1);
  out_cell_norm_volume = out_cell_norm_volume*out_cell_norm_volume*out_cell_norm_volume;

  typename hsds_fd_tree<psm_sample<APM>,3>::const_iterator block_it = block.begin();
  for (; block_it != block.end(); ++block_it) {

    unsigned int node_x = block_it->first.get_node_coordinate(0);
    unsigned int node_y = block_it->first.get_node_coordinate(1);
    unsigned int node_z = block_it->first.get_node_coordinate(2);

    float cell_val = block_it->second.alpha;

    if (block_it->first.lvl == resolution_level) {
      // just copy value to output array
      unsigned int out_index = node_z*ncells*ncells + node_y*ncells + node_x;
      data[out_index] = cell_val;
    }
    else if (block_it->first.lvl < resolution_level) {
      // cell is bigger than output cells.  copy value to all contained output cells.
      const unsigned int us_factor = 1 << (resolution_level - block_it->first.lvl);
      const unsigned int node_x_start = node_x*us_factor;
      const unsigned int node_y_start = node_y*us_factor;
      const unsigned int node_z_start = node_z*us_factor;
      for (unsigned int z=node_z_start; z<node_z_start+us_factor; ++z) {
        for (unsigned int y=node_y_start; y<node_y_start+us_factor; ++y) {
          for (unsigned int x=node_x_start; x<node_x_start+us_factor; ++x) {
            unsigned int out_index = z*ncells*ncells + y*ncells + x;
            data[out_index] = cell_val;
          }
        }
      }
    }
    else {
      // cell is smaller than output cells. increment output cell value
      unsigned int ds_factor = 1 << (block_it->first.lvl - resolution_level);
      double update_weight = 1.0 / double(ds_factor*ds_factor*ds_factor);
      unsigned int out_index = (node_z/ds_factor)*ncells*ncells + (node_y/ds_factor)*ncells + (node_x/ds_factor);
      data[out_index] += float(cell_val*update_weight);
    }
  }
  // convert float values to char
  unsigned char *byte_data = new unsigned char[ncells*ncells*ncells];
  float* dp = data;
  for (unsigned char* bdp = byte_data; dp < data + ncells*ncells*ncells; ++dp, ++bdp) {
    double P = 1.0 - vcl_exp(-*dp*step_len);
    *bdp = (unsigned char)(vcl_floor((255.0 * P) + 0.5)); // always positive so this is an ok way to round
  }
  delete[] data;

  // write header
  unsigned char data_type = 0; // 0 means unsigned byte

  vxl_uint_32 nx = ncells;
  vxl_uint_32 ny = ncells;
  vxl_uint_32 nz = ncells;

  ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  ofs.write((char*)byte_data,ncells*ncells*ncells);
  delete[] byte_data;

  ofs.close();

  return;
}


#define PSM_SCENE_INSTANTIATE(T) \
  template class psm_scene<T>

#endif

