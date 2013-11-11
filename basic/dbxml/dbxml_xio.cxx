// This is dbxml/dbxml_xio.txx

//:
// \file


#include "dbxml_xio.h"
#include <dbxml/dbxml_algos.h>

#include <bxml/bxml_document.h>


//==============================================================================
// Create new xml element
// The caller is responsible for deleting the created objects
//==============================================================================


//------------------------------------------------------------------------------
//:
bxml_element* xml_new(const vcl_string& name, double data)
{
  bxml_element* new_element = new bxml_element(name);
  new_element->set_attribute("type", "double");

  vcl_stringstream strm;
  strm << data;
  new_element->append_text(strm.str());
  return new_element;
}

//------------------------------------------------------------------------------
//:
bxml_element* xml_new(const vcl_string& name, float data)
{
  bxml_element* new_element = new bxml_element(name);
  new_element->set_attribute("type", "float");
  vcl_stringstream strm;
  strm << data;
  new_element->append_text(strm.str());
  return new_element;
}

//------------------------------------------------------------------------------
//:
bxml_element* xml_new(const vcl_string& name, int data)
{
  bxml_element* new_element = new bxml_element(name);
  new_element->set_attribute("type", "int");

  vcl_stringstream strm;
  strm << data;
  new_element->append_text(strm.str());
  return new_element;
}


//------------------------------------------------------------------------------
//:
bxml_element* xml_new(const vcl_string& name, unsigned data)
{
  bxml_element* new_element = new bxml_element(name);
  new_element->set_attribute("type", "unsigned");

  vcl_stringstream strm;
  strm << data;
  new_element->append_text(strm.str());
  return new_element;
}


//------------------------------------------------------------------------------
//:
bxml_element* xml_new(const vcl_string& name, const vcl_string& data)
{
  bxml_element* new_element = new bxml_element(name);
  new_element->set_attribute("type", "string");

  vcl_stringstream strm;
  strm << data;
  new_element->append_text(strm.str());
  return new_element;
}



//==============================================================================
//: Parse bxml_element to various data type
// No check for name of element
//==============================================================================


//------------------------------------------------------------------------------
//:
bool xml_parse(const bxml_element* element, double& data)
{
  vcl_string text = "";
  if (!dbxml_algos::get_first_text(element, text))
    return false;

  vcl_istringstream strm(text);
  strm >> data;
  return true;
}


//------------------------------------------------------------------------------
//:
bool xml_parse(const bxml_element* element, float& data)
{
  vcl_string text = "";
  if (!dbxml_algos::get_first_text(element, text))
    return false;

  vcl_istringstream strm(text);
  strm >> data;
  return true;
}

//------------------------------------------------------------------------------
//:
bool xml_parse(const bxml_element* element, int& data)
{
  vcl_string text = "";
  if (!dbxml_algos::get_first_text(element, text))
    return false;

  vcl_istringstream strm(text);
  strm >> data;
  return true;
}

//------------------------------------------------------------------------------
//:
bool xml_parse(const bxml_element* element, unsigned& data)
{
  vcl_string text = "";
  if (!dbxml_algos::get_first_text(element, text))
    return false;

  vcl_istringstream strm(text);
  strm >> data;
  return true;
}


//------------------------------------------------------------------------------
//:
bool xml_parse(const bxml_element* element, vcl_string& data)
{
  vcl_string text = "";
  if (!dbxml_algos::get_first_text(element, text))
    return false;

  data = text;
  return true;
}




