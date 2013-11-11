//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/04/07
#include "dborl_dataset.h"
#include <dborl/dborl_object_base.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dborl/dborl_index_leaf.h>
#include <dborl/dborl_index_leaf_sptr.h>

void dborl_dataset::add_object(dborl_object_base_sptr obj)
{
  objects_.push_back(obj);
}
void dborl_dataset::add_objects(vcl_vector<dborl_object_base_sptr>& objs)
{
  objects_.insert(objects_.end(), objs.begin(), objs.end());
}
void dborl_dataset::get_category_cnts(vcl_map<vcl_string, int>& cnts)
{
  if (!objects_.size())
    return;

  cnts.clear();

  vcl_map<vcl_string, int>& tmp = objects_[0]->desc_->category_list_;
  for (vcl_map<vcl_string, int>::iterator i = tmp.begin(); i != tmp.end(); i++) {
    cnts[i->first] = i->second;
  }

  for (unsigned i = 1; i < objects_.size(); i++) {
    vcl_map<vcl_string, int>& tmp = objects_[i]->desc_->category_list_;
    for (vcl_map<vcl_string, int>::iterator it = tmp.begin(); it != tmp.end(); it++) {
      vcl_map<vcl_string, int>::iterator cnts_iter = cnts.find(it->first);
      if (cnts_iter == cnts.end())
        cnts[it->first] = it->second;
      else
        cnts_iter->second = cnts_iter->second + it->second;
    }
  }
}

void dborl_dataset::get_category_assignments(vcl_map<vcl_string, vcl_vector<dborl_object_base_sptr> >& assign)
{
  assign.clear();

  for (unsigned i = 0; i < objects_.size(); i++) {
    vcl_map<vcl_string, int>& tmp = objects_[i]->desc_->category_list_;
   
    for (vcl_map<vcl_string, int>::iterator it = tmp.begin(); it != tmp.end(); it++) {
      if (it->second > 0) {
        vcl_map<vcl_string, vcl_vector<dborl_object_base_sptr> >::iterator assign_iter = assign.find(it->first);
        if (assign_iter != assign.end())
          assign_iter->second.push_back(objects_[i]);
        else {
          vcl_vector<dborl_object_base_sptr> tmp(1, objects_[i]);
          assign[it->first] = tmp;
        }
      }
    }
  }

}

dborl_index_sptr dborl_dataset::create_flat_index(vcl_string base_dir)
{
  bool create_paths = true;
  if (base_dir.compare("") == 0)
    create_paths = false;

  dborl_index_node_sptr n = new dborl_index_node("root");
  for (unsigned i = 0; i < objects_.size(); i++) {
    n->add_obj(objects_[i]);
    n->add_name(objects_[i]->name_);
    n->add_path(base_dir + objects_[i]->name_ + "/");
  }

  dborl_index_sptr ind = new dborl_index(name_ + "_flat_index");
  ind->add_root(n->cast_to_index_node_base());

  vcl_map<vcl_string, int> cnts;
  get_category_cnts(cnts);
  for (vcl_map<vcl_string, int>::iterator it = cnts.begin(); it != cnts.end(); it++) {
    dborl_index_leaf_sptr l = new dborl_index_leaf(it->first);
    ind->add_child(n->cast_to_index_node_base(), l->cast_to_index_node_base());
  }
  if (objects_.size() > 0) {
    if (objects_[0]->cast_to_image_object() != 0)
      ind->set_type(dborl_index_type::flat_image);
  }

  return ind;
}


