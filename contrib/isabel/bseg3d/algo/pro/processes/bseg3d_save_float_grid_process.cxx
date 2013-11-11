
//This is lemsvxl/contrib/isabel/algo/pro/processes/bseg3d_save_float_grid_process.cxx
//:
// \file
// \brief A process for saving a floating point grid to Dristhi format
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



//: set input and output types
bool bseg3d_save_float_grid_process_cons(bprb_func_process& pro)
{
   // Inputs
  // 0. The path to the grid
  // 1. The output path
  // 2. nx //size of grid
  // 3. ny
  // 4. nz
  vcl_vector<vcl_string> input_types_(5);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";

  // No outputs
  vcl_vector<vcl_string> output_types_(0);

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool bseg3d_save_float_grid_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.n_inputs() != 5)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << 5 << vcl_endl;
    return false;
  }


  vcl_string grid_path = pro.get_input<vcl_string>(0);
 
  vcl_string volume_path = pro.get_input<vcl_string>(1);

  unsigned nx = pro.get_input<unsigned>(2);
  unsigned ny = pro.get_input<unsigned>(3);
  unsigned nz = pro.get_input<unsigned>(4);
 
  // create the grid from in memory file
  bvxm_voxel_grid<float> *grid = new bvxm_voxel_grid<float>(grid_path,vgl_vector_3d<unsigned>(nx, ny, nz));
   
  //save the grid
  bseg3d_explore_mixtures explorer;
  explorer.save_float_grid_raw(grid,volume_path);
  
  return true;
}
