// This is brcv/rec/dborl/algo/dborl_utilities.cxx
#include <algorithm>
#include <fstream>
#include <iostream>
#include <borld/borld_category_info.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <vul/vul_file.h>
#include <vsol/vsol_box_2d.h>
#include "dborl_utilities.h"

//: simple parse: put each string in the file into the strings vector
bool parse_strings_from_file(std::string fname, std::vector<std::string>& strings)
{
  std::ifstream fp(fname.c_str());
  if (!fp) {
    std::cout<<" In dborl_utilities - parse_file(): Unable to Open " << fname << std::endl;
    return false;
  }

  while (!fp.eof()) {
    std::string name;
    fp >> name;
    if (name.size() > 0) {
      std::string just_name = vul_file::strip_extension(name.c_str());
      strings.push_back(just_name);
    }
  }
  fp.close();
  return true;
}

bool parse_lines_from_file(std::string fname, std::vector<std::string>& strings)
{
  std::ifstream fp(fname.c_str());
  if (!fp) {
    std::cout<<" In dborl_utilities - parse_file(): Unable to Open " << fname << std::endl;
    return false;
  }

  while (!fp.eof()) {
    char buffer[1000];
    fp.getline(buffer, 1000);
    std::string name = buffer;
    if (name.size() > 0) {
      strings.push_back(name);
    }
  }
  fp.close();
  return true;
}

//: return the id in the categories vector for the category one of whose prefixes matches the object name
int dborl_get_category(std::string object_name, std::vector<borld_category_info_sptr>& cats)
{
  for (unsigned i = 0; i < cats.size(); i++) {
    for (unsigned j = 0; j < cats[i]->prefix_list_.size(); j++) {
      if (object_name.find(cats[i]->prefix_list_[j]) != std::string::npos)
        return i;
    }
  }

  return -1;
}

bool parse_evaluation_file(std::string fname, std::map<std::string, buld_exp_stat_sptr>& category_statistics, std::string& algo_name)
{

  bxml_document param_doc = bxml_read(fname);
  if (!param_doc.root_element())
    return false;
  
  if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
    std::cout << "params root is not ELEMENT\n";
    return false;
  }

  bxml_data_sptr root = param_doc.root_element();
  
  bxml_element * data = static_cast<bxml_element*>(root.ptr());
  if (!data)
    return false;

  std::string val;

  data->get_attribute("name", val);
  if (val.compare("evaluation") != 0) {
    std::cout << " Not an evaluation file\n";
    return false;
  }

  int ival;
  for (bxml_element::const_data_iterator it = data->data_begin(); it != data->data_end(); it++) {
    if ((*it)->type() != bxml_data::ELEMENT)
      continue;

    bxml_element * elm = static_cast<bxml_element*>((*it).ptr());

    if (elm->name().compare("algorithm") == 0) {
      algo_name = elm->attribute("name");
      continue;
    }

    buld_exp_stat_sptr s = new buld_exp_stat();
    
    std::stringstream ss(elm->attribute("TP"));
    ss >> ival;
    s->increment_TP_by(ival);
    
    std::stringstream ss2(elm->attribute("FP"));
    ss2 >> ival;
    s->increment_FP_by(ival);

    std::stringstream ss3(elm->attribute("TN"));
    ss3 >> ival;
    s->increment_TN_by(ival);

    std::stringstream ss4(elm->attribute("FN"));
    ss4 >> ival;
    s->increment_FN_by(ival);

    category_statistics[elm->attribute("name")] = s;
  }

  return true;
}

//: print the evaluation result of an instance along with the detected boxes
bool print_obj_evaluation(std::string out_file, std::string obj_name, std::vector<vsol_box_2d_sptr>& detected_boxes, std::vector<std::string>& categories, buld_exp_stat& stat)
{
  if (!detected_boxes.size() == categories.size()) {
    std::cout << "print_evaluation() - detected boxes size: " << detected_boxes.size() << " categories size: " << categories.size() << " they are not equal! exiting without printing!\n"; 
    return false;
  }

  std::ofstream of(out_file.c_str());
  of << "<type name=\"evaluation_object\">\n";
  of << "<statistics name=\"" << obj_name << "\" TP=\"" << stat.TP_ << "\" FP=\"" << stat.FP_ << "\" TN=\"" << stat.TN_ << "\" FN=\"" << stat.FN_ << "\"></statistics>\n";
  of << "<description>\n";
  for (unsigned i = 0; i < detected_boxes.size(); i++) {
    if (!detected_boxes[i])
      continue;
    if (detected_boxes[i]->area() <= 0)
      continue;

    of << "<instance category=\"" << categories[i] << "\" "; 
    of << "bndboxminx=\"" << detected_boxes[i]->get_min_x() << "\" ";
    of << "bndboxminy=\"" << detected_boxes[i]->get_min_y() << "\" ";
    of << "bndboxmaxx=\"" << detected_boxes[i]->get_max_x() << "\" ";
    of << "bndboxmaxy=\"" << detected_boxes[i]->get_max_y() << "\">";
    of << "</instance>\n";
  }
  of << "</description>\n";
  of.close();
  return true;
}
bool parse_obj_evaluation(std::string out_file, std::string& obj_name, std::vector<vsol_box_2d_sptr>& detected_boxes, std::vector<std::string>& categories, buld_exp_stat& stat)
{
  detected_boxes.clear();
  categories.clear();

  bxml_document param_doc = bxml_read(out_file);
  if (!param_doc.root_element())
    return false;
  
  if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
    std::cout << "params root is not ELEMENT\n";
    return false;
  }

  bxml_data_sptr root = param_doc.root_element();
  
  bxml_element * data = static_cast<bxml_element*>(root.ptr());
  if (!data)
    return false;

  std::string val;

  data->get_attribute("name", val);
  if (val.compare("evaluation_object") != 0) {
    std::cout << " Not an object evaluation file\n";
    return false;
  }

  int ival;
  for (bxml_element::const_data_iterator it = data->data_begin(); it != data->data_end(); it++) {
    if ((*it)->type() != bxml_data::ELEMENT)
      continue;

    bxml_element * elm = static_cast<bxml_element*>((*it).ptr());

    if (elm->name().compare("statistics") == 0) {
      obj_name = elm->attribute("name");
      
      std::stringstream ss(elm->attribute("TP"));
      ss >> ival;
      stat.TP_ = ival;
      
      std::stringstream ss2(elm->attribute("FP"));
      ss2 >> ival;
      stat.FP_ = ival;

      std::stringstream ss3(elm->attribute("TN"));
      ss3 >> ival;
      stat.TN_ = ival;

      std::stringstream ss4(elm->attribute("FN"));
      ss4 >> ival;
      stat.FN_ = ival;
    } else if (elm->name().compare("description") == 0) {
      for (bxml_element::const_data_iterator ite = elm->data_begin(); ite != elm->data_end(); ite++) {
        if ((*ite)->type() != bxml_data::ELEMENT)
          continue;

        bxml_element * elm2 = static_cast<bxml_element*>((*ite).ptr());
        if (elm2->name().compare("instance") == 0) {
          std::string cat = elm2->attribute("category");
          vsol_box_2d_sptr b = new vsol_box_2d();
          float valx, valy;
          std::stringstream ss(elm2->attribute("bndboxminx"));
          ss >> valx;
          std::stringstream ss2(elm2->attribute("bndboxminy"));
          ss2 >> valy;
          b->add_point(valx, valy);
          std::stringstream ss3(elm2->attribute("bndboxmaxx"));
          ss3 >> valx;
          std::stringstream ss4(elm2->attribute("bndboxmaxy"));
          ss4 >> valy;
          b->add_point(valx, valy);
          detected_boxes.push_back(b);
          categories.push_back(cat);
        }

      }

    }
  }

  return true;
}

dborl_parameter_base* convert_parameter_from_bpro1(std::string prefix, std::string prefix_desc, bpro1_param* par)
{
  dborl_parameter_base* p;

  if ( bpro1_choice_param_type* param = dynamic_cast<bpro1_choice_param_type*>(par) ) {
    std::string desc = param->description();
    for (unsigned i = 0; i < param->choices().size(); i++) {
      std::stringstream aa;
      aa << i;
      desc = desc + ", " + aa.str() + ": " + (param->choices())[i];
    }
    p = new dborl_parameter<unsigned>(prefix, prefix + param->name(), prefix_desc + desc, param->value(), param->default_value());
    return p;
  }
  else if( bpro1_param_type<int> * param = dynamic_cast<bpro1_param_type<int> *>(par) ) {
    p = new dborl_parameter<int>(prefix, prefix + param->name(), prefix_desc + param->description(), param->value(), param->default_value());
    return p;
  }
  else if( bpro1_param_type<unsigned> * param = dynamic_cast<bpro1_param_type<unsigned> *>(par) ) {
    p = new dborl_parameter<unsigned>(prefix, prefix + param->name(), prefix_desc + param->description(), param->value(), param->default_value());
    return p;
  }
  else if( bpro1_param_type<float> * param = dynamic_cast<bpro1_param_type<float> *>(par) ) {
    p = new dborl_parameter<float>(prefix, prefix + param->name(), prefix_desc + param->description(), param->value(), param->default_value());
    return p;
  }
  else if( bpro1_param_type<double> * param = dynamic_cast<bpro1_param_type<double> *>(par) ) {
    p = new dborl_parameter<double>(prefix, prefix + param->name(), prefix_desc + param->description(), param->value(), param->default_value());
    return p;
  }
  else if( bpro1_param_type<std::string> * param = dynamic_cast<bpro1_param_type<std::string> *>(par) ) {
    p = new dborl_parameter<std::string>(prefix, prefix + param->name(), prefix_desc + param->description(), param->value(), param->default_value());
    return p;
  }
  else if( bpro1_param_type<bool> * param = dynamic_cast<bpro1_param_type<bool> *>(par) ) {
    p = new dborl_parameter<bool>(prefix, prefix + param->name(), prefix_desc + param->description(), param->value(), param->default_value());
    return p;
  }
  else if( bpro1_param_type<bpro1_filepath> * param = dynamic_cast<bpro1_param_type<bpro1_filepath> *>(par) ) {
    p = new dborl_parameter<std::string>(prefix, prefix + param->name(), prefix_desc + param->description(), param->value().path, param->default_value().path);
    p->set_type_info(dborl_parameter_type_info::PATH);
    return p;
  }
  else{
    std::cerr << "dborl_utilities::convert_parameter() - No valid argument type for parameter: " << par->name() << std::endl;
    return 0;
  }

}

void set_process_parameters_of_bpro1(dborl_algo_params& algo_params, bpro1_process& pro, std::string algo_abbreviation) 
{
  dborl_set_params_of_bpro1_process(algo_params.param_list_, 
    algo_abbreviation, pro.parameters());
  ////
  //for (unsigned i = 0; i < algo_params.param_list_.size(); i++) {
  //  if (algo_params.param_list_[i]->param_group().compare(algo_abbreviation) != 0)
  //    continue;

  //  std::string name = algo_params.param_list_[i]->param_name();
  //  name = name.substr(algo_abbreviation.size(), name.length());
  //  if (algo_params.param_list_[i]->type_info() == dborl_parameter_type_info::PATH) {
  //    bpro1_filepath path(algo_params.param_list_[i]->value_str());
  //    pro.parameters()->set_value(name, path);
  //  } else if (algo_params.param_list_[i]->type_info() == dborl_parameter_type_info::FLAG) {
  //    std::string val_str = algo_params.param_list_[i]->value_str();
  //    val_str.compare("off") == 0 ? pro.parameters()->set_value(name, false) : pro.parameters()->set_value(name, true);
  //  } else 
  //    bpro1_parameters_set_value_from_str((*pro.parameters()), name, algo_params.param_list_[i]->value_str());
  //}
}



// ----------------------------------------------------------------------------
//: Set parameters of a process using a list of parameters
void dborl_set_params_of_bpro1_process(const std::vector<dborl_parameter_base* >& param_list, 
                                       const std::string param_group_name,  
                                       const bpro1_parameters_sptr& bpro1_params)
{
  for (unsigned i = 0; i < param_list.size(); ++i) 
  {
    if (param_list[i]->param_group().compare(param_group_name) != 0)
      continue;

    std::string name = param_list[i]->param_name();
    name = name.substr(param_group_name.size(), name.length());
    if (param_list[i]->type_info() == dborl_parameter_type_info::PATH) 
    {
      bpro1_filepath path(param_list[i]->value_str());
      bpro1_params->set_value(name, path);
    } 
    else if (param_list[i]->type_info() == dborl_parameter_type_info::FLAG) 
    {
      std::string val_str = param_list[i]->value_str();
      if (val_str.compare("off") == 0)
      {
        bpro1_params->set_value(name, false);
      }
      else
      {
        bpro1_params->set_value(name, true);
      }
    } 
    else
    {
      bpro1_parameters_set_value_from_str(*bpro1_params, name, param_list[i]->value_str());

    }
  }
}


