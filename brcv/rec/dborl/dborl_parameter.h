//:
// \file
// \brief A class to hold a single parameter along with all of its attributes and methods to generate parameter files for the algorithms
//        The design of this class is very similar to the vul_arg class in vxl/core/vul
//        the templated class should be used with and instantiated for the types: bool, vcl_string, int, short, float, double, char
//        Other types are not supported since they cannot be passed easily via xml files nor their types can be determined by 'typeid'
//
// \author Ozge C Ozcanli (Brown)
// \date   December 20, 2007
//
// \verbatim
//  Modifications
//  Ricardo Fabbri - 18 October 2009 - specialized parse_value_from_str to strings
// \endverbatim
//


#if !defined(_DBORL_PARAMETER_H)
#define _DBORL_PARAMETER_H

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

class dborl_parameter_type_info
{
public:
  enum possible_types {
    STRING,
    FLAG,  // bool
    NUMERAL,
    FILEASSOC,  // any parameter that stores the path of files stored in the ORL database 
                // which need to be passed to the algorithm by ORL need to have this type instead of STRING
    PATH,       // used during conversion from bpro1_parameter class to dborl_parameter class
  };
};


class dborl_parameter_base
{
protected:
  vcl_string param_group_;
  vcl_string param_name_;
  vcl_string desc_;
  vcl_string long_desc_;
  vcl_string type_;

  short system_info_;
  int system_info_index_;   // if an algorithm has more than one input each requiring VOX to pass some paths, attributes etc via passing a system_info field,
                              // then this index determines which system_info field belongs to which input. inputs should be indexed by 0, 1, 2 etc by the programmer
  vcl_string file_type_;

  char type_info_;

public:

  virtual ~dborl_parameter_base() {}
  vcl_string param_group() { return param_group_; }
  vcl_string param_name() { return param_name_; }
  vcl_string desc() { return desc_; }
  vcl_string long_desc() { return long_desc_; }
  vcl_string type() { return type_; }
  char type_info() { return type_info_; }
  short system_info() { return system_info_; }
  int system_info_index() { return system_info_index_; }
  vcl_string file_type() { return file_type_; }

  void set_long_description(vcl_string long_description) { long_desc_ = long_description; }
  void set_param_group(vcl_string g_name) { param_group_ = g_name; }

  void set_type_info(char type_info) { type_info_ = type_info; }

  virtual vcl_string get_params_file_tag() = 0;
  virtual vcl_string value_str() = 0;
  virtual vcl_string default_str() = 0;
  virtual void parse_value_from_str(vcl_string val) = 0;
};

template <class T> 
class dborl_parameter : public dborl_parameter_base
{
protected:
  T value_;     // protected to force the correct assignment of type_ string automatically
  T default_;
  
public:
  dborl_parameter() {};
  
  //: either construct with the following constructors
  dborl_parameter(vcl_string group, vcl_string name, vcl_string description, T value) { set_values(group, name, description, value); }
  dborl_parameter(vcl_string group, vcl_string name, vcl_string description, T value, T default_value) { 
    set_values(group, name, description, value, default_value); }
  dborl_parameter(vcl_string group, vcl_string name, vcl_string description, T value, T default_value, int system_info_index, short system_info) { 
    set_values(group, name, description, value, default_value, system_info_index, system_info); }
  dborl_parameter(vcl_string group, vcl_string name, vcl_string description, T value, T default_value, int system_info_index, short system_info, vcl_string file_type) { 
    set_values(group, name, description, value, default_value, system_info_index, system_info, file_type); }

  //: constructor for special parameters with an option to set type info field so that type can be set
  dborl_parameter(vcl_string group, vcl_string name, vcl_string description, T value, T default_value, int system_info_index, short system_info, vcl_string file_type, char type_info)
  { set_values(group, name, description, value, default_value, system_info_index, system_info, file_type, type_info); }
  
  //: or set the values later with the following methods
  void set_values(vcl_string group, vcl_string name, vcl_string description, T value);
  void set_values(vcl_string group, vcl_string name, vcl_string description, T value, T default_value) {
    set_values(group, name, description, value); 
    default_ = default_value;
  }
  void set_values(vcl_string group, vcl_string name, vcl_string description, T value, T default_value, int system_info_index, short system_info) {
    set_values(group, name, description, value, default_value);
    system_info_ = system_info;
    system_info_index_ = system_info_index;
  }
  void set_values(vcl_string group, vcl_string name, vcl_string description, T value, T default_value, int system_info_index, short system_info, vcl_string file_type) {
    set_values(group, name, description, value, default_value);
    system_info_ = system_info;
    system_info_index_ = system_info_index;
    file_type_ = file_type; 
  }
  //: constructor for special parameters with an option to set type info field so that type can be set
  void set_values(vcl_string group, vcl_string name, vcl_string description, T value, T default_value, int system_info_index, short system_info, vcl_string file_type, char type_info);

  //: or set the values later with the following methods and add to the given list
  void set_values(vcl_vector<dborl_parameter_base*>& list, vcl_string group, vcl_string name, vcl_string description, T value);
  void set_values(vcl_vector<dborl_parameter_base*>& list, vcl_string group, vcl_string name, vcl_string description, T value, T default_value) {
    set_values(list, group, name, description, value); 
    default_ = default_value;
  }
  void set_values(vcl_vector<dborl_parameter_base*>& list, vcl_string group, vcl_string name, vcl_string description, T value, T default_value, int system_info_index, short system_info) {
    set_values(list, group, name, description, value, default_value);
    system_info_ = system_info;
    system_info_index_ = system_info_index;
  }
  void set_values(vcl_vector<dborl_parameter_base*>& list, vcl_string group, vcl_string name, vcl_string description, T value, T default_value, int system_info_index, short system_info, vcl_string file_type) {
    set_values(list, group, name, description, value, default_value);
    system_info_ = system_info;
    system_info_index_ = system_info_index;
    file_type_ = file_type; 
  }
  //: constructor for special parameters with an option to set type info field so that type can be set
  //  if algorithm will be used in ORL framework, all the file path parameters which will be passed by ORL should be set with type_info = dborl_parameter_type_info::FILEASSOC
  void set_values(vcl_vector<dborl_parameter_base*>& list, vcl_string group, vcl_string name, vcl_string description, T value, T default_value, int system_info_index, short system_info, vcl_string file_type, char type_info);

  void set_default(T &val) { default_ = val; }
  T get_default() { return default_; }

  T      & operator () () { return value_; }
  const T      & operator () () const { return value_; }
  T      & operator = (const T & rhs) { value_ = rhs; return value_; }
  dborl_parameter<T> & operator =(const dborl_parameter<T> &rhs);

  virtual vcl_string get_params_file_tag();
  virtual vcl_string value_str(); 
  virtual vcl_string default_str(); 
  virtual void parse_value_from_str(vcl_string val);

};

//: add to the following enumeration as new system_info attribute values are defined for application running with ORL 
//  see the document: orl_algorithm_specification.doc under the documentation for ORL for an explanation of each system info attribute value
class dborl_parameter_system_info
{
public:
  enum system_info {
    NOT_APPLICABLE,
    NOT_DEFINED,
    INPUT_OBJECT_DIR,
    INPUT_OBJECT_ID,
    INPUT_OBJECT_STRING_ID,
    INPUT_OBJECT_PARENT_ID,
    INPUT_OBJECT_PARENT_DIR,
    OUTPUT_DIRECTORY,
    STATUS_BLOCK,
    PARAM_BLOCK,
    OUTPUT_FILE,
    OUTPUT_PERF,
    OUTPUT_CATEGORIZATION,
    OUTPUT_EVALUATION,
    OUTPUT_VIDEO,
    OUTPUT_PS,
    OUTPUT_VRML,
    OUTPUT_SVG,
    OUTPUT
  };

  static vcl_string get_system_info_str(short info) {
    vcl_string tag;
    switch(info) {
      case INPUT_OBJECT_DIR: tag = "INPUT_OBJECT_DIR"; break;
      case INPUT_OBJECT_ID: tag = "INPUT_OBJECT_ID"; break;
      case INPUT_OBJECT_STRING_ID: tag = "INPUT_OBJECT_STRING_ID"; break;
      case INPUT_OBJECT_PARENT_ID: tag = "INPUT_OBJECT_PARENT_ID"; break;
      case INPUT_OBJECT_PARENT_DIR: tag = "INPUT_OBJECT_PARENT_DIR"; break;
      case OUTPUT_DIRECTORY: tag = "OUTPUT_DIRECTORY"; break;
      case STATUS_BLOCK: tag = "STATUS_BLOCK"; break;
      case PARAM_BLOCK: tag = "PARAM_BLOCK"; break;
      case OUTPUT_FILE: tag = "OUTPUT_FILE"; break;
      case OUTPUT_PERF: tag = "OUTPUT_PERF"; break;
      case OUTPUT_CATEGORIZATION: tag = "OUTPUT_CATEGORIZATION"; break;
      case OUTPUT_EVALUATION: tag = "OUTPUT_EVALUATION"; break;
      case OUTPUT_VIDEO: tag = "OUTPUT_VIDEO"; break;
      case OUTPUT_PS: tag = "OUTPUT_PS"; break;
      case OUTPUT_VRML: tag = "OUTPUT_VRML"; break;
      case OUTPUT_SVG: tag = "OUTPUT_SVG"; break;
      case OUTPUT: tag = "OUTPUT"; break;
      default: tag = ""; break;
    }
    return tag;
  }
};

#endif  //_DBORL_PARAMETER_H
