
//This is lemsvxl/contrib/isabel/algo/pro/processes/bseg3d_threshold_world_process.cxx
//:
// \file
// \brief A process for thresholding the occupancy grid of a voxel world
// \author Isabel Restrepo
// \date 03/11/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim

#include <vcl_string.h>
#include <vul/vul_file.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_voxel_traits.h>

#include <bseg3d/algo/bseg3d_explore_mixtures.h>



namespace bseg3d_threshold_world_process_globals
{
  // Inputs
  // 0. A voxel world
  // 1. The threshold 
  // 2. Output path for thresholded world
  const unsigned int n_inputs_ = 7;

  //Outputs
  // This process has no outputs to the database because the outputs grids are disk based
  const unsigned int n_outputs_ = 0;
  
}


//: set input and output types
bool bseg3d_threshold_world_process_cons(bprb_func_process& pro)
{
  using namespace bseg3d_threshold_world_process_globals;

   // Inputs
  // 0. Path of occupancy grid to threshold
  // 1. Path for thresholded occupancy grid (values above threshold are unchanged, otherwise they are set to 0)
  // 2. Path for mask (values above threshol are set to 1.0, 0 otherwise)
  // 3. Threshold
  // 4. nx (size of grids)
  // 5. ny 
  // 6. nz
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  input_types_[3] = "float";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "unsigned";


  // 0. A voxel world 
  vcl_vector<vcl_string> output_types_(n_outputs_);

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Griven an input grid and a threshold, this process process returns two grids;
//  One being the thresholded grid, the other being the mask such that
//  original * mask = thresholded. All grids are disk based.
bool bseg3d_threshold_world_process(bprb_func_process& pro)
{
  using namespace bseg3d_threshold_world_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << n_inputs_ << vcl_endl;
    return false;
  }

  vcl_string grid_in_path = pro.get_input<vcl_string>(0);
  vcl_string grid_out_path = pro.get_input<vcl_string>(1);
  vcl_string mask_path = pro.get_input<vcl_string>(2);
  float threshold = pro.get_input<float>(3);
  unsigned nx = pro.get_input<unsigned>(4);
  unsigned ny = pro.get_input<unsigned>(5);
  unsigned nz = pro.get_input<unsigned>(6);

  //construct the grids
  vgl_vector_3d<unsigned> grid_size(nx,ny,nz);
  
  bvxm_voxel_grid_base_sptr grid_in_base = new bvxm_voxel_grid<float>(grid_in_path, grid_size);
  bvxm_voxel_grid_base_sptr grid_out_base = new bvxm_voxel_grid<float>(grid_out_path, grid_size);
  bvxm_voxel_grid_base_sptr mask_base = new bvxm_voxel_grid<float>(mask_path, grid_size);

  // threshold 
  bseg3d_explore_mixtures world_explorer;
  world_explorer.get_region_above_threshold(grid_in_base,grid_out_base,mask_base,threshold);

  return true;
}
