// This is basic/dbxml/dbxml_input_converter.cxx
//:
// \file
//
// This file was taken from code in brl/bbas/bxml directory and modifed to 
// support the 2.5.0 xerces lib.  T Orechia. 4/16/2004
//
//
// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "dbxml_input_converter.h"
#include <string> 
//#include <vcl_stdlib.h>
#include <dbxml/dbxml_dom.h>
#include <iostream>


//: Constructor
dbxml_input_converter::dbxml_input_converter()
{
  debug_=false;
  null_id_ = "p00";
}


//: Destructor
dbxml_input_converter::~dbxml_input_converter()
{
}


std::string dbxml_input_converter::get_DOM_tag(DOMNode * node)
{
  int node_type = node->getNodeType();
  if (node_type != DOMNode::ELEMENT_NODE) {
    std::cout << "In bxml_input_converter::get_DOM_tag: node_type="
             << node_type << std::endl;
    return "";
  }
  char *tag_name = XMLString::transcode(node->getNodeName());
  //char* tag_name = node->getNodeName().transcode();
  std::string tname(tag_name);
  return tname;
}


int dbxml_input_converter::check_tag(DOMNode *node)
{
    return(check_tag(node,0));
}


int dbxml_input_converter::check_tag(DOMNode *node,int level)
{
  int node_type = node->getNodeType();
  if (node_type != DOMNode::ELEMENT_NODE) {
    if (debug_)
      std::cout << "dbxml_input_converter::check_tag()  node_type="
               << node_type << std::endl;
    return 0;
  }

  char *tag_name = XMLString::transcode(node->getNodeName());
  std::string tname(tag_name);
  // virtual method get_tag_name()
  if (level == 0){
    if (debug_)
      std::cout << "dbxml_input_converter::check_tag() tag_name from DOM: "
               << tname << ", tag_name expected: " << get_class_name()
               << std::endl;

    if (tname == get_class_name())
      return(1);
  }
  if (level == 1){
    if (debug_)
      std::cout << "dbxml_input_converter::check_tag() tag_name from DOM: "
               << tname << ", tag_name expected: " << get_tag_name_1()
               << std::endl;

    if (tname == get_tag_name_1())
      return 1;
  }
  else{ 
    if (debug_)
      std::cout << "dbxml_input_converter::check_tag() tag_name from DOM: "
               << tname << ", tag_name expected: " << get_tag_name_2()
               << std::endl;

    if ( tname == get_tag_name_2() ) {
      return 2;
    }
    else if ( tname == get_tag_name_3() )
      return 3;
 
    //else if ( tname == get_ref_tag_name() ) {
    // return 2;
    //}
  }
    //not found
  if (debug_)
    std::cout<< "dbxml_input_converter::check_tag() tag not found\n";
  return 0;
}



bool dbxml_input_converter::getNextElementSibling(DOMNode* node)
{
  node = node->getNextSibling();

  bool found = false;
  while (!found && node != 0) {
    int node_type = node->getNodeType();
    if (node_type == DOMNode::ELEMENT_NODE) {
      found = true;
    }
    else {
      node = node->getNextSibling();
    }
  }
  return found;
}


bool dbxml_input_converter::has_attr(DOMNode *node, std::string attr_name)
{
  XMLCh *attr=XMLString::transcode(attr_name.c_str());

  const XMLCh *thisattr = ((DOMElement *)node)->getAttribute(attr);
  return thisattr != NULL;
}


std::string dbxml_input_converter::get_string_attr(DOMNode *node, std::string attr_name)
{
  //XMLString::transcode(attr_name.c_str(), tempStr, 99);

  XMLCh *attr=XMLString::transcode(attr_name.c_str());
  const XMLCh *sx  = ((DOMElement *)node)->getAttribute(attr);
  XMLString::release(&attr);
  char* cx = XMLString::transcode(sx); //transcode does new
  std::string str_x(cx);
  XMLString::release(&cx);
  return str_x;
}


int dbxml_input_converter::get_int_attr(DOMNode *node,std::string attr_name)
{
  std::string cx = get_string_attr(node,attr_name) ;
  int x = atoi(cx.c_str());
  return x;
}


float dbxml_input_converter::get_float_attr(DOMNode *node,std::string attr_name)
{
  std::string cx = get_string_attr(node,attr_name) ;
  float x = atof(cx.c_str());
  return x;
}


bool dbxml_input_converter::get_bool_attr(DOMNode *node,std::string attr_name)
{
  std::string cx = get_string_attr(node,attr_name) ;
  bool x = ( (cx == "true") || (cx == "1") || (cx == "on") );
  return x;
}
