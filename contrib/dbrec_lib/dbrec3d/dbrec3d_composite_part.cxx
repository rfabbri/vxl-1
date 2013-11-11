//:
// \file
// \author Isabel Restrepo
// \date 7-Sep-2010
//#include "dbrec3d_composite_part.h"
//#include <bxml/bxml_find.h>
//#if 0
////: XML read
//template <class T_compositor>
//dbrec3d_part_sptr dbrec3d_composite_part<T_compositor>::parse_xml_element(bxml_data_sptr d)
//{
//  bxml_element query("dbrec3d_composite_part");
//  bxml_data_sptr root = bxml_find_by_name(d, query);
//  if (!root || root->type() != bxml_data::ELEMENT) {
//    return 0;
//  }
//  bxml_element* root_elm = dynamic_cast<bxml_element*>(root.ptr());
//  
//  //get the variables
//  int type_id = -1;
//  float class_prior = -1.0;
//  root_elm->get_attribute("type_id", type_id);
//  root_elm->get_attribute("class_prior", class_prior);
//  
//  compositor_.parse_xml_element(root_elm);
//  set_class_prior(class_prior);
//  reset_type_id(type_id);
//#if 0
//  //get the children id
//  bxml_element query("dbrec3d_contexts");
//  bxml_data_sptr contexts_data = bxml_find_by_name(doc.root_element(), query);
//  bxml_element* contexts_elm = dynamic_cast<bxml_element *> (contexts_data.as_pointer());
//  if (!contexts_elm) {
//    vcl_cout << "dbrec_parse_hierarchy_xml::parse() - could not find the main node with name dbrec3d_contexts\n";
//    return false;
//  }
//  
//  for(bxml_element::const_data_iterator d_it = contexts_elm->data_begin(); d_it!=contexts_elm->data_end(); d_it++)
//  {
//    dbrec3d_context_parse_xml_element(*d_it);
//  }
//  
//  vcl_vector<dbrec_part_sptr> children;  // empty for now
//  
//  dbrec3d_composite_part<dbrec3d_pairwise_compositor<dbrec3d_pairwise_gaussian_model> >* cp = 
//  new dbrec3d_composite_part<dbrec3d_pairwise_compositor<dbrec3d_pairwise_gaussian_model> >(type, children, compositor, radius);
//  cp->set_class_prior(class_prior);
//
//  out_p = cp;
//  return out_p;
//#endif
//  return this;
//}
//#endif


