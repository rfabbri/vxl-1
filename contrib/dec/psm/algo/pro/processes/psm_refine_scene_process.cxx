//This is lemsvxl/contrib/dec/psm/algo/pro/processes/psm_refine_scene_process.cxx
//:
// \file
// \brief A process for refining the octree cells in a scene.
// \author Daniel Crispell
// \date 02/05/2009
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
#include <psm/algo/psm_refine_scene.h>

#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_simple_grey_processor.h>

namespace psm_refine_scene_process_globals
{
  const unsigned int n_inputs_ = 3;
  const unsigned int n_output_ = 1;
}


//: set input and output types
bool psm_refine_scene_process_cons(bprb_func_process& pro)
{
  using namespace psm_refine_scene_process_globals;

  // input[0]: The scene
  // input[1]: The threshold for occlusion probability for refining octree cells.
  // input[2]: "true" if appearance of refined cells is to be reset

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "psm_scene_base_sptr";
  input_types_[1] = "float";
  input_types_[2] = "bool";

  // output[0]: The number of leaf nodes in the scene
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "unsigned int";

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Initializes a scene block to the given subdivision level
bool psm_refine_scene_process(bprb_func_process &pro)
{

 using namespace psm_refine_scene_process_globals;

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

  bool reset_refined_appearance = pro.get_input<bool>(2);

  unsigned int ncells = 0;

  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_refine_scene(*scene, occlusion_prob_thresh, reset_refined_appearance);
        ncells = scene->num_cells();
        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {
        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_refine_scene(*scene, occlusion_prob_thresh, reset_refined_appearance);
        ncells = scene->num_cells();
        break;
      }
       case PSM_APM_MOG_RGB:
      {
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_refine_scene(*scene, occlusion_prob_thresh, reset_refined_appearance);
        ncells = scene->num_cells();
        break;
      }
       case PSM_APM_SIMPLE_RGB:
      {
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        psm_refine_scene(*scene, occlusion_prob_thresh, reset_refined_appearance);
        ncells = scene->num_cells();
        break;
      }
    default:
      vcl_cerr << "error - psm_refine_scene_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

  // set output
  pro.set_output_val<unsigned int>(0,ncells);

  return true;
}
