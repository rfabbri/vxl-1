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
#include <vcl_string.h> 
//#include <vcl_stdlib.h>
#include <dbxml/dbxml_dom.h>
#include <vcl_iostream.h>


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


vcl_string dbxml_input_converter::get_DOM_tag(DOMNode * node)
{
  int node_type = node->getNodeType();
  if (node_type != DOMNode::ELEMENT_NODE) {
    vcl_cout << "In bxml_input_converter::get_DOM_tag: node_type="
             << node_type << vcl_endl;
    return "";
  }
  char *tag_name = XMLString::transcode(node->getNodeName());
  //char* tag_name = node->getNodeName().transcode();
  vcl_string tname(tag_name);
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
      vcl_cout << "dbxml_input_converter::check_tag()  node_type="
               << node_type << vcl_endl;
    return 0;
  }

  char *tag_name = XMLString::transcode(node->getNodeName());
  vcl_string tname(tag_name);
  // virtual method get_tag_name()
  if (level == 0){
    if (debug_)
      vcl_cout << "dbxml_input_converter::check_tag() tag_name from DOM: "
               << tname << ", tag_name expected: " << get_class_name()
               << vcl_endl;

    if (tname == get_class_name())
      return(1);
  }
  if (level == 1){
    if (debug_)
      vcl_cout << "dbxml_input_converter::check_tag() tag_name from DOM: "
               << tname << ", tag_name expected: " << get_tag_name_1()
               << vcl_endl;

    if (tname == get_tag_name_1())
      return 1;
  }
  else{ 
    if (debug_)
      vcl_cout << "dbxml_input_converter::check_tag() tag_name from DOM: "
               << tname << ", tag_name expected: " << get_tag_name_2()
               << vcl_endl;

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
    vcl_cout<< "dbxml_input_converter::check_tag() tag not found\n";
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


bool dbxml_input_converter::has_attr(DOMNode *node, vcl_string attr_name)
{
  XMLCh *attr=XMLString::transcode(attr_name.c_str());

  const XMLCh *thisattr = ((DOMElement *)node)->getAttribute(attr);
  return thisattr != NULL;
}


vcl_string dbxml_input_converter::get_string_attr(DOMNode *node, vcl_string attr_name)
{
  //XMLString::transcode(attr_name.c_str(), tempStr, 99);

  XMLCh *attr=XMLString::transcode(attr_name.c_str());
  const XMLCh *sx  = ((DOMElement *)node)->getAttribute(attr);
  XMLString::release(&attr);
  char* cx = XMLString::transcode(sx); //transcode does new
  vcl_string str_x(cx);
  XMLString::release(&cx);
  return str_x;
}


int dbxml_input_converter::get_int_attr(DOMNode *node,vcl_string attr_name)
{
  vcl_string cx = get_string_attr(node,attr_name) ;
  int x = atoi(cx.c_str());
  return x;
}


float dbxml_input_converter::get_float_attr(DOMNode *node,vcl_string attr_name)
{
  vcl_string cx = get_string_attr(node,attr_name) ;
  float x = atof(cx.c_str());
  return x;
}


bool dbxml_input_converter::get_bool_attr(DOMNode *node,vcl_string attr_name)
{
  vcl_string cx = get_string_attr(node,attr_name) ;
  bool x = ( (cx == "true") || (cx == "1") || (cx == "on") );
  return x;
}
