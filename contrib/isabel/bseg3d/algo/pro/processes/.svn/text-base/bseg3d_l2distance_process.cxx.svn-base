
//This is lemsvxl/contrib/isabel/algo/pro/processes/bseg3d_l2distance_process.cxx
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



//: set input and output types
bool bseg3d_l2distance_process_cons(bprb_func_process& pro)
{

   // Inputs
  // 0. Path to mog grid, filr must exist
  // 1. Path to mask grid, if file doesn't exist a "ones" mask is created
  // 2. Path to output grid, if file exists it gets overwritten
  // 3. nx (grid size)
  // 4. ny
  // 5. nz
  vcl_vector<vcl_string> input_types_(6);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";


  // No outputs to the database. The resulting grid is stored on disk
  vcl_vector<vcl_string> output_types_(0);

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool bseg3d_l2distance_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.n_inputs() != 6)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << 6 << vcl_endl;
    return false;
  }

  vcl_string apm_path = pro.get_input<vcl_string>(0);
  vcl_string mask_path = pro.get_input<vcl_string>(1);
  vcl_string output_path = pro.get_input<vcl_string>(2);
  unsigned nx = pro.get_input<unsigned>(3);
  unsigned ny = pro.get_input<unsigned>(4);
  unsigned nz = pro.get_input<unsigned>(5);

 //get the grids
  vgl_vector_3d<unsigned> grid_size(nx,ny,nz);
  
  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;
  bvxm_voxel_grid_base_sptr apm_base = new bvxm_voxel_grid<mix_gauss_type>(apm_path, grid_size);

  //giving a mask is optional, if mask path does not exist, then an all "ones" mask is created
  //the mask path should still be specified as an input
  bool mask_exists = false;
  if(vul_file::exists(mask_path))
    mask_exists = true;

  bvxm_voxel_grid<float> *mask_base = new bvxm_voxel_grid<float>(mask_path, grid_size);
  if (!mask_exists){
    vcl_cout<< "Mask file does not exist, creating dummy mask\n";
    mask_base->initialize_data(float(1));
  }

  bvxm_voxel_grid_base_sptr output_base = new bvxm_voxel_grid<float>(output_path, grid_size);

  //calculate distances
  bseg3d_explore_mixtures explorer;
  explorer.calculate_l2distances(apm_base, mask_base, output_base,false);
  
  return true;
}
