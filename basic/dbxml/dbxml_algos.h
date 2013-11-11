// This is dbxml/dbxml_algos.h
#ifndef dbxml_algos_h_
#define dbxml_algos_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A collection of functions for finding elements in xml file
// \author Nhon Trinh
// \date July 15, 2009
//
// \verbatim
// Modifications
// \endverbatim

#include <vcl_string.h>
#include <bxml/bxml_document.h>


//==============================================================================
// dbxml_algos
//==============================================================================

//: A collection of functions to find elements in an xml document
class dbxml_algos
{
public:
  //: Append a (child) element to a (parent) element
  // Return a pointer to the added element
  // Typical use case: bmxl_element* x = elm.append_element(elm, new bxml_element("abs"));
  static bxml_element* append_element(bxml_element* parent, bxml_element* child);

  //: Find the first element that matches the name
  static bxml_element* find_by_name(const bxml_data_sptr& head, 
    const vcl_string& query_name);

  //: Get all element of a certain name
  // IMPORTANT: Head ptr must be immediate parent of element with query name
  // Returns all nodes at a certain depth
  static void find_all_elems_by_name(const bxml_data_sptr& head,
                                     const vcl_string& query_name,
                                     vcl_vector<bxml_data_sptr>& data);


  //: Cast to an bxml element. Return 0 if the casting fails
  // This is a convenient way to check the type of bxml_data object
  static bxml_element* cast_to_element(const bxml_data_sptr& data);


  //: Cast to an bxml element of a specified type
  // Return a null pointer if the casting fails
  // This is a convenient way to check the type of bxml_data object
  static bxml_element* cast_to_element(const bxml_data_sptr& data, 
    const vcl_string& element_name);

  //: Get the first text data of the element (if there is one)
  static bool get_first_text(const bxml_element* elm, vcl_string& text);

private:
  dbxml_algos(){};
};









#endif // dbmxl_find_h_
