//:
// \brief A process to save a context as vrml. The user can specify a region of iterstet to restict the output
// \file
// \author Gamze Tunali
// \date 12-Aug-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/dbrec3d_context_manager.h>
#include <dbrec3d/dbrec3d_vrml_visitor.h>
#include <dbrec3d/dbrec3d_octree_context.h>
#include <dbrec3d/dbrec3d_part_instance.h>

//:global variables
namespace dbrec3d_save_vrml_process_globals 
{
  const unsigned n_inputs_= 8;
  const unsigned n_outputs_= 0;
}


//:sets input and output types
bool dbrec3d_save_vrml_process_cons(bprb_func_process& pro)
{
  using namespace dbrec3d_save_vrml_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "int";        //context_id
  input_types_[i++] = "vcl_string"; //path for the vrml file to be written
  
  input_types_[i++] = "double";   // roi min point
  input_types_[i++] = "double";
  input_types_[i++] = "double";
  
  input_types_[i++] = "double";   // roi max point
  input_types_[i++] = "double";
  input_types_[i++] = "double";
  
    
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool dbrec3d_save_vrml_process(bprb_func_process& pro)
{
  using namespace dbrec3d_save_vrml_process_globals;
  
  //get inputs
  int context_id = pro.get_input<int>(0);
  vcl_string fname = pro.get_input<vcl_string>(1);
  double min_x =pro.get_input<double>(2);
  double min_y =pro.get_input<double>(3);
  double min_z =pro.get_input<double>(4);
  double max_x =pro.get_input<double>(5);
  double max_y =pro.get_input<double>(6);
  double max_z =pro.get_input<double>(7);

  
  
  dbrec3d_context_sptr context_ptr = CONTEXT_MANAGER->get_context(context_id);
  double cell_length = context_ptr->finest_cell_length();
  vcl_ofstream os(fname.c_str());
  dbrec3d_vrml_visitor vrml_vis(os, cell_length);
  
  
  vgl_box_3d<double> roi(min_x, min_y, min_z, max_x, max_y, max_z);
   

  // cast to octree context
  dbrec3d_octree_context<dbrec3d_part_instance>* context = dynamic_cast<dbrec3d_octree_context<dbrec3d_part_instance>*> (context_ptr.as_pointer());
  if (context) {
    if(roi.is_empty())
      vrml_vis.write_context(context);
    else 
      vrml_vis.write_context(context, roi);
    
    os.close();
  } 
  else {
    dbrec3d_octree_context<dbrec3d_pair_composite_instance>* context = dynamic_cast<dbrec3d_octree_context<dbrec3d_pair_composite_instance>*> (context_ptr.as_pointer());
    if(roi.is_empty())
      vrml_vis.write_context(context);
    else 
      vrml_vis.write_context(context, roi);
    
    os.close();
  }
  return true;
}