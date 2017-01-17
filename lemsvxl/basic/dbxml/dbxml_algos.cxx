// This is dbxml/dbxml_algos.cxx

#include "dbxml_algos.h"


//:
// \file

//==============================================================================
// dbxml_algos
//==============================================================================


//------------------------------------------------------------------------------
//: Append a (child) element to a (parent) element
// Return a pointer to the added element
// Typical use case: bmxl_element* x = elm.append_element(elm, new bxml_element("abs"));
bxml_element* dbxml_algos::
append_element(bxml_element* parent, bxml_element* child)
{
  if (!child) return 0;
  parent->append_data(child);
  return child;
}


//------------------------------------------------------------------------------
//: Find the first element that matches the name
bxml_element* dbxml_algos::
find_by_name(const bxml_data_sptr& head, const vcl_string& query_name)
{
  if (head->type() != bxml_data::ELEMENT)
     return 0;
  bxml_element* h_elm = static_cast<bxml_element*>(head.ptr());
  if (h_elm->name() == query_name)
  {
    return h_elm;
  }
  else
  {
    // recursively check nested elements
    for (bxml_element::const_data_iterator i = h_elm->data_begin();
      i != h_elm->data_end();  ++i)
    {
      bxml_element* result = dbxml_algos::find_by_name(*i, query_name);
      if (result)
        return result;
    }
  }
  return 0;
}


//------------------------------------------------------------------------------
//: Find all the elements of a certain name, and populate vector reference
// Assumption that head is the immediate parent of the element with the
// query_name
void dbxml_algos::
find_all_elems_by_name(const bxml_data_sptr& head,
                       const vcl_string& query_name,
                       vcl_vector<bxml_data_sptr>& data)
{
  // We cant do anything if first element isnt an element type
  if (head->type() != bxml_data::ELEMENT)
  {
    return;
  }

  // Grab first element
  bxml_element* h_elm = static_cast<bxml_element*>(head.ptr());

  // Use iterator to go over all elements
  for (bxml_element::const_data_iterator i = h_elm->data_begin();
    i != h_elm->data_end();  ++i)
  {
    if ( (*i)->type() == bxml_data::ELEMENT )
    {
      bxml_element* data_elm = static_cast<bxml_element*>((*i).ptr());
      if (data_elm->name() == query_name)
      {
        // pushd back into vector
        data.push_back(data_elm);
      }
    } 
  }
}


//------------------------------------------------------------------------------
//: Cast to an bxml element. Return 0 if the casting fails
// This is a convenient way to check the type of bxml_data object
bxml_element* dbxml_algos::
cast_to_element(const bxml_data_sptr& data)
{
  if (data->type() != bxml_data::ELEMENT)
    return 0;
  return static_cast<bxml_element* >(data.as_pointer());
}


//------------------------------------------------------------------------------
//: Cast to an bxml element of a specified type
// Return a null pointer if the casting fails
// This is a convenient way to check the type of bxml_data object
bxml_element* dbxml_algos::
cast_to_element(const bxml_data_sptr& data, const vcl_string& element_name)
{
  if (data->type() != bxml_data::ELEMENT)
    return 0;

  bxml_element* elm = static_cast<bxml_element* >(data.as_pointer());
  return (elm->name() == element_name) ? elm : 0;
}


//------------------------------------------------------------------------------
//: Get the first text data of the element (if there is one)
bool dbxml_algos::
get_first_text(const bxml_element* elm, vcl_string& text)
{
  text = "";
  if (!elm)
    return false;;

  // Find the first child that is a text element
  bxml_text* text_elm = 0;
  for (bxml_element::const_data_iterator iter = elm->data_begin();
    iter != elm->data_end(); ++iter)
  {
    if ((*iter)->type() == bxml_data::TEXT)
    {
      text_elm = static_cast<bxml_text* >(iter->ptr());
    }
  }
  if (!text_elm)
    return false;

  text = text_elm->data();
  return true;
}

