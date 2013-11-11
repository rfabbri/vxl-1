//This is lemsvxl/contrib/dec/psm/algo/pro/processes/psm_clean_unobserved_process.cxx
//:
// \file
// \brief A process for cleaning up unobserved cells
// \author Daniel Crispell
// \date 04/30/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim

#include <vcl_string.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>
#include <psm/algo/psm_clean_unobserved.h>

#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_simple_grey_processor.h>

namespace psm_clean_unobserved_process_globals
{
  const unsigned int n_inputs_ = 3;
}


//: set input and output types
bool psm_clean_unobserved_process_cons(bprb_func_process& pro)
{
  using namespace psm_clean_unobserved_process_globals;

  // input[0]: The scene
  // input[1]: The minimum number of observations a cell must have
  // input[2]: The minimum max visibility a cell must have

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "psm_scene_base_sptr";
  input_types_[1] = "unsigned";
  input_types_[2] = "float";

  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}


//: Initializes a scene block to the given subdivision level
bool psm_clean_unobserved_process(bprb_func_process &pro)
{

 using namespace psm_clean_unobserved_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the inputs
  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(0);
  psm_apm_type apm_type = scene_base->appearance_model_type();

  unsigned int nobs_thresh = pro.get_input<unsigned int>(1);
  float maxvis_thresh = pro.get_input<float>(2);


  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_clean_unobserved(*scene, nobs_thresh, maxvis_thresh);
        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
         psm_clean_unobserved(*scene, nobs_thresh, maxvis_thresh);
        break;
      }
       case PSM_APM_MOG_RGB:
      {
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
         psm_clean_unobserved(*scene, nobs_thresh, maxvis_thresh);
        break;
      }
       case PSM_APM_SIMPLE_RGB:
      {
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_clean_unobserved(*scene, nobs_thresh, maxvis_thresh);
        break;
      }
    default:
      vcl_cerr << "error - psm_clean_unobserved_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

  return true;
}
