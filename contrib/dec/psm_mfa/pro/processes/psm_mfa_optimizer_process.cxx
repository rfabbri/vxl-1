//This is lemsvxl/contrib/dec/psm/pro/processes/psm_mfa_optimizer_process.cxx
//:
// \file
// \brief A process for updating a psm_scene with a set of images, using the mean-field approximation optimization method
// \author Daniel Crispell
// \date 12/18/2009
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

#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>

#include <psm_mfa/psm_mfa_optimizer.h>

namespace psm_mfa_optimizer_process_globals
{
  const unsigned int n_inputs_ = 3;
  const unsigned int n_outputs_ = 0;
  //Define parameters here
}


//: set input and output types
bool psm_mfa_optimizer_process_cons(bprb_func_process& pro)
{
  using namespace psm_mfa_optimizer_process_globals;

  // process takes 3 inputs:
  //input[0]: The scene
  //input[1]: The filename of the text file containing list of image names

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "psm_scene_base_sptr";
  input_types_[1] = "vcl_string";

  // process has 0 outputs:
  vcl_vector<vcl_string> output_types_(n_outputs_);

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_mfa_optimizer_process(bprb_func_process& pro)
{
  using namespace psm_mfa_optimizer_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the parameters 
  bool black_background = true;

  // get the inputs
  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(0);
  psm_apm_type apm_type = scene_base->appearance_model_type();

  vcl_string image_list_fname = pro.get_input<vcl_string>(1);

  // extract list of image_ids from file
  vcl_ifstream ifs(image_list_fname.c_str());
  if (!ifs.good()) {
    vcl_cerr << "error opening file " << image_list_fname << vcl_endl;
    return false;
  }
  vcl_vector<vcl_string> image_filenames;
  vcl_vector<vcl_string> camera_filenames;

  unsigned int n_images = 0;
  ifs >> n_images;
  for (unsigned int i=0; i<n_images; ++i) {
    vcl_string img_fname, cam_fname;
    ifs >> img_fname;
    ifs >> cam_fname;
    image_filenames.push_back(img_fname);
    camera_filenames.push_back(cam_fname);
  }
  ifs.close();

  switch (apm_type) {
    case PSM_APM_SIMPLE_GREY:
      {
        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_mfa_optimizer<PSM_APM_SIMPLE_GREY> optimizer(*scene, image_filenames, camera_filenames, black_background);
        optimizer.optimize();

        break;
      }
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        //psm_mfa_optimizer<PSM_APM_MOG_GREY> optimizer(*scene, image_filenames, camera_filenames, black_background);
        //optimizer.optimize();

        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {     
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        //psm_mfa_optimizer<PSM_APM_SIMPLE_RGB> optimizer(*scene, image_filenames, camera_filenames, black_background);
        //optimizer.optimize();


        break;
      }
      case PSM_APM_MOG_RGB:
      {     
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        //psm_mfa_optimizer<PSM_APM_MOG_RGB> optimizer(*scene, image_filenames, camera_filenames, black_background);
        //optimizer.optimize();


        break;
      }
    default:
      vcl_cerr << "error - psm_mfa_optimizer_process: unsupported appearance model type " << apm_type << vcl_endl;
      return false;
  }

  //store output
  // (none)

  return true;
}
