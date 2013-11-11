//:
// \brief A process to init bof info file from global_pca_info
// \file
// \author Isabel Restrepo
// \date 2-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/bof/bof_info.h>

//:global variables
namespace bof_init_info_file_process_globals 
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_init_info_file_process_cons(bprb_func_process& pro)
{
  using namespace bof_init_info_file_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // path to pca_global_info/taylor_global_info file
  input_types_[1] = "vcl_string"; // bof_info file

  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_init_info_file_process(bprb_func_process& pro)
{
  using namespace bof_init_info_file_process_globals;
  
  //get inputs
  vcl_string info_dir = pro.get_input<vcl_string>(0);
  vcl_string bof_file = pro.get_input<vcl_string>(1);

  bof_info(info_dir, bof_file);  
  
  return true;
}