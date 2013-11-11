//This is lemsvxl/contrib/isabel/algo/pro/processes/bseg3d_merge_mixtures_process.cxx
//:
// \file
// \brief A process for merging gaussian mixtures in a grid into a single gaussian
// \author Isabel Restrepo
// \date 05/5/2009
//
// \verbatim
//  Modifications
//    
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_func_process.h>
#include <bseg3d/algo/bseg3d_explore_mixtures.h>



//: set input and output types
bool bseg3d_merge_mixtures_process_cons(bprb_func_process& pro)
{

   // Inputs
  // 0. Path to input grid(the one with gaussian mixtures)
  // 1. Path to univariate gaussian grid
  // 3. nx (grid size)
  // 4. ny
  // 5. nz
  vcl_vector<vcl_string> input_types_(5);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";


  // No outputs to the database. The resulting grid is stored on disk
  vcl_vector<vcl_string> output_types_(0);

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool bseg3d_merge_mixtures_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.n_inputs() != 5)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << 5 << vcl_endl;
    return false;
  }

  vcl_string apm_path = pro.get_input<vcl_string>(0);
  vcl_string output_path = pro.get_input<vcl_string>(1);
  unsigned nx = pro.get_input<unsigned>(2);
  unsigned ny = pro.get_input<unsigned>(3);
  unsigned nz = pro.get_input<unsigned>(4);

 //get the grids
  vgl_vector_3d<unsigned> grid_size(nx,ny,nz);
  
  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;

  bvxm_voxel_grid_base_sptr apm_base = new bvxm_voxel_grid<mix_gauss_type>(apm_path, grid_size);

  bvxm_voxel_grid_base_sptr output_base = new bvxm_voxel_grid<gauss_type>(output_path, grid_size);

  //merge mixtures
  bseg3d_explore_mixtures explorer;
  explorer.merge_mixtures(apm_base, output_base);
  
  return true;
}

