//This is lemsvxl/contrib/isabel/algo/pro/processes/bseg3d_multiply_grids_process.cxx
//:
// \file
// \brief A process for multiplying two grids. 
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
#include <bvxm/bvxm_voxel_slab.h>



//: set input and output types
bool bseg3d_multiply_grids_process_cons(bprb_func_process& pro)
{

   // Inputs
  // 0. Path to grid1
  // 1. Path to grid2
  // 2. Path to output grid
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
bool bseg3d_multiply_grids_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.n_inputs() != 6)
  {
    vcl_cout << pro.name() << "The number of inputs should be " << 6 << vcl_endl;
    return false;
  }

  vcl_string path1 = pro.get_input<vcl_string>(0);
  vcl_string path2 = pro.get_input<vcl_string>(1);
  vcl_string output_path = pro.get_input<vcl_string>(2);
  unsigned nx = pro.get_input<unsigned>(3);
  unsigned ny = pro.get_input<unsigned>(4);
  unsigned nz = pro.get_input<unsigned>(5);

 //get  and create the grids
  vgl_vector_3d<unsigned> grid_size(nx,ny,nz);
  
  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;
  typedef float datatype;

  bvxm_voxel_grid<datatype> grid1(path1, grid_size);
  bvxm_voxel_grid<datatype> grid2(path2, grid_size);
  bvxm_voxel_grid<datatype> grid_out(output_path, grid_size);
  
 
  //multipy
  bvxm_voxel_grid<datatype>::iterator grid1_it = grid1.begin();
  bvxm_voxel_grid<datatype>::iterator grid2_it = grid2.begin();
  bvxm_voxel_grid<datatype>::iterator grid_out_it = grid_out.begin();

  for(; grid1_it != grid1.end(); ++grid1_it, ++grid2_it, ++grid_out_it)
  {

     bvxm_voxel_slab<datatype>::iterator slab1_it = (*grid1_it).begin();
     bvxm_voxel_slab<datatype>::iterator slab2_it = (*grid2_it).begin();
     bvxm_voxel_slab<datatype>::iterator slab_out_it = (*grid_out_it).begin();

     for(; slab1_it!=(*grid1_it).end(); ++slab1_it ,++slab2_it, ++slab_out_it)
     {
       //(*slab_out_it) = (1.0f/(1.0f + vcl_abs(*slab1_it)))* vcl_max((*slab2_it), 0.0f);
      //(*slab_out_it) = (*slab1_it)+ vcl_log(vcl_max(*slab2_it, 0.0000001f));
      (*slab_out_it) =(*slab1_it)*(*slab2_it);
     }


  }
  return true;
}
