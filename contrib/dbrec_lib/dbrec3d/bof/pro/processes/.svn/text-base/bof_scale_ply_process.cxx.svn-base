//:
// \brief
// \file
// \author Isabel Restrepo
// \date 18-May-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_scene_categories.h>

//:global variables
namespace bof_scale_ply_process_globals 
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_scale_ply_process_cons(bprb_func_process& pro)
{
  using namespace bof_scale_ply_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "double";

  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_scale_ply_process(bprb_func_process& pro)
{
  using namespace bof_scale_ply_process_globals;
  
  //get inputs
  vcl_string ply_file_in = pro.get_input<vcl_string>(0);
  vcl_string ply_file_out = pro.get_input<vcl_string>(1);
  double scale = pro.get_input<double>(2);

  bof_scene_categories::scale_ply(ply_file_in, ply_file_out, scale);
  
  return true;
}