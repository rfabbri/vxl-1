//This is lemsvxl/contrib/dec/psm/pro/processes/psm_update_process.cxx
//:
// \file
// \brief A process for incrementally updating a psm_scene with an image.
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
#include <vil/vil_view_as.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>

#include <psm/algo/psm_update_2pass.h>


namespace psm_update_process_globals
{
  const unsigned int n_inputs_ = 3;
  const unsigned int n_outputs_ = 1;
  //Define parameters here
  const vcl_string param_use_black_background_ =  "use_black_background";
}


//: set input and output types
bool psm_update_process_cons(bprb_func_process& pro)
{
  using namespace psm_update_process_globals;

  // process takes 3 inputs:
  //input[0]: The image
  //input[1]: The camera
  //input[3]: The scene

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "psm_scene_base_sptr";

  // process has 1 output:
  //output[0]: the image containing the pixel probability density values.
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0]= "vil_image_view_base_sptr";


  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_update_process(bprb_func_process& pro)
{
  using namespace psm_update_process_globals;

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

  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(2);
  psm_apm_type apm_type = scene_base->appearance_model_type();

  vil_image_view<float> *prob_img = new vil_image_view<float>(image_base->ni(),image_base->nj(),1);
  vil_image_view_base_sptr prob_img_base = prob_img;

  //vpgl_perspective_camera<double> *pcam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.ptr());
  //if (!pcam) {
  //  vcl_cerr << "error: only perspective cameras allowed for now." << vcl_endl;
  //  return false;
  //}

   vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(image_base.ptr());
   if (!img_byte) {
     vcl_cerr << "error: expected input image to have pixel type vxl_byte" << vcl_endl;
     return false;
   }

  
  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        vil_image_view<psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype> img(image_base->ni(), image_base->nj(), 1);
        vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
 
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_update(*scene, camera.ptr(), img, use_black_background);

        break;
      }
    case PSM_APM_MOG_RGB:
      {
        vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> img(image_base->ni(), image_base->nj(), 1);
        vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_mathtype> img_planes = vil_view_as_planes(img);

        vil_convert_stretch_range_limited(*img_byte ,img_planes, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);

        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_update(*scene, camera.ptr(), img, use_black_background);

        break;
      }
    default:
      vcl_cerr << "error - psm_update_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

  //store output
  pro.set_output_val<vil_image_view_base_sptr>(0, prob_img_base);


  return true;
}
