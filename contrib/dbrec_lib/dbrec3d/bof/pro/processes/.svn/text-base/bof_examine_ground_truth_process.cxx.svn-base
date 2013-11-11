//:
// \brief A process to examine voxels that belong to .ply objects
// \file
// \author Isabel Restrepo
// \date 21-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_scene_categories.h>

#include <vul/vul_file_iterator.h>

//:global variables
namespace bof_examine_ground_truth_process_globals 
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bof_examine_ground_truth_process_cons(bprb_func_process& pro)
{
  using namespace bof_examine_ground_truth_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i =0;
  input_types_[i++] = "boxm_scene_base_sptr";  //input_scene
  input_types_[i++] = "vcl_string"; //dir_path to .ply  objects
  input_types_[i++] = "vcl_string"; //name of objects
  input_types_[i++] = "vcl_string"; //dir path to save the output scene
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_examine_ground_truth_process(bprb_func_process& pro)
{
  using namespace bof_examine_ground_truth_process_globals;
  
  //get inputs
  unsigned i =0;
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  vcl_string obj_dir = pro.get_input<vcl_string>(i++);
  vcl_string obj_name= pro.get_input<vcl_string>(i++);
  vcl_string out_dir = pro.get_input<vcl_string>(i++);

  //cast the scene
  boxm_scene<boct_tree<short, float> > *scene_float = dynamic_cast<boxm_scene<boct_tree<short, float> > *> (scene_base.as_pointer());
  
  if (!scene_float) {
    vcl_cerr << "Failed to cast float scene\n";
    return false;
  }
  
  //get the ply objects
  vcl_vector<vcl_string> ply_paths;
  for (vul_file_iterator fn=obj_dir + "/*.ply"; fn; ++fn)
    ply_paths.push_back(fn());
    
 
  //create the output scene to hold labele voxels
  vcl_string aux_scene_path = out_dir + "/obj_scene.xml";
  boxm_scene<boct_tree<short, char> > *aux_scene =
  new boxm_scene<boct_tree<short, char> >(scene_float->lvcs(), scene_float->origin(), scene_float->block_dim(), scene_float->world_dim(), scene_float->max_level(), scene_float->init_level());
  aux_scene->set_appearance_model(BOXM_CHAR);
  aux_scene->set_paths(out_dir, "obj_block");
  aux_scene->write_scene("obj_scene.xml");
  scene_float->clone_blocks_to_type<boct_tree<short,char> >(*aux_scene, 0);
  double finest_cell_length = scene_float->finest_cell_length();
  scene_float->unload_active_blocks();
  bof_examine_ground_truth(aux_scene, finest_cell_length, ply_paths, obj_name);
  
  pro.set_output_val<boxm_scene_base_sptr>(0, aux_scene);

  
  return true;
}