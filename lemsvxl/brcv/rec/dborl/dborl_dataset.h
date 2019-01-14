//:
// \file
// \brief Class that holds a dataset of objects
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/04/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//
#if !defined(_DBORL_DATASET_H)
#define _DBORL_DATASET_H

#include <dborl/dborl_object_base_sptr.h>
#include <vbl/vbl_ref_count.h>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <dborl/dborl_index_sptr.h>

class dborl_dataset : public vbl_ref_count
{
public:
  std::string name_;
  std::vector<dborl_object_base_sptr> objects_;

  unsigned size() { return objects_.size(); }
  void add_object(dborl_object_base_sptr obj);
  void add_objects(std::vector<dborl_object_base_sptr>& objs);

  void get_category_cnts(std::map<std::string, int>& cnts);
  void get_category_assignments(std::map<std::string, std::vector<dborl_object_base_sptr> >& assign);

  //: assumes each object has a separate folder with its name under the base_dir and creates paths for objects accordingly
  //  CAUTION: pass base_dir WITH the appropriate file separater, path is created as base_dir + object->name_, i.e. no separater added 
  //  pass base_dir as "" if no need for paths in the index root node
  dborl_index_sptr create_flat_index(std::string base_dir = "");
};

#endif  //_DBORL_DATASET_H
