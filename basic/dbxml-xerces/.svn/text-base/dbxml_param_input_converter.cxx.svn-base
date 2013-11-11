//
// This is basic/dbxml/dbxml_param_input_converter.cxx
//:
// \file
///

#include "dbxml_io.h"
#include "dbxml_input_converter.h"
#include <string.h>
#include <stdlib.h>
#include <vcl_iostream.h> //cout
#include "dbxml_param_input_converter.h"
#include "dbxml_param.h"
 


dbxml_param_input_converter::dbxml_param_input_converter() {
  class_name_ = "vxl";
  tag_name_1_ = "params";
  tag_name_2_ = "param";
}

dbxml_param_input_converter::~dbxml_param_input_converter() {
}



bool dbxml_param_input_converter::extract_object_atrs(DOMNode *node) {
  command_ = get_string_attr(node,"command");
  desc_ = get_string_attr(node,"description");
  type_ = get_string_attr(node,"type");
  value_ = get_string_attr(node,"value");
/*    if (strcmp(type.c_str(),"float") ==0)
  float value_ = get_float_attr(node,"value");
  else if (strcmp(type.c_str(),"integer")==0)
  int value = get_int_attr(node,"value");
  else if (strcmp(type.c_str(),"flag")==0){
      vcl_string stringvalue = get_string_attr(node,"value");
        if (strcmp(stringvalue.c_str(), "on")== 0)
      bool value = true;
    else
      bool value = false;
      }
  else
  vcl_string value = get_string_attr(node,"value");
*/
  return true;
}

bool dbxml_param_input_converter::extract_from_dom_1(DOMNode *node) {
  new_or_ref = check_tag(node,1);

  if (new_or_ref == 0) {
    vcl_cout << "dbxml_param_input_converter:: Error, bad tag\n";
    return false;
  }

  return true;
}
bool dbxml_param_input_converter::extract_from_dom_2(DOMNode *node) {
  new_or_ref = check_tag(node,2);

  if (new_or_ref == 0) {
    vcl_cout << "dbxml_param_input_converter:: Error, bad tag\n";
    return false;
  }
     extract_object_atrs(node);

  return true;
}
bool dbxml_param_input_converter::extract_from_dom_3(DOMNode *node) {
  return true;
}

//:
// Construct the object if it is not already in the object table
// It will be in the object table if it has already been constructed
// and the current occurrence is just a ref in the xml file.
// WARNING!!
// There is an issue with the use of the generic pointer, since we
// have no way to reference count the pointer. We might be able to
// deal with the problem in the destructor of the generic pointer

dbxml_generic_ptr dbxml_param_input_converter::construct_object_1(vcl_vector<dbxml_generic_ptr>& objs)
{
  return NULL;
}
dbxml_generic_ptr dbxml_param_input_converter::construct_object_3(vcl_vector<dbxml_generic_ptr>& objs)
{
  return NULL;
}

dbxml_generic_ptr dbxml_param_input_converter::construct_object_2(vcl_vector<dbxml_generic_ptr>& objs)
{
  if (new_or_ref != 0) {
        dbxml_param *p = new dbxml_param(command_,desc_,type_,value_);
    dbxml_generic_ptr gp(p);
    objs.push_back(p);
 
        /*
    if ( !(id_ == null_id_) ) {
     obj_table_[id_] = gp;
     p->ref();//Keep the point alive until it is used
    }
  */
return(p);
 }
  return NULL;
}
