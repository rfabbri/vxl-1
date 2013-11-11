//This is lemsvxl/contrib/dec/psm/pro/processes/psm_convert_vis_implicit_process.cxx
//:
// \file
// \brief A process for converting a vis_implicit auxillary scene into a volume file in VTK format.
// \author Daniel Crispell
// \date 02/05/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim


#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_3d.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>
#include <psm/psm_aux_scene_base.h>
#include <psm/psm_aux_scene.h>


namespace psm_convert_vis_implicit_process_globals
{
  const unsigned int n_inputs_ = 4;
}


//: set input and output types
bool psm_convert_vis_implicit_process_cons(bprb_func_process& pro)
{
  using namespace psm_convert_vis_implicit_process_globals;

  //input[0]: The scene
  //input[1]: The directory to write to
  //input[2]: The resolution level of the output
  //input[3]: The scale factor for the output data

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "psm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "float";

  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_convert_vis_implicit_process(bprb_func_process& pro)
{
  using namespace psm_convert_vis_implicit_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the inputs
  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(0); 

  psm_apm_type apm_type = scene_base->appearance_model_type();

  vcl_string filename = pro.get_input<vcl_string>(1);

  unsigned resolution_level = pro.get_input<unsigned>(2);

  float scale_factor = pro.get_input<float>(3);

  psm_aux_scene_base_sptr aux_scene_base;
  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        aux_scene_base = scene->get_aux_scene<PSM_AUX_VIS_IMPLICIT>();

        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        aux_scene_base = scene->get_aux_scene<PSM_AUX_VIS_IMPLICIT>();

        break;
      }
    case PSM_APM_MOG_RGB:
      {
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        aux_scene_base = scene->get_aux_scene<PSM_AUX_VIS_IMPLICIT>();

        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        aux_scene_base = scene->get_aux_scene<PSM_AUX_VIS_IMPLICIT>();

        break;
      }
    default:
      vcl_cerr << "error - psm_convert_vis_implicit_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }
  psm_aux_scene<PSM_AUX_VIS_IMPLICIT> *aux_scene = dynamic_cast<psm_aux_scene<PSM_AUX_VIS_IMPLICIT>*>(aux_scene_base.ptr());
  if (!aux_scene) {
    vcl_cerr << "error casting aux_scene to appropriate type. " << vcl_endl;
    return false;
  }
  vcl_cout << "converting scene.. resolution level = " << resolution_level << vcl_endl;

  hsds_fd_tree<psm_vis_implicit_sample,3> &aux_block = aux_scene->get_block(vgl_point_3d<int>(0,0,0));
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

  hsds_fd_tree<psm_vis_implicit_sample,3>::const_iterator block_it = aux_block.begin();
  for (; block_it != aux_block.end(); ++block_it) {
    float cell_val = 1.0f;
    unsigned int node_x = block_it->first.get_node_coordinate(0);
    unsigned int node_y = block_it->first.get_node_coordinate(1);
    unsigned int node_z = block_it->first.get_node_coordinate(2);
    if (block_it->second.n_obs > 1) {
      cell_val = block_it->second.max_vis;
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
