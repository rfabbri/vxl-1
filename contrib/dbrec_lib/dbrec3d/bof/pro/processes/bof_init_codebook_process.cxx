//:
// \brief A process to intialize the codebook and category' scenes needed for learning class distributions
// \file
// \author Isabel Restrepo
// \date 9-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_codebook.h>

//:global variables
namespace bof_init_codebook_process_globals 
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bof_init_codebook_process_cons(bprb_func_process& pro)
{
  using namespace bof_init_codebook_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  //path to the k-means file to be used as cosebook
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bof_codebook_sptr";
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_init_codebook_process(bprb_func_process& pro)
{
  using namespace bof_init_codebook_process_globals;
  
  //get inputs
  vcl_string mean_file = pro.get_input<vcl_string>(0);
   
  pro.set_output_val<bof_codebook_sptr>(0, new bof_codebook(mean_file));

  
  return true;
}