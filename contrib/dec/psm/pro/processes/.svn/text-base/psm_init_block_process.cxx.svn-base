//This is lemsvxl/contrib/dec/psm/pro/processes/psm_init_block_process.cxx
//:
// \file
// \brief A process for initializing a scene block and declaring it valid.
// \author Daniel Crispell
// \date 02/05/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim


#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <bgeo/bgeo_lvcs.h>
#include <bgeo/bgeo_lvcs_sptr.h>

#include <vul/vul_file.h>


namespace psm_init_block_process_globals
{
  const unsigned int n_inputs_ = 6;
}


//: set input and output types
bool psm_init_block_process_cons(bprb_func_process& pro)
{
  using namespace psm_init_block_process_globals;

  // input[0]: The scene
  // input[1]: The block x index
  // input[2]: The block y index
  // input[3]: The block z index
  // input[4]: The subdivision level to initialize at.
  // input[5]: The initial max occlusion probability for each cell
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "psm_scene_base_sptr";
  input_types_[1] = "int";
  input_types_[2] = "int";
  input_types_[3] = "int";
  input_types_[4] = "unsigned";
  input_types_[5] = "float";
  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}


//: Initializes a scene block to the given subdivision level
bool psm_init_block_process(bprb_func_process &pro)
{
  using namespace psm_init_block_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the inputs
  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(0); 
  int x_index = pro.get_input<int>(1);
  int y_index = pro.get_input<int>(2);
  int z_index = pro.get_input<int>(3);
  unsigned int sub_level = pro.get_input<unsigned>(4);
  float init_occlusion_prob = pro.get_input<float>(5);

  // convert occlusion probability into occlusion density
  double cell_len = scene_base->block_len() / (1 << sub_level);
  float init_alpha = (float)(-vcl_log(1.0 - init_occlusion_prob) / cell_len);

  vcl_cout << "cell_len = " << cell_len << "  init_alpha = " << init_alpha << vcl_endl;


  psm_apm_type apm_type = scene_base->appearance_model_type();

  vcl_cout << "apm_type = " << apm_type << vcl_endl;
  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        scene->init_block(vgl_point_3d<int>(x_index, y_index, z_index), sub_level, init_alpha);
        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        scene->init_block(vgl_point_3d<int>(x_index, y_index, z_index), sub_level, init_alpha);
        break;
      }
    case PSM_APM_MOG_RGB:
      {
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        scene->init_block(vgl_point_3d<int>(x_index, y_index, z_index), sub_level, init_alpha);
        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        scene->init_block(vgl_point_3d<int>(x_index, y_index, z_index), sub_level, init_alpha);
        break;
      }
    default:
      vcl_cerr << "error - psm_render_expected_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

  return true;
}
