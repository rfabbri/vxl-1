//:
// \file
// \brief A tree implemented as a directed graph using dbgrl_class,
//        The vertices only have out_edges()
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/17/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

#if !defined(_DBORL_INDEX_H)
#define _DBORL_INDEX_H

class dborl_index_type
{
public:
  enum type {
    flat_image,
    flat_point_cloud,
    flat_vehicle_track,
    image_tree,
    flat_job           // the same as flat_image in terms of format
  };
};

#include <dbgrl/dbgrl_graph.h>
#include <dborl/dborl_index_node_base.h>
#include <dborl/dborl_index_edge.h>
#include <dborl/dborl_index_node_sptr.h>
#include <dborl/dborl_index_node_base_sptr.h>
#include <vcl_vector.h>

class dborl_index : public dbgrl_graph<dborl_index_node_base , dborl_index_edge>
{
public:
  int type_;
  vcl_string name_;
  dborl_index_node_base_sptr root_;

  dborl_index() : name_(""), root_(0) {}
  dborl_index(vcl_string name) : name_(name), root_(0) {}

  void set_type(int t) { type_ = t; }
  int get_type() { return type_; }
  vcl_string get_type_string();
  int set_type_from_string(vcl_string type_str);
  bool is_type(int type) { return type == type_; }

  //: assumes the index is currently empty, if not returns false
  bool add_root(dborl_index_node_base_sptr root);

  //: check if the node is in the tree
  bool node_exists(dborl_index_node_base_sptr root);

  //: does not check whether parent exists before adding the child and the edge, use node_exists() if need to be sure
  bool add_child(dborl_index_node_base_sptr parent, dborl_index_node_base_sptr child);
  
  //: does not check whether parent exists before accessing the outgoing edges to get children, use node_exists() if need to be sure
  bool get_children(dborl_index_node_base_sptr parent, vcl_vector<dborl_index_node_base_sptr>& cs);
  
  //: does not check whether child exists before accessing the incoming edge to get the parent, assumes only one incoming edge
  dborl_index_node_base_sptr get_parent(dborl_index_node_base_sptr child);
  
  void b_read();
  void b_write();
  void write_xml(vcl_ostream& os);
  void write_node_xml(dborl_index_node_sptr n, vcl_ostream& os);
  
};

#endif  //_DBORL_INDEX_H
