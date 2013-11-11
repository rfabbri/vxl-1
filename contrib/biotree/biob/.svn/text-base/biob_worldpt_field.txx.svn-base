#ifndef biob_worldpt_field_txx_
#define biob_worldpt_field_txx_

#include "biob_worldpt_field.h"
#include <biob/biob_grid_worldpt_roster.h>
#include <vsl/vsl_basic_xml_element.h>

template <class T>
void x_write(vcl_ostream& os, biob_worldpt_field<T> f)
{
  
  vsl_basic_xml_element element("biob_worldpt_field");
  element.x_write_open(os);
  
  // write the roster
  f.roster()->x_write_this(os);
  
  // write the values
  vcl_vector<T> values = f.values();
  vsl_basic_xml_element values_elm("biob_worldpt_field_values");
  values_elm.add_attribute("size", (int) values.size());
  values_elm.x_write_open(os);
  for (unsigned int i=0; i < values.size(); i++) {
    x_write(os, values[i]);
  }
  values_elm.x_write_close(os);
  element.x_write_close(os);
}

// Code for easy instantiation.
#undef BIOB_WORLDPT_FIELD_INSTANTIATE
#define BIOB_WORLDPT_FIELD_INSTANTIATE(T) \
template class biob_worldpt_field<T>;\
template void x_write(vcl_ostream&, biob_worldpt_field<T>)

#endif
