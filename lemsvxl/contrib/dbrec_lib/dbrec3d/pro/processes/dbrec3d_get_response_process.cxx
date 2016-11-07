//:
// \brief A short process to extract the response from a context of part instances
// \file
// \author Isabel Restrepo
// \date 22-Jun-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>


#include <dbrec3d/dbrec3d_context_manager.h>

//:global variables
namespace dbrec3d_get_response_process_globals 
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}


//:sets input and output types
bool dbrec3d_get_response_process_cons(bprb_func_process& pro)
{
  using namespace dbrec3d_get_response_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "int"; //context id
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr" ;
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool dbrec3d_get_response_process(bprb_func_process& pro)
{
  using namespace dbrec3d_get_response_process_globals;
  
  //get inputs
  int context_id = pro.get_input<int>(0);
    

  //store output
  typedef boct_tree<short, float> float_tree_type;
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef boct_tree<short, gauss_type > gauss_tree_type;
  
  pro.set_output_val<boxm_scene_base_sptr>(0, CONTEXT_MANAGER->response_scene<dbrec3d_part_instance>(context_id));
  
  return true;
}