//This is lemsvxl/contrib/dec/psm/pro/processes/psm_pixel_probability_range_process.cxx
//:
// \file
// \brief A process for computing the probability of the pixels in a particular image +/- the given intensity range
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

#include <psm/algo/psm_pixel_probability_range.h>

namespace psm_pixel_probability_range_process_globals
{
  const unsigned int n_inputs_ = 4;
  const unsigned int n_outputs_ = 2;
  //Define parameters here
  const vcl_string param_use_black_background_ =  "use_black_background";
}


//: set input and output types
bool psm_pixel_probability_range_process_cons(bprb_func_process& pro)
{
  using namespace psm_pixel_probability_range_process_globals;

  // process takes 4 inputs:
  //input[0]: The image
  //input[1]: The camera
  //input[2]: The probability range
  //input[3]: The scene

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "float";
  input_types_[3] = "psm_scene_base_sptr";

  // process has 1 output:
  //output[0]: the image containing the pixel probabilities.
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0]= "vil_image_view_base_sptr";

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_pixel_probability_range_process(bprb_func_process& pro)
{
  using namespace psm_pixel_probability_range_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }


  // get the parameters
  bool use_black_background;
  pro.parameters()->get_value(param_use_black_background_, use_black_background);

  // get the inputs
  vil_image_view_base_sptr image_base = pro.get_input<vil_image_view_base_sptr>(0);

  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(1);

  float obs_range = pro.get_input<float>(2);

  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(3);
  psm_apm_type apm_type = scene_base->appearance_model_type();

  vil_image_view<float> *prob_img = new vil_image_view<float>(image_base->ni(),image_base->nj(),1);
  vil_image_view_base_sptr prob_img_base = prob_img;

  //vpgl_perspective_camera<double> *pcam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.ptr());
  //if (!pcam) {
  //  vcl_cerr << "error: only perspective cameras allowed for now." << vcl_endl;
   // return false;
  //}

  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        vil_image_view_base_sptr img_base_conv = vil_convert_stretch_range(psm_apm_traits<PSM_APM_MOG_GREY>::obs_mathtype(),image_base);

        vil_image_view<psm_apm_traits<PSM_APM_MOG_GREY>::obs_mathtype> *img = dynamic_cast<vil_image_view<psm_apm_traits<PSM_APM_MOG_GREY>::obs_mathtype>*>(img_base_conv.ptr());
        if (!img) {
          vcl_cerr << "error casting image to appropriate type " << vcl_endl;
          return false;
        }
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_pixel_probability_range(*scene, camera.ptr(), *img, *prob_img, obs_range, use_black_background);

        break;
      }
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
        psm_pixel_probability_range(*scene, camera.ptr(), *img, *prob_img, obs_range, use_black_background);

        break;
      }
    case PSM_APM_MOG_RGB:
      {
        // convert to floating point
        vil_image_view_base_sptr img_base_conv = vil_convert_to_component_order(vil_convert_stretch_range(psm_apm_traits<PSM_APM_MOG_RGB>::obs_mathtype(),image_base));

        vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_mathtype> *img = dynamic_cast<vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_mathtype>*>(img_base_conv.ptr());
        if (!img) {
          vcl_cerr << "error casting image to appropriate type " << vcl_endl;
          return false;
        }
        vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> img_rgb = vil_view_as_rgb(*img);

        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_pixel_probability_range(*scene, camera.ptr(), *img, *prob_img, obs_range, use_black_background);

        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
        // convert to floating point
        vil_image_view_base_sptr img_base_conv = vil_convert_to_component_order(vil_convert_stretch_range(psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_mathtype(),image_base));

        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_mathtype> *img = dynamic_cast<vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_mathtype>*>(img_base_conv.ptr());
        if (!img) {
          vcl_cerr << "error casting image to appropriate type " << vcl_endl;
          return false;
        }
        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype> img_rgb = vil_view_as_rgb(*img);

        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_pixel_probability_range(*scene, camera.ptr(), *img, *prob_img, obs_range, use_black_background);

        break;
      }
    default:
      vcl_cerr << "error - psm_pixel_probability_range_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

  //store output
  pro.set_output_val<vil_image_view_base_sptr>(0, prob_img_base);

  return true;
}
