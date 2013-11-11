//:
// \file
// \brief base class for ORL objects
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/17/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

#include "dborl_index.h"
#include <dborl/dborl_index_edge_sptr.h>
#include <dborl/dborl_index_edge.h>
#include <dborl/dborl_index_leaf.h>
#include <dborl/dborl_index_node.h>

vcl_string dborl_index::get_type_string()
{ 
  switch(type_) {
    case dborl_index_type::flat_image:
      return "flat_image";
    case dborl_index_type::flat_point_cloud:
      return "flat_point_cloud";
    case dborl_index_type::flat_vehicle_track:
      return "flat_vehicle_track";
    case dborl_index_type::image_tree:
      return "image_tree";
    case dborl_index_type::flat_job:
      return "flat_job";
  }
  return "unknown";
}

int dborl_index::set_type_from_string(vcl_string type_str)
{
  if (type_str.compare("flat_image") == 0) {
    type_ = dborl_index_type::flat_image;
    return type_;
  } else if (type_str.compare("flat_point_cloud") == 0) {
    type_ = dborl_index_type::flat_point_cloud;
    return type_;
  } else if (type_str.compare("flat_vehicle_track") == 0) {
    type_ = dborl_index_type::flat_vehicle_track;
    return type_;
  } else if (type_str.compare("image_tree") == 0) {
    type_ = dborl_index_type::image_tree;
    return type_;
  } else if (type_str.compare("flat_job") == 0) {
    type_ = dborl_index_type::flat_job;
    return type_;
  } else {
    return -1;
  }
}

bool dborl_index::add_root(dborl_index_node_base_sptr root)
{
  if (number_of_vertices() != 0)
    return false;

  add_vertex(root);
  root_ = root;
  return true;
}

//: check if the node is in the tree --> a linear search over the existing nodes for now,
//  could be way more efficient if we take advantage of the tree structure, start from the root
//  and search breadth-first and stop when the parent is located,
//  however the implementation is not worthfile for now since the indices are really shallow 
bool dborl_index::node_exists(dborl_index_node_base_sptr node)
{
  bool found_it = false;
  for (vertex_iterator it = vertices_begin(); it != vertices_end(); it++) {
    if (node.ptr() == (*it).ptr()) {
      found_it = true;
      break;
    }
  }
  if (!found_it)
    return false;
  else
    return true;
}

//: does not check whether parent exists before adding the child and the edge, use node_exists() if need to be sure
bool dborl_index::add_child(dborl_index_node_base_sptr parent, dborl_index_node_base_sptr child)
{
  add_vertex(child);

  //set the source and target nodes
  dborl_index_edge_sptr e = new dborl_index_edge(parent, child);

  if (add_edge(e)) {
    //if this is a valid edge, add the links from the nodes to this edge
    parent->add_outgoing_edge(e);
    child->add_incoming_edge(e);
  } else
    return false;

  return true;
}

//: does not check whether parent exists before accessing the outgoing edges to get children, use node_exists() if need to be sure
bool dborl_index::get_children(dborl_index_node_base_sptr parent, vcl_vector<dborl_index_node_base_sptr>& cs)
{
  //vcl_list<dborl_index_edge_sptr>& edges = parent->out_edges();
  if (!parent->out_edges().size())
    return false;

  for (vcl_list<dborl_index_edge_sptr>::const_iterator it = parent->out_edges().begin(); it != parent->out_edges().end(); it++)
    cs.push_back((*it)->target());

  return true;
}

//: does not check whether child exists before accessing the incoming edge to get the parent, assumes only one incoming edge
dborl_index_node_base_sptr dborl_index::get_parent(dborl_index_node_base_sptr child)
{
  if (!child->in_degree())
    return 0;

  //: return the first incoming edge's source
  vcl_list<dborl_index_edge_sptr>::const_iterator it = child->in_edges().begin();
  return (*it)->source();
}
  
void dborl_index::b_read()
{

}

void dborl_index::b_write()
{

}

void dborl_index::write_node_xml(dborl_index_node_sptr n, vcl_ostream& os)
{
  os << "<node>\n";
  n->write_xml(os);
  vcl_vector<dborl_index_node_base_sptr> ch;
  get_children(n->cast_to_index_node_base(), ch);
  for (unsigned i = 0; i < ch.size(); i++) {
    if (ch[i]->cast_to_index_node())
      write_node_xml(ch[i]->cast_to_index_node(), os);
    else if (ch[i]->cast_to_index_leaf())
      ch[i]->cast_to_index_leaf()->write_xml(os);
  }
  os << "</node>\n";
}

void dborl_index::write_xml(vcl_ostream& os)
{
  os << "<file basictype=\"index\" name = \"" << name_ << "\" filetype = \"" << get_type_string() << "\">\n";
  write_node_xml(root_->cast_to_index_node(), os);
  os << "</file>\n";
}

