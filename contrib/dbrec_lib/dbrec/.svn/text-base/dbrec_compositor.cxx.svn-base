//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/27/09
//
//
#include "dbrec_compositor.h"

#include <bxml/bxml_find.h>

//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_compositor::xml_element() const
{
  bxml_element* data = new bxml_element("compositor");
  data->append_text("\n ");
  return data;
}

//: read self from the bxml_data_sptr
dbrec_compositor_sptr dbrec_compositor::xml_parse_element(bxml_data_sptr data)
{
  dbrec_compositor_sptr comp;
  bxml_element query("compositor");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  if (!base_root)
    return comp;
  comp = new dbrec_compositor;
  return comp;
}

