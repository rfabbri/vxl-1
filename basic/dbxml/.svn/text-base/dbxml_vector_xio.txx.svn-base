// This is dbxml/dbxml_vector_xio.txx

#ifndef dbxml_vector_xio_txx_
#define dbxml_vector_xio_txx_


//:
// \file


#include "dbxml_vector_xio.h"
#include <dbxml/dbxml_algos.h>


//==============================================================================
//: CREAT NEW XML ELEMENT
// The caller is responsible for deleting the created objects
//==============================================================================


//------------------------------------------------------------------------------
//: create new xml element for a vcl_vector (simple types only)
template<class T>
bxml_element* xml_new(const vcl_string& name, const vcl_vector<T>& data)
{
  bxml_element* new_element = new bxml_element(name);

  // vcl_string DATA_TYPE must be defined in the implementation file
  const vcl_string type = DATA_TYPE;
  new_element->set_attribute("type", type);

  unsigned size = data.size();
  new_element->set_attribute("size", size);

  const vcl_string delimiter = " ";
  
  vcl_stringstream strm;
  if (size > 0)
    strm << data[0];
  for (unsigned i =1; i < size; ++i)
  {
    strm << delimiter << data[i];
  }
  new_element->append_text(strm.str());
  return new_element;
};




//==============================================================================
//: PARSE XML ELEMENT
// No check for name of element
//==============================================================================

//: parse an xml element to a vcl_vector
template<class T>
bool xml_parse(const bxml_element* element, vcl_vector< T >& data)
{
  data.clear();

  vcl_string text = "";
  if (!dbxml_algos::get_first_text(element, text))
    return false;

  // use a stringstream as a mediator
  vcl_istringstream strm(text);

  // allocate memory
  unsigned size = 0;

  // known size
  if (element->get_attribute("size", size))
  {
    data.resize(size);
    for (unsigned i =0; i < size; ++i)
    {
      if (! (strm >> data[i]))
      {
        return false;
      }
    }
  }
  else // if size is unknown, keep reading till the end of the string
  {
    T value;
    while ( strm >> value ) 
    {
      data.push_back(value);
    }
  }
  return true;
};




//===========================================================================================

//: Macro to instantiate xio for vcl_vector<T>
#define DBXML_VECTOR_XIO_INSTANTIATE(T) \
template bxml_element* xml_new(const vcl_string& name, const vcl_vector<T>& data); \
template bool          xml_parse(const bxml_element* element, vcl_vector< T >& data);
#endif // dbxml_vector_xio_txx_






