//:
// \brief
// \file
// \author Isabel Restrepo
// \date 27-Sep-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_scene_categories.h>

//:global variables
namespace bof_load_category_info_process_globals 
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bof_load_category_info_process_cons(bprb_func_process& pro)
{
  using namespace bof_load_category_info_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //bof_dir
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bof_scene_categories_sptr";
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_load_category_info_process(bprb_func_process& pro)
{
  using namespace bof_load_category_info_process_globals;
  
  //get inputs
  vcl_string bof_dir = pro.get_input<vcl_string>(0);
  
  bof_scene_categories_sptr category_info = new bof_scene_categories(bof_dir);
   
  //store output
  pro.set_output_val<bof_scene_categories_sptr>(0, category_info);
  
  return true;
}