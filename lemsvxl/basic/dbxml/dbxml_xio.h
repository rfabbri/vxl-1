// This is dbxml/dbxml_xio.h
#ifndef dbxml_xio_h_
#define dbxml_xio_h_

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//:
// \file
// \brief Functions to create new bxml_element from various data types
// \author Nhon Trinh
// \date August 9, 2009  
//
// \verbatim
// Modifications
// \endverbatim

#include <string>

class bxml_element;


//==============================================================================
//: Create new bxml_element from various data type
// The caller is responsible for deleting the created objects
bxml_element* xml_new(const std::string& name, double data);
bxml_element* xml_new(const std::string& name, float data);
bxml_element* xml_new(const std::string& name, int data);
bxml_element* xml_new(const std::string& name, unsigned data);
bxml_element* xml_new(const std::string& name, const std::string& data);


//==============================================================================
//: Parse bxml_element to various data type
// No check for name of element
bool xml_parse(const bxml_element* element, double& data);
bool xml_parse(const bxml_element* element, float& data);
bool xml_parse(const bxml_element* element, int& data);
bool xml_parse(const bxml_element* element, unsigned& data);
bool xml_parse(const bxml_element* element, std::string& data);


#endif // dbxml_xio_h_
