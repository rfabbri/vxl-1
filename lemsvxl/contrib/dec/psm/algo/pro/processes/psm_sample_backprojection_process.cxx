//This is lemsvxl/contrib/dec/psm/pro/processes/psm_sample_backprojection_process.cxx
//:
// \file
// \brief A process for generating a set of 3-d point samples based on an image position and variance
// \author Daniel Crispell
// \date 07/07/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim

#include <vcl_fstream.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <vgl/vgl_point_3d.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>

#include <psm/algo/psm_sample_backprojection.h>

namespace psm_sample_backprojection_process_globals
{
  const unsigned int n_inputs_ = 8;
  const unsigned int n_outputs_ = 0;
  //Define parameters here
 const vcl_string param_n_image_samples_ =  "n_image_samples";
 const vcl_string param_n_samples_per_ray_ = "n_samples_per_ray";
}


//: set input and output types
bool psm_sample_backprojection_process_cons(bprb_func_process& pro)
{
  using namespace psm_sample_backprojection_process_globals;

  // process takes 5 inputs:
  //input[0]: The image
  //input[1]: The camera 
  //input[2]: pixel index (x)
  //input[3]: pixel index (y)
  //input[4]: the pixel position std dev.
  //input[5]: use appearance information?
  //input[6]: The scene
  //input[7]: the filename to write the samples to

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "float";
  input_types_[3] = "float";
  input_types_[4] = "float";
  input_types_[5] = "bool";
  input_types_[6] = "psm_scene_base_sptr";
  input_types_[7] = "vcl_string";

   // process has 0 outputs:
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_sample_backprojection_process(bprb_func_process& pro)
{
  using namespace psm_sample_backprojection_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the parameters
   unsigned int n_img_samples = 10;
   unsigned int n_samples_per_ray = 100;
   pro.parameters()->get_value(param_n_image_samples_, n_img_samples);
   pro.parameters()->get_value(param_n_samples_per_ray_, n_samples_per_ray);
   
   vcl_cout << "n_image_samples = " << n_img_samples << vcl_endl;
   vcl_cout << "n_samples_per_ray = " << n_samples_per_ray << vcl_endl;

  vil_image_view_base_sptr image_base = pro.get_input<vil_image_view_base_sptr>(0);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(1);

  float pix_i = pro.get_input<float>(2);
  float pix_j = pro.get_input<float>(3);
  float pix_sigma = pro.get_input<float>(4);
  bool use_appearance = pro.get_input<bool>(5);

  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(6);

  vcl_string output_filename = pro.get_input<vcl_string>(7);
  
  psm_apm_type apm_type = scene_base->appearance_model_type();

  vcl_vector<vgl_point_3d<double> > points;
 
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
        psm_sample_backprojection(*scene, *img, camera.ptr(), pix_i, pix_j, pix_sigma, use_appearance, points, n_img_samples, n_samples_per_ray);
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
        psm_sample_backprojection(*scene, *img, camera.ptr(), pix_i, pix_j, pix_sigma, use_appearance, points, n_img_samples, n_samples_per_ray);
        break;
      }
    case PSM_APM_MOG_RGB:
      {   
        // convert image to floating point
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
        psm_sample_backprojection(*scene, img_rgb, camera.ptr(), pix_i, pix_j, pix_sigma, use_appearance, points, n_img_samples, n_samples_per_ray);
        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
        // convert image to floating point
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
        psm_sample_backprojection(*scene, img_rgb, camera.ptr(), pix_i, pix_j, pix_sigma, use_appearance, points, n_img_samples, n_samples_per_ray);
        break;
      }

    default:
      vcl_cerr << "error - psm_sample_backprojection_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

  // write output to disk
  vcl_ofstream ofs(output_filename.c_str());
  if (!ofs.good()) {
    vcl_cerr << "error writing to output file " << output_filename << vcl_endl;
    return false;
  }
  vcl_vector<vgl_point_3d<double> >::const_iterator pit = points.begin();
  //ofs << points.size() << vcl_endl;
  for (; pit != points.end(); ++pit) {
    ofs << pit->x() << " " << pit->y() << " " << pit->z() << vcl_endl;
  }
  
  return true;
}
