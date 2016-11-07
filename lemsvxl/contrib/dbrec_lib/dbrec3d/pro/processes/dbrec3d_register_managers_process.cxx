//:
// \brief A process to register dbrec3d_parts_manager and dbrec3d_context_manager
// \file
// \author Isabel Restrepo
// \date 21-Jun-2010


#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include<dbrec3d/dbrec3d_parts_manager.h>
#include<dbrec3d/dbrec3d_context_manager.h>

//:global variables
namespace dbrec3d_register_managers_process_globals 
{
  const unsigned n_inputs_ = 0;
  const unsigned n_outputs_ = 2;
}


//:sets input and output types
bool dbrec3d_register_managers_process_cons(bprb_func_process& pro)
{
  using namespace dbrec3d_register_managers_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
 
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "dbrec3d_parts_manager_sptr";
  output_types_[1] = "dbrec3d_context_manager_sptr";
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool dbrec3d_register_managers_process(bprb_func_process& pro)
{
  using namespace dbrec3d_register_managers_process_globals;

  //store output
  pro.set_output_val<dbrec3d_parts_manager_sptr>(0, PARTS_MANAGER);
  pro.set_output_val<dbrec3d_context_manager_sptr>(1, CONTEXT_MANAGER);
  
  return true;
}