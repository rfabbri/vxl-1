//:
// \file
// \author Isabel Restrepo
// \date 23-Aug-2010
//
#include "dbrec3d_io_visitors.h"
#include "dbrec3d_pairwise_compositor.h"
#include "dbrec3d_models.h"

#include <bxml/bxml_document.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>


//: XML write utility for dbrec3d
void dbrec3d_xml_write_parts_and_contexts(vcl_string p_doc, vcl_string c_doc)
{
  vcl_vector<dbrec3d_part_sptr> parts = PARTS_MANAGER->get_all_parts();
 
  //write the parsts xml file
  dbrec3d_write_xml_visitor xml_writer;
  xml_writer.write_hierarchy(parts, p_doc);
  
  //write the contexts
  CONTEXT_MANAGER->xml_write(c_doc);
}

//: XML read utility for dbrec3d
void dbrec3d_xml_parse_parts_and_contexts(vcl_string p_doc, vcl_string c_doc)
{

  //parse the parst xml file
  dbrec3d_parse_xml_visitor xml_parser;
  xml_parser.parse_hierarchy(p_doc);
  //write the contexts
  CONTEXT_MANAGER->xml_parse(c_doc);
}

dbrec3d_write_xml_visitor::dbrec3d_write_xml_visitor()
{
  bxml_element * p_data = new bxml_element("parts");
  p_data->append_text("\n");
  part_data_ = p_data;
}

void dbrec3d_write_xml_visitor::write_hierarchy(vcl_vector<dbrec3d_part_sptr> &hierarchy, vcl_string& name)
{

  for(unsigned i=0; i< hierarchy.size(); i++)
  {
    hierarchy[i]->accept(this);
  }

  bxml_document doc;
  bxml_element * root = new bxml_element("hierarchy");
  doc.set_root_element(root);
  root->append_text("\n");
  root->append_data(part_data_);
  root->append_text("\n");
  vcl_ofstream os(name.c_str());
  bxml_write(os,doc);
  os.close();
}


void dbrec3d_write_xml_visitor::visit(dbrec3d_part_sptr part)
{
  
  //: check if the part has been written before
  vcl_set<unsigned>::iterator it = part_set_.find(part->type_id());
  if (it == part_set_.end()) {
    //: just dump the part as a new data node
    bxml_data_sptr data = part->xml_element();
    bxml_element* part_data_ptr = dynamic_cast<bxml_element*>(part_data_.ptr());
    part_data_ptr->append_data(data);
    part_data_ptr->append_text("\n");
    part_set_.insert(part->type_id());
   
  }
}

bool dbrec3d_parse_xml_visitor::parse_hierarchy(const vcl_string& name)
{
  vcl_ifstream is(name.c_str());
  if (!is) return false;
  bxml_document doc = bxml_read(is);
  bxml_element query("hierarchy");
  bxml_data_sptr hierarchy_root = bxml_find_by_name(doc.root_element(), query);
  if (!hierarchy_root) {
    vcl_cout << "dbrec_parse_hierarchy_xml::parse() - could not find the main node with name hierarchy!\n";
    return false;
  }
  bxml_element parts_query("parts");
  bxml_data_sptr parts_data = bxml_find_by_name(hierarchy_root, parts_query);
  bxml_element* parts_elm = dynamic_cast<bxml_element*>(parts_data.ptr());
  if (!parts_data || !parts_elm) {
    vcl_cout << "dbrec_parse_hierarchy_xml::parse() - could not find the main node with name parts!\n";
    return false;
  }

 typedef dbrec3d_pairwise_compositor<dbrec3d_pairwise_gaussian_model> T_comp;
 dbrec3d_part_sptr part=0;
 for (bxml_element::const_data_iterator d_it = parts_elm->data_begin(); d_it != parts_elm->data_end(); d_it++) {
    if ((*d_it)->type() == bxml_data::ELEMENT) {
      //: try to parse it with each part type
      if (part = dbrec3d_primitive_kernel_part::parse_xml_element(*d_it))
        PARTS_MANAGER->register_part(part);
      else if (part = dbrec3d_composite_part<T_comp >::parse_xml_element(*d_it))
        PARTS_MANAGER->register_part(part);
    }
  }

  return true;
}

bool dbrec3d_parse_xml_visitor::parse_composition_structure(bxml_data_sptr d,
                                                            vcl_map<unsigned, vcl_pair<dbrec3d_part_sptr, bool> >& part_map)
{
  bxml_element query("composition");
  bxml_data_sptr root = bxml_find_by_name(d, query);
  if (!root || root->type() != bxml_data::ELEMENT) {
    return false;
  }
  bxml_element* c_root = dynamic_cast<bxml_element*>(root.ptr());
  unsigned type_id, nchildren;
  c_root->get_attribute("type_id", type_id);
  c_root->get_attribute("nchildren", nchildren);
  //: retrieve the part
  vcl_map<unsigned, vcl_pair<dbrec3d_part_sptr, bool> >::iterator it = part_map.find(type_id);
  if (it == part_map.end())
    return false;
  dbrec3d_part_sptr p = it->second.first;
  typedef dbrec3d_pairwise_compositor<dbrec3d_pairwise_gaussian_model> T_comp;
  dbrec3d_composite_part<T_comp >* cp = dynamic_cast<dbrec3d_composite_part<T_comp >*>(p.ptr());
  if (!cp)
    return false;
  if (!nchildren)
    return true;  // no need to parse the rest
  //: read out the children
  for (bxml_element::const_data_iterator s_it = c_root->data_begin(); s_it != c_root->data_end(); s_it++) {
    if ((*s_it)->type() == bxml_data::TEXT) {
      bxml_text* t = dynamic_cast<bxml_text*>((*s_it).ptr());
      vcl_stringstream text_d(t->data()); vcl_string buf;
      vcl_vector<vcl_string> tokens;
      while (text_d >> buf) {
        tokens.push_back(buf);
      }
      if (tokens.size() != nchildren)
        continue;
      for (unsigned i = 0; i < nchildren; i++) {
        vcl_stringstream ss2(tokens[i]); unsigned c_type;
        ss2 >> c_type;
        it = part_map.find(c_type);
        if (it == part_map.end())
          return false;
        //cp->children().push_back(it->second.first);
        it->second.second = true;  // this part has been a child of someone at least once
      }
      break;
    }
  }

  return true;
}

