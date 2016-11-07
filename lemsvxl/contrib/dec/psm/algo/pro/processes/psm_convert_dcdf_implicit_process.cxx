//This is lemsvxl/contrib/dec/psm/pro/processes/psm_convert_dcdf_implicit_process.cxx
//:
// \file
// \brief A process for converting a dcdf_implicit auxillary scene into a volume file in VTK format.
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

#include <psm/algo/psm_convert_dcdf_implicit.h>

namespace psm_convert_dcdf_implicit_process_globals
{
  const unsigned int n_inputs_ = 4;
}


//: set input and output types
bool psm_convert_dcdf_implicit_process_cons(bprb_func_process& pro)
{
  using namespace psm_convert_dcdf_implicit_process_globals;

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
bool psm_convert_dcdf_implicit_process(bprb_func_process& pro)
{
  using namespace psm_convert_dcdf_implicit_process_globals;

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
  bool status = false; 

  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        aux_scene_base = scene->get_aux_scene<PSM_AUX_DCDF_IMPLICIT_GREY>();
        psm_aux_scene<PSM_AUX_DCDF_IMPLICIT_GREY> *aux_scene = dynamic_cast<psm_aux_scene<PSM_AUX_DCDF_IMPLICIT_GREY>*>(aux_scene_base.ptr());
        if (!aux_scene) {
          vcl_cerr << "error casting aux_scene to appropriate type. " << vcl_endl;
          return false;
        }
        status = psm_convert_dcdf_implicit(aux_scene, resolution_level, scale_factor, filename);

        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        aux_scene_base = scene->get_aux_scene<PSM_AUX_DCDF_IMPLICIT_GREY>();
        psm_aux_scene<PSM_AUX_DCDF_IMPLICIT_GREY> *aux_scene = dynamic_cast<psm_aux_scene<PSM_AUX_DCDF_IMPLICIT_GREY>*>(aux_scene_base.ptr());
        if (!aux_scene) {
          vcl_cerr << "error casting aux_scene to appropriate type. " << vcl_endl;
          return false;
        }
        status = psm_convert_dcdf_implicit(aux_scene, resolution_level, scale_factor, filename);

        break;
      }
    case PSM_APM_MOG_RGB:
      {
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        aux_scene_base = scene->get_aux_scene<PSM_AUX_DCDF_IMPLICIT_RGB>();
        psm_aux_scene<PSM_AUX_DCDF_IMPLICIT_RGB> *aux_scene = dynamic_cast<psm_aux_scene<PSM_AUX_DCDF_IMPLICIT_RGB>*>(aux_scene_base.ptr());
        if (!aux_scene) {
          vcl_cerr << "error casting aux_scene to appropriate type. " << vcl_endl;
          return false;
        }
        status = psm_convert_dcdf_implicit(aux_scene, resolution_level, scale_factor, filename);

        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        aux_scene_base = scene->get_aux_scene<PSM_AUX_DCDF_IMPLICIT_RGB>();
        psm_aux_scene<PSM_AUX_DCDF_IMPLICIT_RGB> *aux_scene = dynamic_cast<psm_aux_scene<PSM_AUX_DCDF_IMPLICIT_RGB>*>(aux_scene_base.ptr());
        if (!aux_scene) {
          vcl_cerr << "error casting aux_scene to appropriate type. " << vcl_endl;
          return false;
        }
        status = psm_convert_dcdf_implicit(aux_scene, resolution_level, scale_factor, filename);
        break;
      }
    default:
      vcl_cerr << "error - psm_convert_dcdf_implicit_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

return status;
}

