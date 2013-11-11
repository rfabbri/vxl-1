//This is lemsvxl/contrib/dec/psm/pro/processes/psm_mog_to_simple_process.cxx
//:
// \file
// \brief A process for converting a mixture of Gaussian appearance model scene to one which uses the "simple" model
// \author Daniel Crispell
// \date 12/22/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim

#include <psm/psm_apm_traits.h>
#include <psm/psm_mog_grey_processor.h>
#include <psm/psm_simple_grey_processor.h>
#include <psm/psm_mog_rgb_processor.h>
#include <psm/psm_simple_rgb_processor.h>
#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <bgeo/bgeo_lvcs.h>
#include <bgeo/bgeo_lvcs_sptr.h>

#include <vul/vul_file.h>


namespace psm_mog_to_simple_process_globals
{
  const unsigned int n_inputs_ = 2;
}


//: set input and output types
bool psm_mog_to_simple_process_cons(bprb_func_process& pro)
{
  using namespace psm_mog_to_simple_process_globals;

  // input[0]: The mog scene
  // input[1]: The simple scene
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "psm_scene_base_sptr";
  input_types_[1] = "psm_scene_base_sptr";
  
  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}


//: converts a scene with mog appearance to simple appearance
bool psm_mog_to_simple_process(bprb_func_process &pro)
{
  using namespace psm_mog_to_simple_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the inputs
  psm_scene_base_sptr scene_base_mog = pro.get_input<psm_scene_base_sptr>(0); 
  psm_scene_base_sptr scene_base_simple = pro.get_input<psm_scene_base_sptr>(1);
 
  psm_apm_type apm_type = scene_base_mog->appearance_model_type();

  vcl_cout << "apm_type = " << apm_type << vcl_endl;
  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene_mog = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base_mog.ptr());
        if (!scene_mog) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        // make sure other scene is SIMPLE_GREY
        apm_type = scene_base_simple->appearance_model_type();
        if (apm_type != PSM_APM_SIMPLE_GREY) {
          vcl_cerr << "Error: appearance model type of second input should be SIMPLE_GREY" << vcl_endl;
          return false;
        }
        psm_scene<PSM_APM_SIMPLE_GREY> *scene_simple = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base_simple.ptr());
        if (!scene_simple) {
          vcl_cerr << "error casting scene base to scene (second input)" << vcl_endl;
          return false;
        }
        // do the conversion one cell at a time
        psm_scene<PSM_APM_MOG_GREY>::block_index_iterator bit = scene_mog->block_index_begin();
        for (; bit != scene_mog->block_index_end(); ++bit) {
          hsds_fd_tree<psm_sample<PSM_APM_MOG_GREY>,3> &block_mog = scene_mog->get_block(*bit);
          hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3> block_simple = block_mog.twin_tree(psm_sample<PSM_APM_SIMPLE_GREY>());
          
          hsds_fd_tree<psm_sample<PSM_APM_MOG_GREY>,3>::const_iterator cell_it_mog = block_mog.begin();
          hsds_fd_tree<psm_sample<PSM_APM_SIMPLE_GREY>,3>::iterator cell_it_simple = block_simple.begin();
          // for each cell in block
          for (; cell_it_mog != block_mog.end(); ++cell_it_mog, ++cell_it_simple) {
            // create new appearance with identical mean and P(mean)
            psm_apm_traits<PSM_APM_MOG_GREY>::obs_datatype mean = psm_apm_traits<PSM_APM_MOG_GREY>::apm_processor::expected_color(cell_it_mog->second.appearance);
            float P_mean = psm_apm_traits<PSM_APM_MOG_GREY>::apm_processor::prob_density(cell_it_mog->second.appearance, mean);
            float sigma = (float)(vnl_math::one_over_sqrt2pi  / P_mean);
            psm_apm_traits<PSM_APM_SIMPLE_GREY>::apm_datatype appearance(mean,sigma);
            cell_it_simple->second = psm_sample<PSM_APM_SIMPLE_GREY>(cell_it_mog->second.alpha,appearance);
          }
          scene_simple->set_block(*bit, block_simple);
        }
        break;
      }
    case PSM_APM_MOG_RGB:
      {
        // TODO
        vcl_cout << "conversion not implemented for rgb yet" << vcl_endl;
        break;
      }
   
    default:
      vcl_cerr << "error - psm_mog_to_simple_process: unexpected appearance model type " << apm_type << vcl_endl;
      return false;
  }

  return true;
}
