//This is lemsvxl/contrib/dec/psm/pro/processes/psm_ray_probe_process.cxx
//:
// \file
// \brief A process for storing alpha and depth values along a camera ray
// \author Daniel Crispell
// \date 07/07/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim

#include <fstream>
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
#include <psm/psm_cell_id.h>
#include <psm/algo/psm_ray_probe.h>

namespace psm_ray_probe_process_globals
{
  const unsigned int n_inputs_ = 6;
  const unsigned int n_outputs_ = 0;
  //Define parameters here
 
}


//: set input and output types
bool psm_ray_probe_process_cons(bprb_func_process& pro)
{
  using namespace psm_ray_probe_process_globals;

  // process takes 5 inputs:
  //input[0]: The image
  //input[0]: The camera 
  //input[1]: pixel index (x)
  //input[2]: pixel index (y)
  //input[3]: The scene
  //input[4]: the filename to write the probe to

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "float";
  input_types_[3] = "float";
  input_types_[4] = "psm_scene_base_sptr";
  input_types_[5] = vcl_string";

   // process has 0 outputs:
  
  std::vector<std::string> output_types_(n_outputs_);
  
  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_ray_probe_process(bprb_func_process& pro)
{
  using namespace psm_ray_probe_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << "The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }


  vil_image_view_base_sptr image_base = pro.get_input<vil_image_view_base_sptr>(0);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(1);

  float probe_i = pro.get_input<float>(2);
  float probe_j = pro.get_input<float>(3);

  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(4);

  std::string output_filename = pro.get_input<std::string>(5);
  
  psm_apm_type apm_type = scene_base->appearance_model_type();

  std::vector<float> depth_vals;
  std::vector<float> alpha_vals;
  std::vector<float> PI_vals;
  std::vector<psm_cell_id> cell_ids;

  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        vil_image_view_base_sptr img_base_conv = vil_convert_stretch_range(psm_apm_traits<PSM_APM_MOG_GREY>::obs_mathtype(),image_base);
        vil_image_view<psm_apm_traits<PSM_APM_MOG_GREY>::obs_mathtype> *img = dynamic_cast<vil_image_view<psm_apm_traits<PSM_APM_MOG_GREY>::obs_mathtype>*>(img_base_conv.ptr());
        if (!img) {
          std::cerr << "error casting image to appropriate type " << std::endl;
          return false;
        }

        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          std::cerr << "error casting scene_base to scene" << std::endl;
          return false;
        }
        psm_ray_probe(*scene, *img, camera.ptr(), probe_i, probe_j, depth_vals, alpha_vals, PI_vals, cell_ids);
        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        vil_image_view_base_sptr img_base_conv = vil_convert_stretch_range(psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype(),image_base);
        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype> *img = dynamic_cast<vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_mathtype>*>(img_base_conv.ptr());
        if (!img) {
          std::cerr << "error casting image to appropriate type " << std::endl;
          return false;
        }

        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          std::cerr << "error casting scene_base to scene" << std::endl;
          return false;
        }
        psm_ray_probe(*scene, *img, camera.ptr(), probe_i, probe_j, depth_vals, alpha_vals, PI_vals, cell_ids);
        break;
      }
    case PSM_APM_MOG_RGB:
      {   
        // convert image to floating point
        vil_image_view_base_sptr img_base_conv = vil_convert_to_component_order(vil_convert_stretch_range(psm_apm_traits<PSM_APM_MOG_RGB>::obs_mathtype(),image_base));
        vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_mathtype> *img = dynamic_cast<vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_mathtype>*>(img_base_conv.ptr());
        if (!img) {
          std::cerr << "error casting image to appropriate type " << std::endl;
          return false;
        }
        vil_image_view<psm_apm_traits<PSM_APM_MOG_RGB>::obs_datatype> img_rgb = vil_view_as_rgb(*img);

        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          std::cerr << "error casting scene_base to scene" << std::endl;
          return false;
        }
        psm_ray_probe(*scene, img_rgb, camera.ptr(), probe_i, probe_j, depth_vals, alpha_vals, PI_vals, cell_ids);
        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
        // convert image to floating point
        vil_image_view_base_sptr img_base_conv = vil_convert_to_component_order(vil_convert_stretch_range(psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_mathtype(),image_base));
        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_mathtype> *img = dynamic_cast<vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_mathtype>*>(img_base_conv.ptr());
        if (!img) {
          std::cerr << "error casting image to appropriate type " << std::endl;
          return false;
        }
        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype> img_rgb = vil_view_as_rgb(*img);

        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          std::cerr << "error casting scene_base to scene" << std::endl;
          return false;
        }
        psm_ray_probe(*scene, img_rgb, camera.ptr(), probe_i, probe_j, depth_vals, alpha_vals, PI_vals, cell_ids);
        break;
      }

    default:
      std::cerr << "error - psm_ray_probe_process: unknown appearance model type " << apm_type << std::endl;
      return false;
  }

  // write output to disk
  std::ofstream ofs(output_filename.c_str());
  if (!ofs.good()) {
    std::cerr << "error writing to output file " << output_filename << std::endl;
    return false;
  }
  std::vector<float>::const_iterator depth_it = depth_vals.begin();
  for (; depth_it != depth_vals.end(); ++depth_it) {
    ofs << *depth_it << " ";
  }
  ofs << std::endl;

  std::vector<float>::const_iterator alpha_it = alpha_vals.begin();
  for (; alpha_it != alpha_vals.end(); ++alpha_it) {
    ofs << *alpha_it << " ";
  }
  ofs << std::endl;

  std::vector<float>::const_iterator PI_it = PI_vals.begin();
  for (; PI_it != PI_vals.end(); ++PI_it) {
    ofs << *PI_it << " ";
  }
  ofs << std::endl;

  std::vector<psm_cell_id>::const_iterator id_it = cell_ids.begin();
  for (; id_it != cell_ids.end(); ++id_it) {
    ofs << id_it->block_idx_.x() << " ";
  }
  ofs << std::endl;

  id_it = cell_ids.begin();
  for (; id_it != cell_ids.end(); ++id_it) {
    ofs << id_it->block_idx_.y() << " ";
  }
  ofs << std::endl;

  id_it = cell_ids.begin();
  for (; id_it != cell_ids.end(); ++id_it) {
    ofs << id_it->block_idx_.z() << " ";
  }
  ofs << std::endl;

  id_it = cell_ids.begin();
  for (; id_it != cell_ids.end(); ++id_it) {
    ofs << id_it->cell_idx_.idx << " ";
  }
  ofs << std::endl;

  id_it = cell_ids.begin();
  for (; id_it != cell_ids.end(); ++id_it) {
    ofs << id_it->cell_idx_.lvl << " ";
  }
  ofs << std::endl;

  return true;
}
