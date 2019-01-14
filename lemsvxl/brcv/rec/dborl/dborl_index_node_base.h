//:
// \file
// \brief  class that implements   for ORL 
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 17/03/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

#if !defined(_DBORL_INDEX_NODE_BASE_H)
#define _DBORL_INDEX_NODE_BASE_H

#include <string>
#include <bgrld/bgrld_vertex.h>

class dborl_index_node;
class dborl_index_leaf;

class dborl_index_edge;

class dborl_index_node_base : public bgrld_vertex<dborl_index_edge>
{
public:
  std::string name_;

  dborl_index_node_base() {};
  dborl_index_node_base(std::string name) : name_(name) {}

  virtual bool is_leaf() = 0;

  void set_name(std::string n) { name_ = n; }

  virtual void b_read() = 0;
  virtual void b_write() = 0;
  virtual void write_xml(std::ostream& os) = 0;

  virtual dborl_index_node_base* cast_to_index_node_base() { return this; }
  virtual dborl_index_node* cast_to_index_node() = 0;
  virtual dborl_index_leaf* cast_to_index_leaf() = 0;
};

#endif  //_DBORL_INDEX_NODE_BASE_H
