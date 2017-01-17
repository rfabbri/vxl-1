// This is lemsvxlsrc/contrib/brcv/rec/dborl/pro/dborl_eval_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to evaluate recognition algorithms
//
// \author Ozge Can Ozcanli
// \date June 25, 2009
//
// \verbatim
//  Modifications
//   none yet
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <dborl/dborl_exp_stat_sptr.h>
#include <dborl/dborl_evaluation.h>
#include <dborl/dborl_category_info.h>
#include <dborl/algo/dborl_category_info_parser.h>

//: Constructor
//  initialize the experiments stat instance to store the data
bool dborl_exp_stat_initialize_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dborl_exp_stat_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_exp_stat_initialize_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 0) {
    vcl_cerr << "brec_create_hierarchy_process - invalid inputs\n";
    return false;
  }
  dborl_exp_stat_sptr ins = new dborl_exp_stat();
  pro.set_output_val<dborl_exp_stat_sptr>(0, ins);
  return true;
}
//: print self to a file
bool dborl_exp_stat_print_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dborl_exp_stat_sptr");
  input_types.push_back("vcl_string"); // name of the output file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_exp_stat_print_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dborl_exp_stat_print_process - invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i = 0;
  dborl_exp_stat_sptr es = pro.get_input<dborl_exp_stat_sptr>(i++);
  vcl_string out_name = pro.get_input<vcl_string>(i++);

  vcl_ofstream of(out_name.c_str());
  es->print_stats(of);

  return true;
}
//: read self from a file, assumes same format as the print_stats method creates (initialize method needs to be called first to create an instance if needed)
bool dborl_exp_stat_read_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dborl_exp_stat_sptr");
  input_types.push_back("vcl_string"); // name of the input file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_exp_stat_read_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dborl_exp_stat_print_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dborl_exp_stat_sptr es = pro.get_input<dborl_exp_stat_sptr>(i++);
  vcl_string name = pro.get_input<vcl_string>(i++);
  vcl_ifstream ifs(name.c_str());
  if (!ifs) {
    vcl_cout << "cannot open the file: " << name << vcl_endl;
    return false;
  }
  es->read_stats(ifs);
  return true;
}
//: return values
bool dborl_exp_stat_get_values_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dborl_exp_stat_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("float");
  output_types.push_back("float");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_exp_stat_get_values_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 1) {
    vcl_cerr << "dborl_exp_stat_print_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dborl_exp_stat_sptr es = pro.get_input<dborl_exp_stat_sptr>(i++);
  pro.set_output_val<float>(0, es->TPR());
  pro.set_output_val<float>(1, es->FPR());
  return true;
}

bool dborl_exp_stat_eval_classification_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dborl_exp_stat_sptr");
  input_types.push_back("vcl_string"); // gt class
  input_types.push_back("vcl_string"); // outputted class by a classifier
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_exp_stat_eval_classification_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dborl_exp_stat_print_process - invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i = 0;
  dborl_exp_stat_sptr es = pro.get_input<dborl_exp_stat_sptr>(i++);
  vcl_string gt_class = pro.get_input<vcl_string>(i++);
  vcl_string outputted_class = pro.get_input<vcl_string>(i++);
  dborl_evaluation_evaluate_classification(es, gt_class, outputted_class);
  es->increment_positive_cnt(); // every time we see an instance, its a positive of one of our categories
  return true;
}

//: create a category info set and fill it with <category_name, id> pairs read from a simple file
bool dborl_category_info_set_create_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); // file name
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("dborl_category_info_set_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_category_info_set_create_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 1) {
    vcl_cerr << "dborl_category_info_set_create_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  vcl_string file_name = pro.get_input<vcl_string>(i++);
  dborl_category_info_set_sptr cs = new dborl_category_info_set;
  vcl_ifstream is(file_name.c_str());
  if (!is) {
    vcl_cerr << "dborl_category_info_set_create_process - cannot open file: " << file_name << "\n";
    return false;
  }
  cs->read_cats_from_a_simple_file(is);
  is.close();
  pro.set_output_val<dborl_category_info_set_sptr>(0, cs);
  return true;
}
//: write a category info set instance as an xml file
bool dborl_category_info_set_write_xml_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dborl_category_info_set_sptr");
  input_types.push_back("vcl_string"); // output file name
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_category_info_set_write_xml_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dborl_category_info_set_write_xml_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dborl_category_info_set_sptr cis = pro.get_input<dborl_category_info_set_sptr>(i++);
  vcl_string file_name = pro.get_input<vcl_string>(i++);
  
  vcl_ofstream ofs(file_name.c_str());
  write_categories_xml(cis->categories(), ofs);
  ofs.close();
  return true;
}
//: access values of a category info set
bool dborl_category_info_set_get_id_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dborl_category_info_set_sptr");
  input_types.push_back("vcl_string"); // category name
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  vcl_vector<vcl_string> output_types;
  output_types.push_back("int");  // return category id
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_category_info_set_get_id_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    vcl_cerr << "dborl_category_info_set_create_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  dborl_category_info_set_sptr cs = pro.get_input<dborl_category_info_set_sptr>(i++);
  vcl_string cat_name = pro.get_input<vcl_string>(i++);
  dborl_category_info_sptr cinfo = cs->find_category(cat_name);
  if (!cinfo) {
    vcl_cout << "In dborl_category_info_set_get_id_process() -- no info is found in the set for the category: " << cat_name << "!\n";
    return false;
  }
  pro.set_output_val<int>(0, cinfo->id_);
  return true;
}


//: evaluate the classification by reading a f
bool dborl_exp_stat_eval_classification_using_id_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("dborl_exp_stat_sptr");
  input_types.push_back("dborl_category_info_set_sptr");  // pass a category info set instance for this process to match the name to the id
  input_types.push_back("vcl_string"); // gt class
  input_types.push_back("int"); // class id outputted by a classifier
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_exp_stat_eval_classification_using_id_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    vcl_cerr << "dborl_exp_stat_eval_classification_using_ids_process - invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i = 0;
  dborl_exp_stat_sptr es = pro.get_input<dborl_exp_stat_sptr>(i++);
  dborl_category_info_set_sptr cis = pro.get_input<dborl_category_info_set_sptr>(i++);
  vcl_string gt_class = pro.get_input<vcl_string>(i++);
  int outputted_class = pro.get_input<int>(i++);
  dborl_evaluation_evaluate_classification(es, cis, gt_class, outputted_class);
  es->increment_positive_cnt(); // every time we see an instance, its a positive of one of our categories
  return true;
}
