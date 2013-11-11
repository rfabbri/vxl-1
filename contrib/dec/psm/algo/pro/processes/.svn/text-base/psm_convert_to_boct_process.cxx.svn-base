//This is lemsvxl/contrib/dec/psm/pro/processes/psm_convert_to_boct_process.cxx
//:
// \file
// \brief A process for coverting psm and hsds to boct
// \author Vishal Jain
// \date 04/01/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim

// compile errors due to lack of "boxm_scene_short_sample_mog_grey_sptr" definition -DEC
#ifdef COMPILE_ERRORS_FIXED 

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
#include <psm/algo/psm_convert_to_boct.h>

#include <boct/boct_tree.h>
#include <boxm/boxm_sample.h>

namespace psm_convert_to_boct_process_globals
{
  const unsigned int n_inputs_ = 3;
  const unsigned int n_outputs_ = 1;
  //Define parameters here
  //const vcl_string param_use_black_background_ =  "use_black_background";
}


//: set input and output types
bool psm_convert_to_boct_process_cons(bprb_func_process& pro)
{
  using namespace psm_convert_to_boct_process_globals;

  // process takes 4 inputs:
  //input[0]: The camera to render the expected image from
  //input[1]: path of the output directory
  //input[2]: blocks filename prefix
  

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "psm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";

   // process has 2 outputs:
  // output[0]: The rendered frame
  // output[1]: A confidence map, giving probability that each pixel was seen from the original view.

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0]= "boxm_scene_short_sample_mog_grey_sptr";
  //output_types_[1]= "vil_image_view_base_sptr";  

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool psm_convert_to_boct_process(bprb_func_process& pro)
{
  using namespace psm_convert_to_boct_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  //: get the scene 
  psm_scene_base_sptr scene_base = pro.get_input<psm_scene_base_sptr>(0);
  //: get the path and prefix
  vcl_string path=pro.get_input<vcl_string>(1);
  vcl_string prefix=pro.get_input<vcl_string>(2);
  //: create boxm scene and copy the data from psm_scene

  //: get all the blocks

  psm_apm_type apm_type = scene_base->appearance_model_type();


  
  switch (apm_type) {
    case PSM_APM_MOG_GREY:
      {
        psm_scene<PSM_APM_MOG_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > *boxmscene
            =psm_convert_to_boct<PSM_APM_MOG_GREY,boxm_sample<BOXM_APM_MOG_GREY>,short > (*scene,path,prefix);
        boxmscene->write_scene();
        pro.set_output_val<boxm_scene_short_sample_mog_grey_sptr>(0, boxmscene);

        break;
      }
    case PSM_APM_SIMPLE_GREY:
      {

        psm_scene<PSM_APM_SIMPLE_GREY> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_GREY>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        break;
      }
    case PSM_APM_MOG_RGB:
      {
         
        psm_scene<PSM_APM_MOG_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_MOG_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }        
        //boxm_scene_short_sample_mog_grey_sptr boxmscene(
        //    psm_convert_to_boct<PSM_APM_MOG_RGB,boxm_sample<BOXM_APM_MOG_GREY>,short > (*scene,path,prefix));
        //  pro.set_output_val<boxm_scene_short_point_double_sptr>(0, boxmscene);

        break;
      }
    case PSM_APM_SIMPLE_RGB:
      {
        psm_scene<PSM_APM_SIMPLE_RGB> *scene = dynamic_cast<psm_scene<PSM_APM_SIMPLE_RGB>*>(scene_base.ptr());
        if (!scene) {
          vcl_cerr << "error casting scene_base to scene" << vcl_endl;
          return false;
        }
        break;
      }

    default:
      vcl_cerr << "error - psm_render_expected_process: unknown appearance model type " << apm_type << vcl_endl;
      return false;
  }

  //store output
  return true;
}
#endif
