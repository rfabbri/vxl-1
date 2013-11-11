//:
// \file
// \brief  Leaf node class for the index 
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 17/03/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

#if !defined(_DBORL_INDEX_LEAF_H)
#define _DBORL_INDEX_LEAF_H

#include "dborl_index_node_base.h"

class dborl_index_leaf : public dborl_index_node_base
{
public:
  //float certainty_;
  dborl_index_leaf() : dborl_index_node_base() {};
  dborl_index_leaf(vcl_string name) : dborl_index_node_base(name) {}

  virtual bool is_leaf() { return true; }

  virtual void b_read();
  virtual void b_write();
  virtual void write_xml(vcl_ostream& os);
  //void set_certainty(float c);
  //float get_certainty();

  virtual dborl_index_node* cast_to_index_node() { return 0; }
  virtual dborl_index_leaf* cast_to_index_leaf() { return this; }
};

#endif  //_DBORL_INDEX_LEAF_H
