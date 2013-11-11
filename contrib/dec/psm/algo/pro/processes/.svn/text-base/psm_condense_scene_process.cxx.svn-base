//This is lemsvxl/contrib/dec/psm/algo/pro/processes/psm_condense_scene_process.cxx
//:
// \file
// \brief A process for condensing the octree cells in a scene.
// \author Daniel Crispell
// \date 02/05/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim


#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/algo/psm_condense_scene.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <vcl_string.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>



namespace psm_condense_scene_process_globals
{
  const unsigned int n_inputs_ = 3;
}


//: set input and output types
bool psm_condense_scene_process_cons(bprb_func_process& pro)
{
  using namespace psm_condense_scene_process_globals;

  // input[0]: The scene
  // input[1]: The threshold for occlusion probability for collapsing octree cells.
  // input[2]: The minimum octree level

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "psm_scene_base_sptr";
  input_types_[1] = "float";
  input_types_[2] = "unsigned";

  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}


//: Initializes a scene block to the given subdivision level
bool psm_condense_scene_process(bprb_func_process &pro)
{
  using namespace psm_condense_scene_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the inputs
  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(0);
  psm_apm_type apm_type = scene_base->appearance_model_type();

  float occlusion_prob_thresh = pro.get_input<float>(1);
  unsigned int min_level = pro.get_input<unsigned>(2);

  vcl_cout << "condensing scene : occlusion_prob_thresh = " << occlusion_prob_thresh << " min_level = " << min_level << vcl_endl;

  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_condense_scene(*scene, occlusion_prob_thresh, min_level);
        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_condense_scene(*scene, occlusion_prob_thresh, min_level);
        break;
      }
       case PSM_APM_MOG_RGB:
      {
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_condense_scene(*scene, occlusion_prob_thresh, min_level);
        break;
      }
       case PSM_APM_SIMPLE_RGB:
      {
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_condense_scene(*scene, occlusion_prob_thresh, min_level);
        break;
      }
    default:
      vcl_cerr << "error - psm_condense_scene_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

  return true;
}
