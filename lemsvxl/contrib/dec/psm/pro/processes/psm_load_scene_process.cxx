//This is lemsvxl/contrib/dec/psm/pro/processes/psm_load_scene_process.cxx
//:
// \file
// \brief A process for loading a psm_scene from disk.
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

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <string>
#include <iostream>
#include <fstream>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <bgeo/bgeo_lvcs.h>
#include <bgeo/bgeo_lvcs_sptr.h>

#include <vul/vul_file.h>


//:global variables
namespace psm_load_scene_process_globals
{
  //this process takes no inputs
  const unsigned n_outputs_ = 1; 
  //Define parameters here
  const std::string param_storage_directory_ =  "storage_directory";
  const std::string param_appearance_model_type_ = "appearance_model_type";
  const std::string param_origin_x_ = "origin_x";
  const std::string param_origin_y_ = "origin_y";
  const std::string param_origin_z_ = "origin_z";
  const std::string param_block_length_ = "block_length";
  const std::string param_max_levels_ = "max_levels";
  const std::string param_lvcs_path_ = "lvcs_path";
}


//: sets the input and output types
bool psm_load_scene_process_cons(bprb_func_process &pro)
{
  using namespace psm_load_scene_process_globals;
  // set output types
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "psm_scene_base_sptr";
  if(!pro.set_output_types(output_types_)) 
    return false;
  return true;
}



//: Execute the process
bool psm_load_scene_process(bprb_func_process& pro)
{
  using namespace psm_load_scene_process_globals;
  // define and read in the parameters
  std::string storage_dir;
  pro.parameters()->get_value(param_storage_directory_, storage_dir);

  if (!vul_file::is_directory(storage_dir) || !vul_file::exists(storage_dir)) {
    std::cerr << "In psm_load_scene_process::execute() -- input directory is not valid!\n";
    return false;
  }

  std::cout << "In psm_load_scene_process -- storage directory is: " << storage_dir << std::endl;

  std::string appearance_model_type;
  pro.parameters()->get_value(param_appearance_model_type_, appearance_model_type);

  float origin_x, origin_y, origin_z;
  pro.parameters()->get_value(param_origin_x_, origin_x);
  pro.parameters()->get_value(param_origin_y_, origin_y);
  pro.parameters()->get_value(param_origin_z_, origin_z);

  vgl_point_3d<double> origin(origin_x, origin_y, origin_z);

  float block_len;
  pro.parameters()->get_value(param_block_length_, block_len);


  unsigned int max_levels;
  pro.parameters()->get_value(param_max_levels_, max_levels);

  std::cout << "origin = " << origin_x << ", " << origin_y << ", " << origin_z << std::endl;
  std::cout << "block length = " << block_len << std::endl;
  std::cout << "max levels = " << max_levels << std::endl;

  std::string lvcs_path;
  pro.parameters()->get_value(param_lvcs_path_, lvcs_path);

  bgeo_lvcs_sptr lvcs = new bgeo_lvcs();
  if (lvcs_path != "") {
    std::ifstream is(lvcs_path.c_str());
    if (!is)
    {
      std::cerr << " Error opening file  " << lvcs_path << std::endl;
      return false;
    }
    lvcs->read(is);
  }

  psm_scene_base* scene_base = 0;

  if (appearance_model_type == "mog_grey") {
    scene_base = new psm_scene<PSM_APM_MOG_GREY>(origin, block_len, storage_dir, lvcs, max_levels);
  }
  else if (appearance_model_type == "simple_grey") {
    scene_base = new psm_scene<PSM_APM_SIMPLE_GREY>(origin, block_len, storage_dir, lvcs, max_levels);
  }
  else if (appearance_model_type == "mog_rgb") {
    scene_base = new psm_scene<PSM_APM_MOG_RGB>(origin, block_len, storage_dir, lvcs, max_levels);
  }
  else if (appearance_model_type == "simple_rgb") {
    scene_base = new psm_scene<PSM_APM_SIMPLE_RGB>(origin, block_len, storage_dir, lvcs, max_levels);
  }
  else {
    std::cerr << "error! psm_load_scene_process: unknown appearance model type " << appearance_model_type << std::endl;
    return false;
  }

  psm_scene_base_sptr scene_sptr(scene_base);

  pro.set_output_val<psm_scene_base_sptr>(0, scene_sptr);

  return true;
}
