//:
// \file
// \brief
// \author Ozge C Ozcanli (Brown)
// \date   December 20, 2007
//
//

#include "dborl_algo_params.h"
#include <vcl_fstream.h>

//: makes no check as to whether the same parameter is being added or not
void 
dborl_algo_params::add_params(dborl_algo_params_base& other)
{
  for (unsigned i = 0; i < other.param_list_.size(); i++) {
    param_list_.push_back(other.param_list_[i]);
    param_list_[param_list_.size()-1]->set_param_group(other.algo_name_ + "_" + other.param_list_[i]->param_group());
  }
}

//: set the parameters with the same group names and names in the other's list with the values of the parameters in this list
void 
dborl_algo_params::set_params(dborl_algo_params_base& other)
{
  //: make one pass and find all the group names 
  vcl_map<vcl_string, int> groups;
  for (unsigned i = 0; i < param_list_.size(); i++) {
    vcl_map<vcl_string, int>::iterator iter = groups.find(param_list_[i]->param_group());
    if (iter == groups.end()) {  // not added yet
      groups[param_list_[i]->param_group()] = 1;
    } else {
      iter->second = iter->second + 1;  // increment the count
    }
  }

  //: now set the params
  for (vcl_map<vcl_string, int>::const_iterator iter = groups.begin(); iter != groups.end(); iter++) {
    
    vcl_string group_name = iter->first;
    
    //: go over the param list to get each param from this group
    for (unsigned i = 0; i < param_list_.size(); i++) {
      if (param_list_[i]->param_group().compare(iter->first) != 0)
        continue;

      //: go over other's param list to get each param from this group
      for (unsigned j = 0; j < other.param_list_.size(); j++) {
        if (other.param_list_[j]->param_group().compare(iter->first) != 0)
          continue;
        if (other.param_list_[j]->param_name().compare(param_list_[i]->param_name()) == 0)
          other.param_list_[j]->parse_value_from_str(param_list_[i]->value_str());
      }
    }
  }

}
  

dborl_algo_params::dborl_algo_params(vcl_string algo_name) : dborl_algo_params_base(algo_name) {
  
  input_param_filename_ = "";  
  
  //: parameters in the web_directive group, used in creation of params.xml file
  print_params_file.set_values(param_list_, "web_directive", 
                               "print_params_file", 
                               "print the xml parameter file", 
                               "params.xml", 
                               "params.xml", 
                               -1,  // system_info_index is -1 to indicate that it is NON APPLICABLE
                               dborl_parameter_system_info::PARAM_BLOCK);
  

  print_params_only.set_values(param_list_, "web_directive", "print_params_only", "print the xml parameter file only", false, false);
  status_file.set_values(param_list_, "web_directive", "status_block", "Status file path", "status.xml", "status.xml", -1, dborl_parameter_system_info::STATUS_BLOCK, "");
  perf_file.set_values(param_list_, "web_directive", "perf_block", "Performance file path", "perf.xml", "perf.xml", -1, dborl_parameter_system_info::OUTPUT_PERF, "");
  categorization_file.set_values(param_list_, "web_directive", "categorization_file", "Categorization file path", "categorization.xml", "categorization.xml", -1, dborl_parameter_system_info::OUTPUT_CATEGORIZATION, "");
  evaluation_file.set_values(param_list_, "web_directive", "evaluation_file", "Evaluation file path", "evaluation.xml", "evaluation.xml", -1, dborl_parameter_system_info::OUTPUT_EVALUATION, "");

  //: parameters in the basic group of the status block, used in creation of status.xml file
  //  the parameters withing the status group should be treated specially while parsing input.xml etc
  //  the basic status parameters are not added to the param list, but if the user defines any other
  //  status parameter to be written to the optional section of status.xml, he/she should add them to the param_list_
  //  see the dborl_example_algo in /dborl/algo/examples/
  percent_completed.set_values("status", "percent_completed", "the amount of processing completed so far", 0, 0);
  exit_code.set_values("status", "exit_code", "the exit code of the algorithm", 0, 0);
  exit_code.set_long_description("usually set to 0 on normal completion of the algorithm");
  exit_message.set_values("status", "exit_message", "the message to be displayed on return", "", "");
  exit_message.set_long_description("if exit code is not 0, exit_message should be set accordingly");

  //: the following parameter is not added to the param_list either, since its for internal use
  exit_with_no_processing.set_values("internal_use", "exit_with_no_processing", "the flag to signal exit after parsing command line args", false, false);
} 

void dborl_algo_params::print_params_open_root(vcl_ostream& of)
{
  of << "<parameter_root>\n";
  of << "\t<params app=\"" << algo_name_ << "\" >\n";
}
void dborl_algo_params::print_params_close_root(vcl_ostream& of)
{
  of << "\t</params>\n";
  of << "</parameter_root>\n";
} 

bool dborl_algo_params::print_params_xml(vcl_string filename)
{
  vcl_ofstream of(filename.c_str());
  if (!of){
    vcl_cout<<"In dborl_algo_params::print_params_xml : Unable to Open " << filename << vcl_endl;
    return false;
  }

  print_params_open_root(of);

  //: print the parameters in the list
  for (unsigned i = 0; i < param_list_.size(); i++) {
    if (param_list_[i]->param_group().compare("status") != 0 && param_list_[i]->param_group().compare("performance") != 0) {
      of << param_list_[i]->get_params_file_tag();
    }
  }

  print_params_close_root(of);

  of.close();
  return true;
}

void dborl_algo_params::print_status_open(vcl_ostream& of) {
  of << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  of << "<status>\n";
}
void dborl_algo_params::print_status_basic(vcl_ostream& of) {
  of << "\t<basic>\n";
  of << "\t\t<info name = \"percent_completed\" value=\"" << percent_completed() << "\" type=\"float\"/>\n";
  of << "\t\t<info name = \"exit_code\" value=\"" << exit_code() << "\" type=\"integer\"/>\n";
  of << "\t\t<info name = \"exit_message\" value=\"" << exit_message() << "\" type=\"string\"/>\n";
  of << "\t</basic>\n";
}
void dborl_algo_params::print_status_close(vcl_ostream& of) {
  of << "</status>\n";
}
  
//: if any additional params are defined to be written to optional tag of the status.xml file
void dborl_algo_params::print_status_optionals(vcl_ostream& of) {
  of << "\t<optionals>\n";

  for (unsigned i = 0; i < param_list_.size(); i++) {
    if (param_list_[i]->param_group().compare("status") == 0) {
      of << "\t\t<info name = \"" << param_list_[i]->param_name() << "\" value\"" << param_list_[i]->value_str() << "\" type=\"" << param_list_[i]->type() << "\"/>\n";
    }
  }
  of << "\t</optionals>\n";
}
  
bool dborl_algo_params::print_status_xml() {
  vcl_ofstream of(status_file().c_str());
  if (!of){
    vcl_cout<<"In dborl_algo_params::print_status_xml : Unable to Open " << status_file() << vcl_endl;
    return false;
  }

  print_status_open(of);
  print_status_basic(of);
  print_status_optionals(of);
  print_status_close(of);
  of.close();
  return true;
}

//: parse all the group names
bool dborl_algo_params::parse_from_data(bxml_data_sptr root)
{
  bxml_element algo_query(algo_name_);
  bxml_data_sptr algo_root = bxml_find_by_name(root, algo_query);
  
  if (!algo_root) {
    vcl_cout << "dborl_detect_shape_params::parse_from_data() - could not find the main algo node with name: " << algo_name_ << "\n";
    return false;
  }

  //: make one pass and find all the group names to be parsed
  vcl_map<vcl_string, int> groups;
  for (unsigned i = 0; i < param_list_.size(); i++) {
    vcl_map<vcl_string, int>::iterator iter = groups.find(param_list_[i]->param_group());
    if (iter == groups.end()) {  // not added yet
      groups[param_list_[i]->param_group()] = 1;
    } else {
      iter->second = iter->second + 1;  // increment the count
    }
  }

  //: now parse each group
  vcl_map<vcl_string, int> parsed_groups;
  for (vcl_map<vcl_string, int>::const_iterator iter = groups.begin(); iter != groups.end(); iter++) {
    
    //: skip the status group since they have nothing to do with input.xml
    if (iter->first.compare("status") == 0)
      continue;

    //: skip the performance group since they have nothing to do with input.xml
    if (iter->first.compare("performance") == 0)
      continue;
    
    bxml_element query(iter->first);
    bxml_data_sptr result = bxml_find_by_name(algo_root, query);
    if (!result)
      continue;
    
    bxml_element * data = static_cast<bxml_element*>(result.ptr());
    if (!data)
      return false;

    //: go over the param list to parse each param from this group
    vcl_string val;
    for (unsigned i = 0; i < param_list_.size(); i++) {
      if (param_list_[i]->param_group().compare(iter->first) != 0)
        continue;
      data->get_attribute(param_list_[i]->param_name(), val);
      param_list_[i]->parse_value_from_str(val);

      vcl_map<vcl_string, int>::iterator piter = parsed_groups.find(param_list_[i]->param_group());
      if (piter == parsed_groups.end())
        parsed_groups[iter->first] = 1;
      else
        piter->second = piter->second + 1;
    }
  }

  //: check if all the params from all the groups are found, if not return false
  bool all_found = true;
  for (vcl_map<vcl_string, int>::const_iterator iter = groups.begin(); iter != groups.end(); iter++) {
    if (iter->first.compare("status") == 0)
      continue;

    vcl_map<vcl_string, int>::iterator piter = parsed_groups.find(iter->first);
    
    if (piter == parsed_groups.end()) {
      vcl_cout << "dborl_algo_params::parse_from_data() - could not find the node " << iter->first << "\n";
      all_found = false;
    }
    if (piter->second != iter->second) {
      vcl_cout << "dborl_algo_params::parse_from_data() - could not find all params from the node " << iter->first << "\n";
      all_found = false;
    }
  }

  return all_found;
}
  
//: algo_name is the name of the root element in the document
bxml_element *dborl_algo_params::create_default_document_data()
{
  bxml_element * root = new bxml_element(algo_name_);
  
  //: find each group name to search under the root
  //  make one pass and find all the group names to be parsed
  vcl_map<vcl_string, int> groups;
  for (unsigned i = 0; i < param_list_.size(); i++) {
    vcl_map<vcl_string, int>::iterator iter = groups.find(param_list_[i]->param_group());
    if (iter == groups.end()) {  // not added yet
      groups[param_list_[i]->param_group()] = 1;
    } else {
      iter->second = iter->second + 1;  // increment the count
    }
  }

  //: add each group (except status) and their parameters with default values
  for (vcl_map<vcl_string, int>::const_iterator iter = groups.begin(); iter != groups.end(); iter++) {
    
    //: skip the status group since they have nothing to do with input.xml
    if (iter->first.compare("status") == 0)
      continue;

    bxml_element * data = new bxml_element(iter->first);
    root->append_data(data);
    root->append_text("\n");
    //: go over the param list to add each param from this group
    vcl_string val;
    for (unsigned i = 0; i < param_list_.size(); i++) {
      if (param_list_[i]->param_group().compare(iter->first) != 0)
        continue;
      data->set_attribute(param_list_[i]->param_name(), param_list_[i]->default_str()); // write the default value of the parameter
    }
    data->append_text("\n"); 
  }

  return root;
}

bxml_element *dborl_algo_params::create_document_data()
{
  bxml_element * root = new bxml_element(algo_name_);
  
  //: find each group name to search under the root
  //  make one pass and find all the group names to be parsed
  vcl_map<vcl_string, int> groups;
  for (unsigned i = 0; i < param_list_.size(); i++) {
    vcl_map<vcl_string, int>::iterator iter = groups.find(param_list_[i]->param_group());
    if (iter == groups.end()) {  // not added yet
      groups[param_list_[i]->param_group()] = 1;
    } else {
      iter->second = iter->second + 1;  // increment the count
    }
  }

  //: add each group (except status) and their parameters with their current values
  for (vcl_map<vcl_string, int>::const_iterator iter = groups.begin(); iter != groups.end(); iter++) {
    
    //: skip the status group since they have nothing to do with input.xml
    if (iter->first.compare("status") == 0)
      continue;

    bxml_element * data = new bxml_element(iter->first);
    root->append_data(data);
    root->append_text("\n");
    //: go over the param list to add each param from this group
    vcl_string val;
    for (unsigned i = 0; i < param_list_.size(); i++) {
      if (param_list_[i]->param_group().compare(iter->first) != 0)
        continue;
      data->set_attribute(param_list_[i]->param_name(), param_list_[i]->value_str());  // write the current value of the parameter
    }
    data->append_text("\n"); 
  }

  return root;
}

//: input_param_filename_ should be set prior to calling this method
//  parse_command_line_args() sets input_param_filename_ if called prior to this method
bool dborl_algo_params::parse_input_xml()
{
  if (input_param_filename_.compare("") == 0) {
    vcl_cout << "dborl_algo_params::parse_input_xml() -- input_param_filename_ has not been set!\n";
    return false;
  }

  bxml_document param_doc = bxml_read(input_param_filename_);
  if (!param_doc.root_element())
    return false;
  
  if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
    vcl_cout << "params root is not ELEMENT\n";
    return false;
  }

  return parse_from_data(param_doc.root_element());
}

//: print a parameter file with the current values of the parameters
void dborl_algo_params::print_input_xml(vcl_string param_file)
{
  bxml_document doc;
  bxml_element * root = create_document_data();
  doc.set_root_element(root);
  bxml_write(param_file, doc);
}

//: print a parameter file with the default values of the parameters
void dborl_algo_params::print_default_input_xml(vcl_string param_file)
{
  bxml_document doc;
  bxml_element * root = create_default_document_data();
  doc.set_root_element(root);
  bxml_write(param_file, doc);
}


//: the command line args can be passed directly to the params class 
//  the following method extracts the name of the parameter file
//  also supports printing an input parameter file with the default values, 
bool dborl_algo_params::parse_command_line_args(int argc, char* argv[])
{
  vcl_vector<vcl_string> args;
  for (int i = 0; i < argc; i++) {
    vcl_string argument = argv[i];
    args.push_back(argument);
  }
  return parse_command_line_args(args);
}
//: for algos using dborl_cluster for parallel processing, when command line parameters are broadcasted to all processes
bool dborl_algo_params::parse_command_line_args(vcl_vector<vcl_string>& argv)
{
  if (!argv.size() || argv.size() == 1) {
    vcl_cout << "usage: <algo exe name> [-x input.xml] [-print-def-xml] [-usage] [-help] [-?]\n";
    exit_with_no_processing = true;
    return false;
  } else
    vcl_cout << " argv size: " << argv.size() << vcl_endl;
  
  for (unsigned i = 0; i < argv.size(); i++) {
    vcl_string arg = argv[i];
    if (arg.compare("-x") == 0) {
      input_param_filename_ = argv[++i];
    } else if (arg.compare("-usage") == 0 || arg.compare("-help") == 0 || arg.compare("-?") == 0) {
      vcl_cout << "usage: <algo exe name> [-x input.xml] [-print-def-xml] [-usage] [-help] [-?]\n";
      exit_with_no_processing = true;
      return true;
    } else if (arg.compare("-print-def-xml") == 0) {
      print_default_input_xml(vcl_string("input_defaults.xml"));
      exit_with_no_processing = true;
      return true;
    }
  }

  if (input_param_filename_.compare("") == 0) {
    vcl_cout << "usage: <algo exe name> [-x input.xml] [-print-def-xml] [-usage] [-help] [-?]\n";
    return false;
  }

  return true;   // returns true if parameter input files name is correctly identified
}

//: return false if an entry with key "name" does not exist
bool dborl_algo_params::perf_map_update(vcl_string name, double x, double y) 
{  
  vcl_map<vcl_string, vgl_point_2d<double> >::iterator iter = perf_param_map_.find(name);
  if (iter == perf_param_map_.end()) // could not find name
    return false;                                                             
  else                               // found name
    iter->second = vgl_point_2d<double>(x, y); 
  return true; 
}
//: return false if an entry with key "name" already exists
bool dborl_algo_params::perf_map_insert(vcl_string name, double x, double y) 
{ 
  vcl_map<vcl_string, vgl_point_2d<double> >::iterator iter = perf_param_map_.find(name);
  if (iter != perf_param_map_.end()) // found name
    return false;    
  else                               // could not find name
    perf_param_map_[name] = vgl_point_2d<double>(x, y); // inserts name for the first time
  return true; 
}

//: print perf.xml with the current values in the perf_param_map
//  not interested in parsing these files so just treat as a text file (i.e. not using bxml classes)
void dborl_algo_params::print_perf_xml(vcl_string description)
{
  vcl_ofstream of(perf_file().c_str());
  if (!of) {
    vcl_cout << "dborl_algo_params::print_perf_xml() could not open the file: " << perf_file() << "\n";
    return;
  }
  
  of << "<type name = \"performance\">\n";
  of << "<plot type = \"" << dborl_evaluation_plot_type::get_plot_type_str(plot_type_) << "\" description = \"" << description << "\"></plot>\n";
  
  for (vcl_map<vcl_string, vgl_point_2d<double> >::iterator iter = perf_param_map_.begin(); iter != perf_param_map_.end(); iter++) {
    of << "<point legend =\"" << iter->first << "\" y = \"" << (iter->second).y() << "\" x = \"" << (iter->second).x() << "\"></point>\n"; 
  }
  of << "</type>\n";

  return;
}

//: print evaluation.xml with the statistics passed
void dborl_algo_params::print_evaluation_xml(vcl_map<vcl_string, dborl_exp_stat_sptr>& category_statistics, bool print_FN)
{
  vcl_ofstream of(evaluation_file().c_str());
  if (!of) {
    vcl_cout << "dborl_algo_params::print_evaluation_xml() could not open the file: " << evaluation_file() << "\n";
    return;
  }

  of << "<type name = \"evaluation\">\n";
  of << "<algorithm name=\"" << algo_name_ << "\"></algorithm>\n";
  
  for (vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator itt = category_statistics.begin(); itt != category_statistics.end(); itt++)
    itt->second->print_stats(itt->first, of, print_FN);
  
  of << "</type>\n";

  return;
}



