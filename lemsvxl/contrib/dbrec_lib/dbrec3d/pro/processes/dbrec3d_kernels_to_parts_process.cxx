//:
// \brief A process to convert a from bvpl kernels to dbrec3d_parts
// \file
// \author Isabel Restrepo
// \date 21-Jun-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <boxm/boxm_scene.h>

#include <bvpl/kernels/bvpl_kernel.h>

#include <dbrec3d/dbrec3d_parts_manager.h>
#include <dbrec3d/dbrec3d_context_manager.h>
#include <dbrec3d/dbrec3d_models.h>

//:global variables
namespace dbrec3d_kernels_to_parts_process_globals 
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}


//:sets input and output types
bool dbrec3d_kernels_to_parts_process_cons(bprb_func_process& pro)
{
  using namespace dbrec3d_kernels_to_parts_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr"; //The scene containing bvpl kernel responses
  input_types_[1] = "bvpl_kernel_sptr"; //The kernel
  

  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "int"; //The newly created context id
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//:the process
bool dbrec3d_kernels_to_parts_process(bprb_func_process& pro)
{
  using namespace dbrec3d_kernels_to_parts_process_globals;
  
  //get inputs
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  bvpl_kernel_sptr kernel= pro.get_input<bvpl_kernel_sptr>(1);

  if (!scene_base.ptr()) {
    vcl_cerr <<  " :-- Kernel scene is not valid!\n";
    return false;
  }
  
  if ( !kernel ) {
    vcl_cerr << pro.name() << " :-- Kernel is not valid!\n";
    return false;
  }
  
  
  //create parts scene
  typedef boct_tree<short, float> float_tree_type;
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef boct_tree<short, gauss_type > gauss_tree_type;
  int context_id =-1;

  if (boxm_scene<float_tree_type> *scene_in = dynamic_cast<boxm_scene< float_tree_type >* >(scene_base.as_pointer())){
    boxm_scene<boct_tree<short, dbrec3d_part_instance > > *parts_scene =
    new boxm_scene<boct_tree<short, dbrec3d_part_instance > >(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
    parts_scene->set_paths(scene_in->path(), "parts_scene");
    parts_scene->set_appearance_model(DBREC3D_PART_INSTANCE);
    parts_scene->write_scene("/dbrec3d_parts_scene.xml");
    //convert the scene into a context
    dbrec3d_kernel_model model;
  
    context_id =CONTEXT_MANAGER->register_context(scene_in, model, kernel, parts_scene);
    
    //clean memory
    scene_in->unload_active_blocks();
    parts_scene->unload_active_blocks();
    
    
    }
  else if (boxm_scene<gauss_tree_type> *scene_in = dynamic_cast<boxm_scene< gauss_tree_type >* >(scene_base.as_pointer())){
    boxm_scene<boct_tree<short, dbrec3d_part_instance > > *parts_scene = 
    new boxm_scene<boct_tree<short, dbrec3d_part_instance > >(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
    parts_scene->set_paths(scene_in->path(), "parts_scene");
    parts_scene->set_appearance_model(DBREC3D_PART_INSTANCE);
    parts_scene->write_scene("/dbrec3d_parts_scene.xml");
    //convert the scene into a context
    dbrec3d_kernel_model model;
    context_id = CONTEXT_MANAGER->register_context(scene_in, model, kernel, parts_scene);
    
    //clean memory
    scene_in->unload_active_blocks();
    parts_scene->unload_active_blocks();
  }
  else{
    vcl_cerr << "Error in dbrec3d_kernels_to_parts_process: Scene type no supported" <<vcl_endl;
    return false;
  }
  

  
  //store output
  pro.set_output_val<int>(0, context_id);
  
  return true;
}