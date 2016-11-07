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

#include "dborl_index_node.h"

#include <dborl/dborl_object_base.h>

//: return false if not all of the names in the name list are matched with a pointer
bool dborl_index_node::fill_in_the_pointers(vcl_vector<dborl_object_base_sptr>& objects)
{
  obj_list_.clear();
  obj_list_.assign(name_list_.size(), 0);

  for (unsigned j = 0; j < objects.size(); j++) {
    for (unsigned i = 0; i < name_list_.size(); i++) {

      if (objects[j]->name_.compare(name_list_[i]) == 0) 
        obj_list_[i] = objects[j];
      
    }
  }   

  for(unsigned i = 0; i < obj_list_.size(); i++) 
    if (!obj_list_[i]) {
      obj_list_.clear();
      return false;
    }
  
  return true;
}

//: add path/<name>/ as the path of each object in the name list
//  assuming a separate directory is created for each object with name <name>
void dborl_index_node::fill_in_the_paths_using_names(vcl_string path)
{
  path_list_.clear();
  for (unsigned i = 0; i < name_list_.size(); i++) {
#ifdef VCL_WIN32
    path_list_.push_back(path + "\\" + name_list_[i] + "\\");
#else
    path_list_.push_back(path + "/" + name_list_[i] + "/");
#endif
  }
}

void dborl_index_node::b_read()
{

}

void dborl_index_node::b_write()
{

}

//: not opening or closing the node tag!!!\n";
void dborl_index_node::write_xml(vcl_ostream& os)
{
  os << "\t<name>" << name_ << "</name>\n";
  os << "\t<objects>\n";
  for (unsigned i = 0; i <name_list_.size(); i++) {
    os << "\t\t<object name=\"" << name_list_[i] << "\"";
    if (path_list_.size() > i)
      os << " path = \"" << path_list_[i] << "\"";
    os << "></object>\n";
  }
  os << "\t</objects>\n";
}

