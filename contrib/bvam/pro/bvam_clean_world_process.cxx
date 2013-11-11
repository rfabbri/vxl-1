#include "bvam_clean_world_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>


#include <bvam/bvam_voxel_world.h>


bvam_clean_world_process::bvam_clean_world_process()
{
  // process takes 1 input: 
  //input[0]: The voxel world

  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0] = "bvam_voxel_world_sptr";

  // process has 0 outputs.
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
  
}


bool bvam_clean_world_process::execute()
{

  // Sanity check
  if(!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<bvam_voxel_world_sptr>* input0 = 
    static_cast<brdb_value_t<bvam_voxel_world_sptr>* >(input_data_[0].ptr());
  bvam_voxel_world_sptr world = input0->value();

  return world->clean_grids();

}



