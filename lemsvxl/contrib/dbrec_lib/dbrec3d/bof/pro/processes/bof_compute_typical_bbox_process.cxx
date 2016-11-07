//:
// \brief A process to compute the typical bbox of a category
// \file
// \author Isabel Restrepo
// \date 21-Sep-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_scene_categories.h>


//:global variables
namespace bof_compute_typical_bbox_process_globals 
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_compute_typical_bbox_process_cons(bprb_func_process& pro)
{
  using namespace bof_compute_typical_bbox_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // bof_path, where bof_info.xml and bof_category_info.xml are
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_compute_typical_bbox_process(bprb_func_process& pro)
{
  using namespace bof_compute_typical_bbox_process_globals;
  
  //get inputs
  vcl_string bof_path = pro.get_input<vcl_string>(0);
  
  bof_scene_categories categories(bof_path);
  categories.bounding_box_statictics();
    
  return true;
}