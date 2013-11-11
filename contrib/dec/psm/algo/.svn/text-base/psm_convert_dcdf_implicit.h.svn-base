#ifndef psm_convert_dcdf_implicit_h_
#define psm_convert_dcdf_implicit_h_

#include <vcl_string.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_scene.h>
#include <psm/psm_aux_scene.h>


template<psm_aux_type AUX_T>
bool psm_convert_dcdf_implicit(psm_aux_scene<AUX_T> *aux_scene, unsigned int resolution_level, float scale_factor, vcl_string filename)
{
  vcl_cout << "converting scene.. resolution level = " << resolution_level << vcl_endl;

  hsds_fd_tree<typename psm_aux_traits<AUX_T>::sample_datatype ,3> &aux_block = aux_scene->get_block(vgl_point_3d<int>(0,0,0));
  vbl_bounding_box<double,3> block_bb = aux_block.bounding_box();
  // get origin of block
  vnl_vector_fixed<double,3> block_og(block_bb.xmin(), block_bb.ymin(), block_bb.zmin());
  // multiply origin and by the scale factor
  block_og *= scale_factor;

  const unsigned int ncells = 1 << resolution_level;

  // assume that bounding box is a cube
  const double step_len = ((block_bb.xmax() - block_bb.xmin())/double(ncells))*scale_factor;

  // origin should specify center of first cell
  vnl_vector_fixed<double,3> data_og = block_og + (step_len/2.0);

  vcl_ofstream ofs(filename.c_str(),vcl_ios::binary);
  if (!ofs.good()) {
    vcl_cerr << "error opening " << filename << " for write! " << vcl_endl;
    return false;
  }

  // windows lf cf combo doesnt work - just write ascii 10 = newline
  char newline = 10;

  ofs << "# vtk DataFile Version 2.0";
  ofs.write(&newline,1);
  ofs << "Visibility Volume";
  ofs.write(&newline,1);
  ofs << "BINARY";
  ofs.write(&newline,1);
  ofs << "DATASET STRUCTURED_POINTS";
  ofs.write(&newline,1);
  ofs << "DIMENSIONS " << ncells << " " << ncells << " " << ncells;
  ofs.write(&newline,1);
  ofs << "ORIGIN " << data_og;
  ofs.write(&newline,1);
  ofs << "SPACING " << step_len << " " << step_len << " " << step_len;
  ofs.write(&newline,1);
  ofs << "POINT_DATA " << ncells*ncells*ncells;
  ofs.write(&newline,1);
  ofs << "SCALARS volume_scalars unsigned_char 1";
  ofs.write(&newline,1);
  ofs << "LOOKUP_TABLE default";
  ofs.write(&newline,1);

  float *data = new float[ncells*ncells*ncells];
  // init to zero
  for (float* dp = data; dp < data + ncells*ncells*ncells; ++dp) {
    *dp = 0.0f;
  }

  double out_cell_norm_volume = (aux_block.max_level() - resolution_level + 1);
  out_cell_norm_volume = out_cell_norm_volume*out_cell_norm_volume*out_cell_norm_volume;

  hsds_fd_tree<typename psm_aux_traits<AUX_T>::sample_datatype ,3>::const_iterator block_it = aux_block.begin();
  for (; block_it != aux_block.end(); ++block_it) {
    float cell_val = 1.0f;
    unsigned int node_x = block_it->first.get_node_coordinate(0);
    unsigned int node_y = block_it->first.get_node_coordinate(1);
    unsigned int node_z = block_it->first.get_node_coordinate(2);
    if (block_it->second.seg_len_sum_temp_ > 0.0f) {
      cell_val = block_it->second.min_dcdf_;
    }
    else {
      cell_val = 0.0f;
    }
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
    *bdp = (unsigned char)(vcl_floor((255.0 * *dp) + 0.5)); // always positive so this is an ok way to round
  }
  delete[] data;

  ofs.write((char*)byte_data,ncells*ncells*ncells);

  delete[] byte_data;

  return true;
}



#endif

