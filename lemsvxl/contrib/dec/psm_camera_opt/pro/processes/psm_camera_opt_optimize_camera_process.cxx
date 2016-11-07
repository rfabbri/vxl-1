//This is lemsvxl/contrib/dec/psm/pro/processes/psm_camera_opt_optimize_camera_process.cxx
//:
// \file
// \brief A process for optimizing the camera parameters based on generated expected images
// \author Daniel Crispell
// \date 04/02/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim

#include <vcl_string.h>
#include <vcl_fstream.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>

#include <psm_camera_opt/psm_camera_opt_camera_optimizer.h>

namespace psm_camera_opt_optimize_camera_process_globals
{
  const unsigned int n_inputs_ = 3;
  const unsigned int n_outputs_ = 1;
  //Define parameters here
   const vcl_string param_use_black_background_ =  "use_black_background";
   const vcl_string param_rotation_variance_ = "rotation_variance";
   const vcl_string param_position_variance_ = "position_variance";
   const vcl_string param_homography_term_variance_ = "homography_term_variance";
   const vcl_string param_homography_translation_term_variance_ = "homography_translation_term_variance";

}


//: set input and output types
bool psm_camera_opt_optimize_camera_process_cons(bprb_func_process& pro)
{
  using namespace psm_camera_opt_optimize_camera_process_globals;

  // process takes 3 inputs:
  //input[0]: the image
  //input[1]: the camera (current estimate)
  //input[2]: the scene

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "psm_scene_base_sptr";

  // process has 1 output:
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_camera_opt_optimize_camera_process(bprb_func_process& pro)
{
  using namespace psm_camera_opt_optimize_camera_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the parameters 
  bool use_black_background = false;
  pro.parameters()->get_value(param_use_black_background_, use_black_background);
  if (use_black_background) {
    vcl_cout << "using black background." << vcl_endl;
  }

  double rotation_variance = 0.2; //default corresponds to roughly 1 degree std deviation
  pro.parameters()->get_value(param_rotation_variance_, rotation_variance);

  double position_variance = 3.0; // default corresponds to roughly 1.4 meter std deviation
  pro.parameters()->get_value(param_position_variance_, position_variance);

  double homography_term_variance = 0.0;
  pro.parameters()->get_value(param_homography_term_variance_, homography_term_variance);

  double homography_translation_term_variance = 0.0;
  pro.parameters()->get_value(param_homography_translation_term_variance_, homography_translation_term_variance);

  // get the inputs

  vil_image_view_base_sptr image_base = pro.get_input<vil_image_view_base_sptr>(0);

  vpgl_camera_double_sptr cam_base = pro.get_input<vpgl_camera_double_sptr>(1);
  vpgl_perspective_camera<double> *pcam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_base.ptr());
  if (!pcam) {
    vcl_cerr << "error casting to perspective camera" << vcl_endl;
    return false;
  }

  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(2);
  psm_apm_type apm_type = scene_base->appearance_model_type();


  switch (apm_type) {
    case PSM_APM_SIMPLE_GREY:
      {
        vil_image_view_base_sptr img_base_conv = vil_convert_stretch_range(psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype(),image_base);
        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype> *img = dynamic_cast<vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype>*>(img_base_conv.ptr());
        if (!img) {
          vcl_cerr << "error casting image to appropriate type " << vcl_endl;
          return false;
        }

        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }

        psm_camera_opt_camera_optimizer<PSM_APM_SIMPLE_GREY> cam_optimizer(position_variance, rotation_variance, homography_term_variance, homography_translation_term_variance, false);
        bool result = cam_optimizer.optimize(*scene, *img, *pcam);
        if (!result) {
          vcl_cerr << "error: psm_camera_opt_camera_optimizer.optimize() returned false" << vcl_endl;
          return false;
        }

        break;
      }
    
    default:
      vcl_cerr << "error - psm_camera_opt_optimize_camera_process: unsupported appearance model type " << apm_type << vcl_endl;
      return false;
  }

  //store output
  pro.set_output_val<vpgl_camera_double_sptr>(0,cam_base);

  return true;
}
