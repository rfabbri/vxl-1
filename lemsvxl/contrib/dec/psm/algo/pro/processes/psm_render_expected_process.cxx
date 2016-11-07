//This is lemsvxl/contrib/dec/psm/pro/processes/psm_render_expected_process.cxx
//:
// \file
// \brief A process for rendering expected images of a scene
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

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>

//#include <psm/algo/psm_render_expected_aa.h>
#include <psm/algo/psm_render_expected.h>

namespace psm_render_expected_process_globals
{
  const unsigned int n_inputs_ = 4;
  const unsigned int n_outputs_ = 2;
  //Define parameters here
  const vcl_string param_use_black_background_ =  "use_black_background";
}


//: set input and output types
bool psm_render_expected_process_cons(bprb_func_process& pro)
{
  using namespace psm_render_expected_process_globals;

  // process takes 4 inputs:
  //input[0]: The camera to render the expected image from
  //input[1]: number of pixels (x)
  //input[2]: number of pixels (y)
  //input[3]: The scene

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "unsigned";
  input_types_[2] = "unsigned";
  input_types_[3] = "psm_scene_base_sptr";

   // process has 2 outputs:
  // output[0]: The rendered frame
  // output[1]: A confidence map, giving probability that each pixel was seen from the original view.

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
bool psm_render_expected_process(bprb_func_process& pro)
{
  using namespace psm_render_expected_process_globals;

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

  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);

  unsigned npixels_x = pro.get_input<unsigned>(1);
  unsigned npixels_y = pro.get_input<unsigned>(2);

  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(3);
  
  psm_apm_type apm_type = scene_base->appearance_model_type();

  vil_image_view_base_sptr expected_img_base;
  vil_image_view<float> *mask_img = new vil_image_view<float>(npixels_x,npixels_y,1);

  //vpgl_perspective_camera<double> camera.ptr() = dynamic_cast<vpgl_perspective_camera<double>*>(camera.ptr());
  //if (!pcam) {
   // vcl_cerr << "error: only perspective cameras allowed for now." << vcl_endl;
   // return false;
 // }
  
  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        vil_image_view<psm_apm_traits<PSM_APM_MOG_GREY>::obs_mathtype>* expected_img = new vil_image_view<psm_apm_traits<PSM_APM_MOG_GREY>::obs_mathtype>(npixels_x,npixels_y,psm_apm_traits<PSM_APM_MOG_GREY>::obs_dim);

        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_render_expected(*scene, camera.ptr(), *expected_img, *mask_img, use_black_background);

        // convert output to an 8-bit image
        vil_image_view<unsigned char> *expected_byte = new vil_image_view<unsigned char>(expected_img->ni(),expected_img->nj(),expected_img->nplanes());
        vil_convert_stretch_range_limited(*expected_img,*expected_byte, 0.0f, 1.0f);
        expected_img_base = expected_byte;

        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype>* expected_img = new vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype>(npixels_x,npixels_y,psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_dim);

        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_render_expected(*scene, camera.ptr(), *expected_img, *mask_img, use_black_background);

        // convert output to an 8-bit image
        vil_image_view<unsigned char> *expected_byte = new vil_image_view<unsigned char>(expected_img->ni(),expected_img->nj(),expected_img->nplanes());
        vil_convert_stretch_range_limited(*expected_img,*expected_byte, 0.0f, 1.0f);
        expected_img_base = expected_byte;
        break;
      }
    case PSM_APM_MOG_RGB:
      {
        vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype>* expected_img = new vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype>(npixels_x,npixels_y,1);
         
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
 
         psm_render_expected(*scene, camera.ptr(), *expected_img, *mask_img, use_black_background);

        // convert output to an 8-bit image
         vil_image_view<unsigned char> *expected_byte = new vil_image_view<unsigned char>(expected_img->ni(),expected_img->nj(),psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_dim);
        vil_convert_stretch_range_limited(vil_view_as_planes(*expected_img),*expected_byte, 0.0f, 1.0f);
        expected_img_base = expected_byte;
        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype>* expected_img = new vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype>(npixels_x,npixels_y,1);
         
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_render_expected(*scene, camera.ptr(), *expected_img, *mask_img, use_black_background);

        // convert output to an 8-bit image
        vil_image_view<unsigned char> *expected_byte = new vil_image_view<unsigned char>(expected_img->ni(),expected_img->nj(),psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_dim);
        vil_convert_stretch_range_limited(vil_view_as_planes(*expected_img),*expected_byte, 0.0f, 1.0f);
        expected_img_base = expected_byte;
        break;
      }

    default:
      vcl_cerr << "error - psm_render_expected_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

  //store output
  pro.set_output_val<vil_image_view_base_sptr>(0, expected_img_base);
  vil_image_view_base_sptr mask_sptr = mask_img;
  pro.set_output_val<vil_image_view_base_sptr>(1, mask_sptr);

  return true;
}
