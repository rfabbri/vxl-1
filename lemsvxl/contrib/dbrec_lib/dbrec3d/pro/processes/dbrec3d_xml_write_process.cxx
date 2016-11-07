//:
// \brief A process to save the hierarchy and contexts in an xml file
// \file
// \author Isabel Restrepo
// \date 16-Sep-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/dbrec3d_io_visitors.h>

//:global variables
namespace dbrec3d_xml_write_process_globals 
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool dbrec3d_xml_write_process_cons(bprb_func_process& pro)
{
  using namespace dbrec3d_xml_write_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //path to parts' file
  input_types_[1] = "vcl_string"; //path to contexts' file
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool dbrec3d_xml_write_process(bprb_func_process& pro)
{
  using namespace dbrec3d_xml_write_process_globals;
  
  //get inputs
  vcl_string parts_fname = pro.get_input<vcl_string>(0);
  vcl_string contexts_fname = pro.get_input<vcl_string>(1);
  
  dbrec3d_xml_write_parts_and_contexts(parts_fname, contexts_fname);
 
  return true;
}