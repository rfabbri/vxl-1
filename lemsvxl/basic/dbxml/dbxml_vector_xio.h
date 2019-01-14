// This is dbxml/dbxml_vector_xio.h
#ifndef dbxml_vector_xio_h_
#define dbxml_vector_xio_h_

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//:
// \file
// \brief Functions to create new bxml_element from a std::vector
// \author Nhon Trinh
// \date August 9, 2009  
//
// \verbatim
// Modifications
// \endverbatim


#include <bxml/bxml_document.h>
#include <vector>
#include <string>

//==============================================================================
//: CREAT NEW XML ELEMENT
// The caller is responsible for deleting the created objects
//==============================================================================

//: create new xml element for a std::vector (simple types only)
// Example of output:
// <x type="vector+double" size="3">10 12 0.4</x>
template<class T>
bxml_element* xml_new(const std::string& name, const std::vector<T>& data);


//==============================================================================
//: PARSE XML ELEMENT
// No check for name or type of element
//==============================================================================

//: parse an xml element to a std::vector
template<class T>
bool xml_parse(const bxml_element* element, std::vector<T>& data);







#endif // dbxml_vector_xio_h_
