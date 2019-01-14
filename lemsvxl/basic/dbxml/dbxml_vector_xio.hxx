// This is dbxml/dbxml_vector_xio.hxx

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
//: create new xml element for a std::vector (simple types only)
template<class T>
bxml_element* xml_new(const std::string& name, const std::vector<T>& data)
{
  bxml_element* new_element = new bxml_element(name);

  // std::string DATA_TYPE must be defined in the implementation file
  const std::string type = DATA_TYPE;
  new_element->set_attribute("type", type);

  unsigned size = data.size();
  new_element->set_attribute("size", size);

  const std::string delimiter = " ";
  
  std::stringstream strm;
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

//: parse an xml element to a std::vector
template<class T>
bool xml_parse(const bxml_element* element, std::vector< T >& data)
{
  data.clear();

  std::string text = "";
  if (!dbxml_algos::get_first_text(element, text))
    return false;

  // use a stringstream as a mediator
  std::istringstream strm(text);

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

//: Macro to instantiate xio for std::vector<T>
#define DBXML_VECTOR_XIO_INSTANTIATE(T) \
template bxml_element* xml_new(const std::string& name, const std::vector<T>& data); \
template bool          xml_parse(const bxml_element* element, std::vector< T >& data);
#endif // dbxml_vector_xio_txx_






