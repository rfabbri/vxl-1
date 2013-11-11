//:
// \brief A process to save to drishti format the category scene associated with the given id
// \file
// \author Isabel Restrepo
// \date 11-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <boxm/algo/boxm_save_scene_raw_general.h>

#include <bof/bof_info.h>

//:global variables
namespace bof_save_category_scene_raw_process_globals 
{
  const unsigned n_inputs_ =2;
  const unsigned n_outputs_ =0;
}


//:sets input and output types
bool bof_save_category_scene_raw_process_cons(bprb_func_process& pro)
{
  using namespace bof_save_category_scene_raw_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //bof_dir
  input_types_[1] = "int"; //scene id
  
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_save_category_scene_raw_process(bprb_func_process& pro)
{
  using namespace bof_save_category_scene_raw_process_globals;
  
  //get inputs
  vcl_string bof_dir = pro.get_input<vcl_string>(0);
  int scene_id = pro.get_input<int>(1);
  
  bof_info info(bof_dir);
  
  boxm_scene_base_sptr scene_base = info.load_category_scene(scene_id);
  typedef boct_tree<short, char> char_tree_type;
  boxm_scene<char_tree_type>* class_id_scene = dynamic_cast<boxm_scene<char_tree_type>*> (scene_base.as_pointer());
  vcl_stringstream aux_scene_ss;
  aux_scene_ss << info.aux_dir(scene_id) << "/category_scene_" << scene_id << ".raw";
  boxm_save_scene_raw_general(*class_id_scene, aux_scene_ss.str(), 0);

  return true;
}