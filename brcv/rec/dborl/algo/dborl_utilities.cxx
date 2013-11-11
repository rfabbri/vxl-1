// This is brcv/rec/dborl/algo/dborl_utilities.cxx
#include <vcl_algorithm.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>

#include "dborl_utilities.h"
#include <dborl/dborl_category_info.h>

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <vul/vul_file.h>
#include <vsol/vsol_box_2d.h>

//: simple parse: put each string in the file into the strings vector
bool parse_strings_from_file(vcl_string fname, vcl_vector<vcl_string>& strings)
{
  vcl_ifstream fp(fname.c_str());
  if (!fp) {
    vcl_cout<<" In dborl_utilities - parse_file(): Unable to Open " << fname << vcl_endl;
    return false;
  }

  while (!fp.eof()) {
    vcl_string name;
    fp >> name;
    if (name.size() > 0) {
      vcl_string just_name = vul_file::strip_extension(name.c_str());
      strings.push_back(just_name);
    }
  }
  fp.close();
  return true;
}

bool parse_lines_from_file(vcl_string fname, vcl_vector<vcl_string>& strings)
{
  vcl_ifstream fp(fname.c_str());
  if (!fp) {
    vcl_cout<<" In dborl_utilities - parse_file(): Unable to Open " << fname << vcl_endl;
    return false;
  }

  while (!fp.eof()) {
    char buffer[1000];
    fp.getline(buffer, 1000);
    vcl_string name = buffer;
    if (name.size() > 0) {
      strings.push_back(name);
    }
  }
  fp.close();
  return true;
}

//: return the id in the categories vector for the category one of whose prefixes matches the object name
int dborl_get_category(vcl_string object_name, vcl_vector<dborl_category_info_sptr>& cats)
{
  for (unsigned i = 0; i < cats.size(); i++) {
    for (unsigned j = 0; j < cats[i]->prefix_list_.size(); j++) {
      if (object_name.find(cats[i]->prefix_list_[j]) != vcl_string::npos)
        return i;
    }
  }

  return -1;
}

bool parse_evaluation_file(vcl_string fname, vcl_map<vcl_string, dborl_exp_stat_sptr>& category_statistics, vcl_string& algo_name)
{

  bxml_document param_doc = bxml_read(fname);
  if (!param_doc.root_element())
    return false;
  
  if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
    vcl_cout << "params root is not ELEMENT\n";
    return false;
  }

  bxml_data_sptr root = param_doc.root_element();
  
  bxml_element * data = static_cast<bxml_element*>(root.ptr());
  if (!data)
    return false;

  vcl_string val;

  data->get_attribute("name", val);
  if (val.compare("evaluation") != 0) {
    vcl_cout << " Not an evaluation file\n";
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

    dborl_exp_stat_sptr s = new dborl_exp_stat();
    
    vcl_stringstream ss(elm->attribute("TP"));
    ss >> ival;
    s->increment_TP_by(ival);
    
    vcl_stringstream ss2(elm->attribute("FP"));
    ss2 >> ival;
    s->increment_FP_by(ival);

    vcl_stringstream ss3(elm->attribute("TN"));
    ss3 >> ival;
    s->increment_TN_by(ival);

    vcl_stringstream ss4(elm->attribute("FN"));
    ss4 >> ival;
    s->increment_FN_by(ival);

    category_statistics[elm->attribute("name")] = s;
  }

  return true;
}

//: print the evaluation result of an instance along with the detected boxes
bool print_obj_evaluation(vcl_string out_file, vcl_string obj_name, vcl_vector<vsol_box_2d_sptr>& detected_boxes, vcl_vector<vcl_string>& categories, dborl_exp_stat& stat)
{
  if (!detected_boxes.size() == categories.size()) {
    vcl_cout << "print_evaluation() - detected boxes size: " << detected_boxes.size() << " categories size: " << categories.size() << " they are not equal! exiting without printing!\n"; 
    return false;
  }

  vcl_ofstream of(out_file.c_str());
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
bool parse_obj_evaluation(vcl_string out_file, vcl_string& obj_name, vcl_vector<vsol_box_2d_sptr>& detected_boxes, vcl_vector<vcl_string>& categories, dborl_exp_stat& stat)
{
  detected_boxes.clear();
  categories.clear();

  bxml_document param_doc = bxml_read(out_file);
  if (!param_doc.root_element())
    return false;
  
  if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
    vcl_cout << "params root is not ELEMENT\n";
    return false;
  }

  bxml_data_sptr root = param_doc.root_element();
  
  bxml_element * data = static_cast<bxml_element*>(root.ptr());
  if (!data)
    return false;

  vcl_string val;

  data->get_attribute("name", val);
  if (val.compare("evaluation_object") != 0) {
    vcl_cout << " Not an object evaluation file\n";
    return false;
  }

  int ival;
  for (bxml_element::const_data_iterator it = data->data_begin(); it != data->data_end(); it++) {
    if ((*it)->type() != bxml_data::ELEMENT)
      continue;

    bxml_element * elm = static_cast<bxml_element*>((*it).ptr());

    if (elm->name().compare("statistics") == 0) {
      obj_name = elm->attribute("name");
      
      vcl_stringstream ss(elm->attribute("TP"));
      ss >> ival;
      stat.TP_ = ival;
      
      vcl_stringstream ss2(elm->attribute("FP"));
      ss2 >> ival;
      stat.FP_ = ival;

      vcl_stringstream ss3(elm->attribute("TN"));
      ss3 >> ival;
      stat.TN_ = ival;

      vcl_stringstream ss4(elm->attribute("FN"));
      ss4 >> ival;
      stat.FN_ = ival;
    } else if (elm->name().compare("description") == 0) {
      for (bxml_element::const_data_iterator ite = elm->data_begin(); ite != elm->data_end(); ite++) {
        if ((*ite)->type() != bxml_data::ELEMENT)
          continue;

        bxml_element * elm2 = static_cast<bxml_element*>((*ite).ptr());
        if (elm2->name().compare("instance") == 0) {
          vcl_string cat = elm2->attribute("category");
          vsol_box_2d_sptr b = new vsol_box_2d();
          float valx, valy;
          vcl_stringstream ss(elm2->attribute("bndboxminx"));
          ss >> valx;
          vcl_stringstream ss2(elm2->attribute("bndboxminy"));
          ss2 >> valy;
          b->add_point(valx, valy);
          vcl_stringstream ss3(elm2->attribute("bndboxmaxx"));
          ss3 >> valx;
          vcl_stringstream ss4(elm2->attribute("bndboxmaxy"));
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

dborl_parameter_base* convert_parameter_from_bpro1(vcl_string prefix, vcl_string prefix_desc, bpro1_param* par)
{
  dborl_parameter_base* p;

  if ( bpro1_choice_param_type* param = dynamic_cast<bpro1_choice_param_type*>(par) ) {
    vcl_string desc = param->description();
    for (unsigned i = 0; i < param->choices().size(); i++) {
      vcl_stringstream aa;
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
  else if( bpro1_param_type<vcl_string> * param = dynamic_cast<bpro1_param_type<vcl_string> *>(par) ) {
    p = new dborl_parameter<vcl_string>(prefix, prefix + param->name(), prefix_desc + param->description(), param->value(), param->default_value());
    return p;
  }
  else if( bpro1_param_type<bool> * param = dynamic_cast<bpro1_param_type<bool> *>(par) ) {
    p = new dborl_parameter<bool>(prefix, prefix + param->name(), prefix_desc + param->description(), param->value(), param->default_value());
    return p;
  }
  else if( bpro1_param_type<bpro1_filepath> * param = dynamic_cast<bpro1_param_type<bpro1_filepath> *>(par) ) {
    p = new dborl_parameter<vcl_string>(prefix, prefix + param->name(), prefix_desc + param->description(), param->value().path, param->default_value().path);
    p->set_type_info(dborl_parameter_type_info::PATH);
    return p;
  }
  else{
    vcl_cerr << "dborl_utilities::convert_parameter() - No valid argument type for parameter: " << par->name() << vcl_endl;
    return 0;
  }

}

void set_process_parameters_of_bpro1(dborl_algo_params& algo_params, bpro1_process& pro, vcl_string algo_abbreviation) 
{
  dborl_set_params_of_bpro1_process(algo_params.param_list_, 
    algo_abbreviation, pro.parameters());
  ////
  //for (unsigned i = 0; i < algo_params.param_list_.size(); i++) {
  //  if (algo_params.param_list_[i]->param_group().compare(algo_abbreviation) != 0)
  //    continue;

  //  vcl_string name = algo_params.param_list_[i]->param_name();
  //  name = name.substr(algo_abbreviation.size(), name.length());
  //  if (algo_params.param_list_[i]->type_info() == dborl_parameter_type_info::PATH) {
  //    bpro1_filepath path(algo_params.param_list_[i]->value_str());
  //    pro.parameters()->set_value(name, path);
  //  } else if (algo_params.param_list_[i]->type_info() == dborl_parameter_type_info::FLAG) {
  //    vcl_string val_str = algo_params.param_list_[i]->value_str();
  //    val_str.compare("off") == 0 ? pro.parameters()->set_value(name, false) : pro.parameters()->set_value(name, true);
  //  } else 
  //    bpro1_parameters_set_value_from_str((*pro.parameters()), name, algo_params.param_list_[i]->value_str());
  //}
}



// ----------------------------------------------------------------------------
//: Set parameters of a process using a list of parameters
void dborl_set_params_of_bpro1_process(const vcl_vector<dborl_parameter_base* >& param_list, 
                                       const vcl_string param_group_name,  
                                       const bpro1_parameters_sptr& bpro1_params)
{
  for (unsigned i = 0; i < param_list.size(); ++i) 
  {
    if (param_list[i]->param_group().compare(param_group_name) != 0)
      continue;

    vcl_string name = param_list[i]->param_name();
    name = name.substr(param_group_name.size(), name.length());
    if (param_list[i]->type_info() == dborl_parameter_type_info::PATH) 
    {
      bpro1_filepath path(param_list[i]->value_str());
      bpro1_params->set_value(name, path);
    } 
    else if (param_list[i]->type_info() == dborl_parameter_type_info::FLAG) 
    {
      vcl_string val_str = param_list[i]->value_str();
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


