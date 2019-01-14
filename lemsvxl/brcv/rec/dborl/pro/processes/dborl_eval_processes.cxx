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

#include <buld/buld_exp_stat_sptr.h>
#include <borld/borld_evaluation.h>
#include <borld/borld_category_info.h>
#include <dborl/algo/dborl_category_info_parser.h>

//: Constructor
//  initialize the experiments stat instance to store the data
bool buld_exp_stat_initialize_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.push_back("buld_exp_stat_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool buld_exp_stat_initialize_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 0) {
    std::cerr << "brec_create_hierarchy_process - invalid inputs\n";
    return false;
  }
  buld_exp_stat_sptr ins = new buld_exp_stat();
  pro.set_output_val<buld_exp_stat_sptr>(0, ins);
  return true;
}
//: print self to a file
bool buld_exp_stat_print_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.push_back("buld_exp_stat_sptr");
  input_types.push_back("vcl_string"); // name of the output file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool buld_exp_stat_print_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << "buld_exp_stat_print_process - invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i = 0;
  buld_exp_stat_sptr es = pro.get_input<buld_exp_stat_sptr>(i++);
  std::string out_name = pro.get_input<std::string>(i++);

  std::ofstream of(out_name.c_str());
  es->print_stats(of);

  return true;
}
//: read self from a file, assumes same format as the print_stats method creates (initialize method needs to be called first to create an instance if needed)
bool buld_exp_stat_read_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.push_back("buld_exp_stat_sptr");
  input_types.push_back("vcl_string"); // name of the input file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool buld_exp_stat_read_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    std::cerr << "buld_exp_stat_print_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  buld_exp_stat_sptr es = pro.get_input<buld_exp_stat_sptr>(i++);
  std::string name = pro.get_input<std::string>(i++);
  std::ifstream ifs(name.c_str());
  if (!ifs) {
    std::cout << "cannot open the file: " << name << std::endl;
    return false;
  }
  es->read_stats(ifs);
  return true;
}
//: return values
bool buld_exp_stat_get_values_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.push_back("buld_exp_stat_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  std::vector<std::string> output_types;
  output_types.push_back("float");
  output_types.push_back("float");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool buld_exp_stat_get_values_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 1) {
    std::cerr << "buld_exp_stat_print_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  buld_exp_stat_sptr es = pro.get_input<buld_exp_stat_sptr>(i++);
  pro.set_output_val<float>(0, es->TPR());
  pro.set_output_val<float>(1, es->FPR());
  return true;
}

bool buld_exp_stat_eval_classification_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.push_back("buld_exp_stat_sptr");
  input_types.push_back("vcl_string"); // gt class
  input_types.push_back("vcl_string"); // outputted class by a classifier
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool buld_exp_stat_eval_classification_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << "buld_exp_stat_print_process - invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i = 0;
  buld_exp_stat_sptr es = pro.get_input<buld_exp_stat_sptr>(i++);
  std::string gt_class = pro.get_input<std::string>(i++);
  std::string outputted_class = pro.get_input<std::string>(i++);
  borld_evaluation_evaluate_classification(es, gt_class, outputted_class);
  es->increment_positive_cnt(); // every time we see an instance, its a positive of one of our categories
  return true;
}

//: create a category info set and fill it with <category_name, id> pairs read from a simple file
bool dborl_category_info_set_create_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.push_back("vcl_string"); // file name
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  std::vector<std::string> output_types;
  output_types.push_back("borld_category_info_set_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_category_info_set_create_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 1) {
    std::cerr << "dborl_category_info_set_create_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  std::string file_name = pro.get_input<std::string>(i++);
  borld_category_info_set_sptr cs = new borld_category_info_set;
  std::ifstream is(file_name.c_str());
  if (!is) {
    std::cerr << "dborl_category_info_set_create_process - cannot open file: " << file_name << "\n";
    return false;
  }
  cs->read_cats_from_a_simple_file(is);
  is.close();
  pro.set_output_val<borld_category_info_set_sptr>(0, cs);
  return true;
}
//: write a category info set instance as an xml file
bool dborl_category_info_set_write_xml_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.push_back("borld_category_info_set_sptr");
  input_types.push_back("vcl_string"); // output file name
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_category_info_set_write_xml_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    std::cerr << "dborl_category_info_set_write_xml_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  borld_category_info_set_sptr cis = pro.get_input<borld_category_info_set_sptr>(i++);
  std::string file_name = pro.get_input<std::string>(i++);
  
  std::ofstream ofs(file_name.c_str());
  write_categories_xml(cis->categories(), ofs);
  ofs.close();
  return true;
}
//: access values of a category info set
bool dborl_category_info_set_get_id_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.push_back("borld_category_info_set_sptr");
  input_types.push_back("vcl_string"); // category name
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  std::vector<std::string> output_types;
  output_types.push_back("int");  // return category id
  ok = pro.set_output_types(output_types);
  return ok;
}

bool dborl_category_info_set_get_id_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 2) {
    std::cerr << "dborl_category_info_set_create_process - invalid inputs\n";
    return false;
  }
  unsigned i = 0;
  borld_category_info_set_sptr cs = pro.get_input<borld_category_info_set_sptr>(i++);
  std::string cat_name = pro.get_input<std::string>(i++);
  borld_category_info_sptr cinfo = cs->find_category(cat_name);
  if (!cinfo) {
    std::cout << "In dborl_category_info_set_get_id_process() -- no info is found in the set for the category: " << cat_name << "!\n";
    return false;
  }
  pro.set_output_val<int>(0, cinfo->id_);
  return true;
}


//: evaluate the classification by reading a f
bool buld_exp_stat_eval_classification_using_id_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.push_back("buld_exp_stat_sptr");
  input_types.push_back("borld_category_info_set_sptr");  // pass a category info set instance for this process to match the name to the id
  input_types.push_back("vcl_string"); // gt class
  input_types.push_back("int"); // class id outputted by a classifier
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool buld_exp_stat_eval_classification_using_id_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cerr << "buld_exp_stat_eval_classification_using_ids_process - invalid inputs\n";
    return false;
  }
  // get the inputs
  unsigned i = 0;
  buld_exp_stat_sptr es = pro.get_input<buld_exp_stat_sptr>(i++);
  borld_category_info_set_sptr cis = pro.get_input<borld_category_info_set_sptr>(i++);
  std::string gt_class = pro.get_input<std::string>(i++);
  int outputted_class = pro.get_input<int>(i++);
  borld_evaluation_evaluate_classification(es, cis, gt_class, outputted_class);
  es->increment_positive_cnt(); // every time we see an instance, its a positive of one of our categories
  return true;
}
