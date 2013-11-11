//:
// \file
// \brief  Index Node class, the names of the objects are stored and saved/parsed as/from xml documents
//         during categorization, the index should be filled in with pointers to the actual objects having those names,
//         a method ( fill_in_the_pointers() ) is supplied that takes a dataset/a vector of objects and a name-only index and fills in the pointers in the index
//         the assumption is that the dataset contains objects with unique names
//
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 17/03/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

#if !defined(_DBORL_INDEX_NODE_H)
#define _DBORL_INDEX_NODE_H

#include "dborl_index_node_base.h"

#include <vcl_vector.h>
#include <dborl/dborl_object_base_sptr.h>

//:        IMPORTANT NOTE:
//         the path_list_ and obj_list_ may often be empty, however, if they are not, 
//         It is upto the programmer to make sure that the <name, object pointer, path> triplets are in corresponding locations
//         in the vectors of this class

class dborl_index_node : public dborl_index_node_base
{
protected:
  vcl_vector<vcl_string> name_list_;
  vcl_vector<vcl_string> path_list_;
  vcl_vector<dborl_object_base_sptr> obj_list_;  
  
public:
  dborl_index_node() : dborl_index_node_base() {};
  dborl_index_node(vcl_string name) : dborl_index_node_base(name) {}

  virtual bool is_leaf() { return false; }

  void add_name(vcl_string n) { name_list_.push_back(n); }
  void add_names(vcl_vector<vcl_string>& names) { name_list_.insert(name_list_.end(), names.begin(), names.end()); } 

  //: it is upto the user to make sure that the paths and names are inserted in correct order
  void add_path(vcl_string p) { path_list_.push_back(p); }
  void add_paths(vcl_vector<vcl_string>& paths) { path_list_.insert(path_list_.end(), paths.begin(), paths.end()); } 

  //: it is upto the user to make sure that the paths and names are inserted in correct order
  void add_obj(dborl_object_base_sptr o) { obj_list_.push_back(o); }
  void add_objs(vcl_vector<dborl_object_base_sptr>& objs) { obj_list_.insert(obj_list_.end(), objs.begin(), objs.end()); } 

  //: return false if not all of the names in the name list are matched with a pointer
  bool fill_in_the_pointers(vcl_vector<dborl_object_base_sptr>& objects);

  //: add path/<name>/ as the path of each object in the name list
  void fill_in_the_paths_using_names(vcl_string path);

  vcl_vector<dborl_object_base_sptr>& objects(void) { return obj_list_; }
  vcl_vector<vcl_string>& paths(void) { return path_list_; }
  vcl_vector<vcl_string>& names(void) { return name_list_; }

  virtual void b_read();
  virtual void b_write();
  virtual void write_xml(vcl_ostream& os);

  virtual dborl_index_node* cast_to_index_node() { return this; }
  virtual dborl_index_leaf* cast_to_index_leaf() { return 0; }
};

#endif  //_DBORL_INDEX_NODE_H
