//:
// \brief A process to suppress parts with non maxima posterior probability within a part
// \file
// \author Isabel Restrepo
// \date 12-Aug-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/dbrec3d_context_manager.h>

//:global variables
namespace dbrec3d_non_max_suppression_process_globals 
{
  const unsigned n_inputs_= 1;
  const unsigned n_outputs_= 0;
}


//:sets input and output types
bool dbrec3d_non_max_suppression_process_cons(bprb_func_process& pro)
{
  using namespace dbrec3d_non_max_suppression_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "int"; //context_id
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool dbrec3d_non_max_suppression_process(bprb_func_process& pro)
{
  using namespace dbrec3d_non_max_suppression_process_globals;
  
  //get inputs
  int context_id = pro.get_input<int>(0);
  
  dbrec3d_context_sptr context = CONTEXT_MANAGER->get_context(context_id);
 
  if(context)
    context->local_non_maxima_suppression();
 
  //store output
  //pro.set_output_val<boxm_scene_base_sptr>(0, output);
  
  return true;
}