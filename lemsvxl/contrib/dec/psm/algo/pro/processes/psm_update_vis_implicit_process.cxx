//This is lemsvxl/contrib/dec/psm/pro/processes/psm_pixel_probability_range_process.cxx
//:
// \file
// \brief A process for updating the vis_implicit auxillary scene based on a camera viewpoint
// \author Daniel Crispell
// \date 02/05/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>
#include <psm/psm_aux_scene_base.h>
#include <psm/psm_aux_scene.h>

#include <psm/algo/psm_update_vis_implicit_aa.h>


namespace psm_update_vis_implicit_process_globals
{
  const unsigned int n_inputs_ = 3;
}


//: set input and output types
bool psm_update_vis_implicit_process_cons(bprb_func_process& pro)
{
  using namespace psm_update_vis_implicit_process_globals;

  // process takes 3 inputs:
  //input[0]: The image
  //input[1]: The camera
  //input[3]: The scene

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "psm_scene_base_sptr";

  // process has 0 outputs.

  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_update_vis_implicit_process(bprb_func_process& pro)
{
  using namespace psm_update_vis_implicit_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the inputs
  vil_image_view_base_sptr image_base = pro.get_input<vil_image_view_base_sptr>(0);

  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(1);

  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(2);
  psm_apm_type apm_type = scene_base->appearance_model_type();

  //vpgl_perspective_camera<double> *pcam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.ptr());
  //if (!pcam) {
  //  vcl_cerr << "error: only perspective cameras allowed for now." << vcl_endl;
  //  return false;
  //}

  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_aux_scene_base_sptr aux_scene_base = scene->get_aux_scene<PSM_AUX_VIS_IMPLICIT>();

        psm_update_vis_implicit(*scene, aux_scene_base, camera.ptr(), image_base->ni(), image_base->nj());

        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_aux_scene_base_sptr aux_scene_base = scene->get_aux_scene<PSM_AUX_VIS_IMPLICIT>();

        psm_update_vis_implicit(*scene, aux_scene_base, camera.ptr(), image_base->ni(), image_base->nj());

        break;
      }
    case PSM_APM_MOG_RGB:
      {
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_aux_scene_base_sptr aux_scene_base = scene->get_aux_scene<PSM_AUX_VIS_IMPLICIT>();

        psm_update_vis_implicit(*scene, aux_scene_base, camera.ptr(), image_base->ni(), image_base->nj());

        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_aux_scene_base_sptr aux_scene_base = scene->get_aux_scene<PSM_AUX_VIS_IMPLICIT>();

        psm_update_vis_implicit(*scene, aux_scene_base, camera.ptr(), image_base->ni(), image_base->nj());

        break;
      }
    default:
      vcl_cerr << "error - psm_update_vis_implicit_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }


  return true;
}
