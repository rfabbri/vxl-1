//:
// \file
// \brief Base Class for Object Descriptions
//        Ground Truth files are generated using these classes,
//        If ground truth is not available for an object, then the file should be empty but appropriately formatted 
//
// \author Based on original code by  Ozge C Ozcanli (@lems.brown.edu)
// \date 10/03/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#if !defined(_DBORL_DESCRIPTION_BASE_H)
#define _DBORL_DESCRIPTION_BASE_H

#include <map>
#include <string>
#include <vbl/vbl_ref_count.h>
#include <iostream>

class borld_object_type
{
public:
  enum type {
    image,
    point_cloud,
    vehicle_track,
    video
  };
};

class borld_image_description;

class borld_description_base : public vbl_ref_count
{
public:
  std::map<std::string, int> category_list_;
  virtual void b_read() = 0;
  virtual void b_write() = 0;

  virtual void write_xml(std::ostream& os) = 0;
  
  virtual unsigned get_object_type() = 0;

  void add_category(std::string cat) { category_list_[cat] = 1; }
  void add_to_category_cnt(std::string cat, int cnt) { category_list_[cat] = category_list_[cat] + cnt; }
  bool category_exists(std::string cat) { return category_list_.find(cat) != category_list_.end(); }

  inline bool has_single_category() { return category_list_.size() == 1; }
  inline std::string get_first_category() { return category_list_.begin()->first; }
  inline int get_category_cnt(std::string cat) { return category_list_[cat]; }  // be careful if this entry does not exist, then adds it to the map

  virtual borld_description_base* cast_to_description_base() { return this; }
  virtual borld_image_description* cast_to_image_description() = 0;
};

#endif  //_DBORL_DESCRIPTION_BASE_H
