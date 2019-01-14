
#include "bvam_load_voxel_world_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <iostream>
#include <fstream>
#include <bvam/bvam_voxel_world.h>
#include <bvam/bvam_mog_grey_processor.h>

//: Constructor
bvam_load_voxel_world_process::bvam_load_voxel_world_process()
{
  //this process takes one input: the filename of the world_parameters
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0]=std::string";

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "bvam_voxel_world_base_sptr";
  
  //parameters
  /*if( !parameters()->add( "Image file <filename...>" , "image_filename" , bprb_filepath("","*") ))
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }*/

}


//: Destructor
bvam_load_voxel_world_process::~bvam_load_voxel_world_process()
{
}


//: Execute the process
bool
bvam_load_voxel_world_process::execute()
{
  // Sanity check
    if(!this->verify_inputs())
    return false;

  //Retrieve path from input
  brdb_value_t<std::string>* input0 = 
    static_cast<brdb_value_t<std::string>* >(input_data_[0].ptr());

  std::string world_params_fname = input0->value();
 
  std::ifstream ifs(world_params_fname.c_str());
  if (!ifs.is_open()) {
    std::cerr << "error: failed to open world_parameters file " << world_params_fname << std::endl;
    return false;
  }

  bvam_world_params* params = bvam_world_params::instance();
  ifs >> *params;

  bvam_voxel_world_base_sptr vox_world;

  // now create world using default constructor
  switch (params->apm_type()) {
    case bvam_world_params::mog_grey:
      vox_world = static_cast<bvam_voxel_world_base*>(new bvam_voxel_world<bvam_mog_grey_processor>());
      break;
    default:
      std::cerr << "error: don't know how to create a bvam_voxel_world with apm_type = " << params->apm_type() << std::endl;
      return false;
  }

  brdb_value_sptr output0 = new brdb_value_t<bvam_voxel_world_base_sptr>(vox_world);
  
  output_data_[0] = output0;

  return true;
}




