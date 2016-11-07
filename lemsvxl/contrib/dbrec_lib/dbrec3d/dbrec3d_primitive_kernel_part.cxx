//:
// \file
// \author Isabel Restrepo
// \date 24-Aug-2010

#include "dbrec3d_primitive_kernel_part.h"
#include "dbrec3d_visitor.h"

#include <bxml/bxml_find.h>

void dbrec3d_primitive_kernel_part::accept(dbrec3d_visitor *v)
{
  v->visit(this);
}


bxml_data_sptr dbrec3d_primitive_kernel_part::xml_element() const
{
  bxml_element* data = new bxml_element("dbrec3d_primitive_kernel_part");
  data->append_text("\n");
  data->set_attribute("type_id", this->type_id_);
  data->set_attribute("class_prior", class_prior_);
  bxml_data_sptr kernel_data = kernel_->xml_element();
  data->append_data(kernel_data);
  data->append_text("\n");
  return data;
}

dbrec3d_part_sptr dbrec3d_primitive_kernel_part::parse_xml_element(bxml_data_sptr d)
{
  bxml_element query("dbrec3d_primitive_kernel_part");
  bxml_data_sptr root = bxml_find_by_name(d, query);
  if (!root || root->type() != bxml_data::ELEMENT) {
    return NULL;
  }
  bxml_element* gp_root = dynamic_cast<bxml_element*>(root.ptr());
  
  //get the variables
  int type_id = -1; float class_prior= 0.0f; 
  gp_root->get_attribute("type_id", type_id );
  gp_root->get_attribute("class_prior", class_prior);
  
  bvpl_kernel_sptr kernel = bvpl_kernel::parse_xml_element(d);
  
  //create the part
  return new dbrec3d_primitive_kernel_part(type_id, class_prior, kernel, true, false);
}