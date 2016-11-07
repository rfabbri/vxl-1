// This is lemsvxlsrc/contrib/dbrec_lib/dbrec/pro/processes/dbrec_hierarchy_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to create/save/load instances of  part hierarchies.
//
// \author Ozge Can Ozcanli
// \date May 06, 2009
//
// \verbatim
//  Modifications
//   none yet
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <dbrec/dbrec_part_hierarchy_builder.h>
#include <brec/brec_part_hierarchy.h>

//: new classes of dbrec
#include <dbrec/dbrec_part.h>
#include <dbrec/dbrec_image_visitors.h>

//: Constructor
// create hierarchy process may use the builder class or create an empty one to be passed to training processes to learn the hierarchy
bool dbrec_create_hierarchy_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("unsigned");      // detector id for the type of structure to be recognized (if using builder class, otherwise not-used)
  input_types.push_back("vcl_string");      // name of the hierarchy
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("brec_part_hierarchy_sptr");      // output hierarchy
  ok = pro.set_output_types(output_types);
  return ok;
}


bool dbrec_create_hierarchy_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << "brec_create_hierarchy_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  unsigned d_id = pro.get_input<unsigned>(i++);
  vcl_string name = pro.get_input<vcl_string>(i++);

  brec_part_hierarchy_sptr h;
  switch (d_id) {
    case 0: { h = dbrec_part_hierarchy_builder::construct_detector_steeple0(); } break;
    case 1: { h = dbrec_part_hierarchy_builder::construct_detector_steeple1(); } break;
    case 2: { h = dbrec_part_hierarchy_builder::construct_detector_10080_car(); } break;
    default: { vcl_cout << "In dbrec_create_hierarchy_process::execute() -- Unrecognized detector type!!\n"; return false; }
  }
  
  pro.set_output_val<brec_part_hierarchy_sptr>(0, h);

  return true;
}

//: Visualize the new hierarchy class defined in dbrec

//: Constructor
// create hierarchy process may use the builder class or create an empty one to be passed to training processes to learn the hierarchy
bool dbrec_visualize_hierarchy_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");    
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_visualize_hierarchy_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "brec_create_hierarchy_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  vcl_string name = pro.get_input<vcl_string>(i++);
  h->visualize(name);
  return true;
}
//: Constructor
// create hierarchy process may use the builder class or create an empty one to be passed to training processes to learn the hierarchy
bool dbrec_visualize_hierarchy_models_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");     
  input_types.push_back("vcl_string");  // suffix for the file names
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_visualize_hierarchy_models_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dbrec_visualize_hierarchy_models_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  vcl_string name = pro.get_input<vcl_string>(i++);
  for (unsigned i = 0; i < h->class_cnt(); i++) {
    dbrec_part_sptr p = h->root(i);
    p->visualize_models(name);
  }
  return true;
}

//: Constructor
// create hierarchy process may use the builder class or create an empty one to be passed to training processes to learn the hierarchy
bool dbrec_visualize_sampled_class_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");      // hierarchy
  input_types.push_back("int");                   // class id, the index of the root node in the hierarchy
  input_types.push_back("int");                   // type id of the composition, pass the class id -1 if to sample from this composition only
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}


bool dbrec_visualize_sampled_class_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    vcl_cerr << "brec_create_hierarchy_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  int class_id = pro.get_input<int>(i++);
  int composition_type_id = pro.get_input<int>(i++);
  vcl_string name = pro.get_input<vcl_string>(i++);
  if (class_id < 0) {
    dbrec_draw_composition(h, name, composition_type_id, 30.0f, "red");
  } else {
    dbrec_draw_class(h, name, class_id, 30.0f, "red");
  }
  return true;
}

//: write the hierarchy as an xml document, uses and image hierarchy visitor
bool dbrec_write_hierarchy_xml_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr");
  input_types.push_back("vcl_string");  // path of output xml file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_write_hierarchy_xml_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "brec_write_hierarchy_xml_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  vcl_string name = pro.get_input<vcl_string>(i++);

  dbrec_write_xml_visitor wxml;
  for (unsigned i = 0; i < h->class_cnt(); i++)
    h->root(i)->accept(&wxml);
  wxml.write_doc(name);
  return true;
}
//: parse the hierarchy from an xml document 
bool dbrec_parse_hierarchy_xml_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");  // path of output xml file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dbrec_hierarchy_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_parse_hierarchy_xml_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 1) {
    vcl_cerr << "brec_write_hierarchy_xml_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  vcl_string name = pro.get_input<vcl_string>(i++);

  dbrec_parse_hierarchy_xml parser;
  if (!parser.parse(name))
    return false;
  dbrec_hierarchy_sptr parsed_h = parser.get_hierarchy();
  pro.set_output_val<dbrec_hierarchy_sptr>(0, parsed_h);
  return true;
}

//: get the type id of the root node for the given class id
bool dbrec_hierarchy_get_type_id_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dbrec_hierarchy_sptr"); 
  input_types.push_back("int");  // class id
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("int");
  ok = pro.set_output_types(output_types);
  return ok;
}
bool dbrec_hierarchy_get_type_id_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 1) {
    vcl_cerr << "brec_write_hierarchy_xml_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dbrec_hierarchy_sptr h = pro.get_input<dbrec_hierarchy_sptr>(i++);
  int class_id = pro.get_input<int>(i++);
  if (class_id >= (int)h->class_cnt())
    return false;
  dbrec_part_sptr p = h->root(class_id);
  pro.set_output_val<int>(0, p->type());
  return true;
}




