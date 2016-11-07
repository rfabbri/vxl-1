//:
// \file
// \brief
// \author Ozge C Ozcanli (Brown)
// \date   December 20, 2007
//
//

#include "dborl_parameter.h"
#include <vcl_sstream.h>
#include <vcl_typeinfo.h>

template <class T> 
dborl_parameter<T> &
dborl_parameter<T>::operator =(const dborl_parameter<T> &rhs) {
  param_group_ = rhs.param_group_;
  param_name_ = rhs.param_name_;
  desc_ = rhs.desc_;
  long_desc_ = rhs.long_desc_;
  type_ = rhs.type_;
  value_ = rhs.value_;
  default_ = rhs.default_;
  system_info_ = rhs.system_info_;
  system_info_index_ = rhs.system_info_index_;
  file_type_ = rhs.file_type_;
  return *this;
}

template <class T> 
void dborl_parameter<T>::set_values(vcl_string group, vcl_string name, vcl_string description, T value)
{
  param_group_ = group;
  param_name_ = name;
  desc_ = description;
  long_desc_ = description;
  
  type_ = typeid(value).name();
  if (type_.compare(typeid(false).name()) == 0) {
    type_ = "flag";
    type_info_ = dborl_parameter_type_info::FLAG;
  } else if (type_.compare(typeid(vcl_string("dummystring")).name()) == 0) {
    type_ = "string";
    type_info_ = dborl_parameter_type_info::STRING;
  } else if (type_.compare(typeid(0.0f).name()) == 0) {
    type_ = "float";
    type_info_ = dborl_parameter_type_info::NUMERAL;  // float, double, int, short etc.
  } else if (type_.compare(typeid(0.0).name()) == 0) {
    type_ = "double";
    type_info_ = dborl_parameter_type_info::NUMERAL;  // float, double, int, short etc.
  } else {
    type_ = "int";
    type_info_ = dborl_parameter_type_info::NUMERAL;  // float, double, int, short etc.
  }

  //vcl_cout << " type of parameter: " << param_name_ << " is " << type_ << vcl_endl;
  value_ = value;
  default_ = value;
  system_info_ = dborl_parameter_system_info::NOT_APPLICABLE;  // default value for system_info field
  system_info_index_ = -1;  // not applicable
  file_type_ = "";
}

template <class T> 
void dborl_parameter<T>::set_values(vcl_vector<dborl_parameter_base*>& list, vcl_string group, vcl_string name, vcl_string description, T value)
{
  list.push_back(dynamic_cast<dborl_parameter_base*>(this));
  set_values(group, name, description, value);
}
//: constructor for special parameters with an option to set type info field so that type can be set
template <class T> 
void dborl_parameter<T>::set_values(vcl_string group, vcl_string name, vcl_string description, T value,
              T default_value, int system_info_index, short system_info, vcl_string file_type, char type_info)
{
  param_group_ = group;
  param_name_ = name;
  desc_ = description;
  long_desc_ = description;
  value_ = value;
  default_ = default_value;
  system_info_ = system_info;
  system_info_index_ = system_info_index;
  file_type_ = file_type;
  type_info_ = type_info;

  switch (type_info) {
    case dborl_parameter_type_info::FILEASSOC:
      type_ = "fileassoc";
      break;
    case dborl_parameter_type_info::FLAG:
      type_ = "flag";
      break;
    case dborl_parameter_type_info::STRING:
      type_ = "string";
      break;
    default:  // all the cases when dborl_parameter_type_info::NUMERAL and any other
      type_ = typeid(value).name();
      if (type_.compare(typeid(false).name()) == 0) {
        type_ = "flag";
        type_info_ = dborl_parameter_type_info::FLAG;  // reset type_info just in case it was not set properly
      } else if (type_.compare(typeid(vcl_string("dummystring")).name()) == 0) {
        type_ = "string";
        type_info_ = dborl_parameter_type_info::STRING;
      } else if (type_.compare(typeid(0.0f).name()) == 0) {
        type_ = "float";
        type_info_ = dborl_parameter_type_info::NUMERAL;  // float, double, int, short etc.
      } else if (type_.compare(typeid(0.0).name()) == 0) {
        type_ = "double";
        type_info_ = dborl_parameter_type_info::NUMERAL;  // float, double, int, short etc.
      } else {
        type_ = "int";
        type_info_ = dborl_parameter_type_info::NUMERAL;  // float, double, int, short etc.
      }
      break;
  }

  //vcl_cout << " type of parameter: " << param_name_ << " is " << type_ << vcl_endl;
 
}

template <class T> 
void dborl_parameter<T>::set_values(vcl_vector<dborl_parameter_base*>& list, vcl_string group, vcl_string name, vcl_string description, T value, 
                                    T default_value, int system_info_index, short system_info, vcl_string file_type, char type_info)
{
  list.push_back(dynamic_cast<dborl_parameter_base*>(this));
  set_values(group, name, description, value, default_value, system_info_index, system_info, file_type, type_info);
}

template <class T> 
vcl_string dborl_parameter<T>::get_params_file_tag()
{
  vcl_string tag = "\t\t<param attribute_label=\"" + param_group_ + "*" + param_name_ + "\" description=\"" + desc_ + "\"";
  tag = tag + " type=\"" + type_ + "\" default=\"" + default_str() + "\" value=\"" + value_str() + "\"";

  if (system_info_ != dborl_parameter_system_info::NOT_APPLICABLE) {// && system_info_ != dborl_parameter_system_info::NOT_DEFINED) {
    if (system_info_index_ >= 0) {
      vcl_stringstream def_sys;
      def_sys << system_info_index_;
      tag = tag + " system_info_index=\"" + def_sys.str() + "\"";
    }
    tag = tag + " system_info=\"";
    tag = tag + dborl_parameter_system_info::get_system_info_str(system_info_);
    tag = tag + "\"";
  }

  if (file_type_.compare("") != 0) {
    tag = tag + " file_type=\"" + file_type_ + "\"";
  }

  tag = tag + "/>\n";

  return tag;
}

template <class T> 
vcl_string dborl_parameter<T>::value_str() 
{ 
  vcl_string tag;
  vcl_stringstream ss;
  ss << value_;
  tag = ss.str();
  switch(type_info_) {
    case dborl_parameter_type_info::FLAG : {
      tag.compare("1") == 0 ? tag = "on" : tag = "off";
      break;
                                           }
    default:                            {
      // including dborl_parameter_type_info::NUMERAL & dborl_parameter_type_info::STRING 
      break;
                                        }
  }
  return tag;
}

template <class T> 
vcl_string dborl_parameter<T>::default_str() 
{ 
  vcl_string tag;
  vcl_stringstream ss;
  ss << default_;
  tag = ss.str();
  switch(type_info_) {
    case dborl_parameter_type_info::FLAG : {
      tag.compare("1") == 0 ? tag = "on" : tag = "off";
      break;
                                           }
    default:                            {
      // including dborl_parameter_type_info::NUMERAL & dborl_parameter_type_info::STRING 
      break;
                                        }
  }
  return tag;
}

template <class T> 
void dborl_parameter<T>::parse_value_from_str(vcl_string val)
{
  switch(type_info_) {
    case dborl_parameter_type_info::FLAG : {
      value_ = val.compare("on") == 0 ? true : false;
      break;
    }
    default: {
      // including dborl_parameter_type_info::NUMERAL
      vcl_stringstream ss(val);
      ss >> value_;
      break;
    }
  }
}

template<> 
void dborl_parameter<vcl_string>::parse_value_from_str(vcl_string val)
{
  value_ = val;
}

template class dborl_parameter<vcl_string>;
template class dborl_parameter<int>;
template class dborl_parameter<unsigned>;
template class dborl_parameter<bool>;
template class dborl_parameter<short>;
template class dborl_parameter<float>;
template class dborl_parameter<double>;
template class dborl_parameter<char>;
