//This is lemsvxl/contrib/dec/psm/pro/processes/psm_compute_expected_depth_process.cxx
//:
// \file
// \brief A process for rendering expected images of a scene
// \author Daniel Crispell
// \date 07/07/2009
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

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>

//#include <psm/algo/psm_compute_expected_depth_aa.h>
#include <psm/algo/psm_compute_expected_depth.h>

namespace psm_compute_expected_depth_process_globals
{
  const unsigned int n_inputs_ = 4;
  const unsigned int n_outputs_ = 2;
  //Define parameters here
 
}


//: set input and output types
bool psm_compute_expected_depth_process_cons(bprb_func_process& pro)
{
  using namespace psm_compute_expected_depth_process_globals;

  // process takes 4 inputs:
  //input[0]: The camera to compute the expected depth from
  //input[1]: number of pixels (x)
  //input[2]: number of pixels (y)
  //input[3]: The scene

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "unsigned";
  input_types_[2] = "unsigned";
  input_types_[3] = "psm_scene_base_sptr";

   // process has 2 outputs:
  // output[0]: The expected depth
  // output[1]: The probability of no occlusion

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0]= "vil_image_view_base_sptr";
  output_types_[1]= "vil_image_view_base_sptr";  

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_compute_expected_depth_process(bprb_func_process& pro)
{
  using namespace psm_compute_expected_depth_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }


  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);

  unsigned npixels_x = pro.get_input<unsigned>(1);
  unsigned npixels_y = pro.get_input<unsigned>(2);

  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(3);
  
  psm_apm_type apm_type = scene_base->appearance_model_type();

  vil_image_view<float> *expected_depth = new vil_image_view<float>(npixels_x,npixels_y);

  vil_image_view<float> *vis_inf = new vil_image_view<float>(npixels_x, npixels_y);

  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_compute_expected_depth(*scene, camera.ptr(), *expected_depth, *vis_inf);
        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
       psm_compute_expected_depth(*scene, camera.ptr(), *expected_depth, *vis_inf);
       break;
      }
    case PSM_APM_MOG_RGB:
      {   
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
         psm_compute_expected_depth(*scene, camera.ptr(), *expected_depth, *vis_inf);
         break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
           
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_compute_expected_depth(*scene, camera.ptr(), *expected_depth, *vis_inf);
        break;
      }

    default:
      vcl_cerr << "error - psm_compute_expected_depth_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

  //store output
  vil_image_view_base_sptr depth_sptr = expected_depth;
  pro.set_output_val<vil_image_view_base_sptr>(0, depth_sptr);
  vil_image_view_base_sptr vis_inf_sptr = vis_inf;
  pro.set_output_val<vil_image_view_base_sptr>(1, vis_inf_sptr);

  return true;
}
