//:
// \file
// \brief 
// \author Based on original code by      Ozge Can Ozcanli (@lems.brown.edu)
// \date        10/03/07
#include "borld_image_bbox_description.h"

#include <vsol/vsol_box_2d.h>

#include <iostream>

void borld_image_bbox_description::add_box(std::string cat, vsol_box_2d_sptr b)
{
  std::map<std::string, std::vector<vsol_box_2d_sptr> >::iterator iter = data_.find(cat);
  if (iter == data_.end()) {
    std::vector<vsol_box_2d_sptr> tmp(1, b);
    data_[cat] = tmp;
  } else {
    (iter->second).push_back(b);
  }
}

//: CAUTION: assumes that cat exists!! check with category_exists() before using
std::vector<vsol_box_2d_sptr>& borld_image_bbox_description::get_box_vector(std::string cat)
{
  std::map<std::string, std::vector<vsol_box_2d_sptr> >::iterator iter = data_.find(cat);
  return iter->second;
}

unsigned borld_image_bbox_description::version()
{
  return 0;
}

void borld_image_bbox_description::b_read()
{
  std::cout << "IMPLEMENT: borld_image_bbox_description::b_read()\n";
}

void borld_image_bbox_description::b_write()
{
  std::cout << "IMPLEMENT: borld_image_bbox_description::b_write()\n";
}

void borld_image_bbox_description::write_xml(std::ostream& os)
{
  for (std::map<std::string, std::vector<vsol_box_2d_sptr> >::iterator iter = data_.begin(); iter != data_.end(); iter++) {
    for (unsigned i = 0; i < (iter->second).size(); i++) {
      os << "\t\t<instance>\n";
      os << "\t\t\t<category>" << iter->first << "</category>\n";
      os << "\t\t\t<bndbox>\n";
      vsol_box_2d_sptr box = (iter->second)[i];
      os << "\t\t\t\t<xmin>" << box->get_min_x() << "</xmin>\n";
      os << "\t\t\t\t<ymin>" << box->get_min_y() << "</ymin>\n";
      os << "\t\t\t\t<xmax>" << box->get_max_x() << "</xmax>\n";
      os << "\t\t\t\t<ymax>" << box->get_max_y() << "</ymax>\n";
      os << "\t\t\t</bndbox>\n";
      os << "\t\t</instance>\n";
    }
  }
}

