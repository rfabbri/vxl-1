//This is lemsvxl/contrib/dec/psm/pro/processes/psm_save_alpha_raw_process.cxx
//:
// \file
// \brief A process for saving a block's occupancy probability in a Drishi-readable raw format.
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


namespace psm_save_alpha_raw_process_globals
{
  const unsigned int n_inputs_ = 6;
}


//: set input and output types
bool psm_save_alpha_raw_process_cons(bprb_func_process& pro)
{
  using namespace psm_save_alpha_raw_process_globals;

  //input[0]: The scene
  //input[1]: The filename to write to
  //input[2]: The resolution level of the output

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "psm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "int";
  input_types_[4] = "int";
  input_types_[5] = "int";

  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_save_alpha_raw_process(bprb_func_process& pro)
{

  using namespace psm_save_alpha_raw_process_globals;

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
  unsigned int resolution_level = pro.get_input<unsigned>(2);

  int block_x = pro.get_input<int>(3);
  int block_y = pro.get_input<int>(4);
  int block_z = pro.get_input<int>(5);

  vcl_cout << "converting scene.. resolution level = " << resolution_level << vcl_endl;

  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        scene->save_alpha_raw(filename, vgl_point_3d<int>(block_x,block_y,block_z), resolution_level);

        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }   
        scene->save_alpha_raw(filename, vgl_point_3d<int>(block_x,block_y,block_z), resolution_level);

        break;
      }
    case PSM_APM_MOG_RGB:
      {
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        scene->save_alpha_raw(filename, vgl_point_3d<int>(block_x,block_y,block_z), resolution_level);

        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }   
        scene->save_alpha_raw(filename, vgl_point_3d<int>(block_x,block_y,block_z), resolution_level);

        break;
      }
    default:
      vcl_cerr << "error - psm_save_alpha_raw_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }


  return true;
}
