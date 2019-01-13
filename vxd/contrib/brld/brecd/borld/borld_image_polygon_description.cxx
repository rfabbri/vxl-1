//:
// \file
// \brief 
// \author Based on original code by      Ozge Can Ozcanli (@lems.brown.edu)
// \date        10/03/07
#include "borld_image_polygon_description.h"

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>

#include <iostream>

void borld_image_polygon_description::add_polygon(std::string cat, vsol_polygon_2d_sptr p)
{
  std::map<std::string, std::vector<vsol_polygon_2d_sptr> >::iterator iter = data_.find(cat);
  if (iter == data_.end()) {
    std::vector<vsol_polygon_2d_sptr> tmp(1, p);
    data_[cat] = tmp;
  } else {
    (iter->second).push_back(p);
  }
}

std::vector<vsol_polygon_2d_sptr>& borld_image_polygon_description::get_polygon_vector(std::string cat)
{
  std::map<std::string, std::vector<vsol_polygon_2d_sptr> >::iterator iter = data_.find(cat);
  return iter->second;
}

unsigned borld_image_polygon_description::version()
{
  return 0;
}

void borld_image_polygon_description::b_read()
{
  std::cout << "IMPLEMENT: borld_image_polygon_description::b_read()\n";
}

void borld_image_polygon_description::b_write()
{
  std::cout << "IMPLEMENT: borld_image_polygon_description::b_write()\n";
}

void borld_image_polygon_description::write_xml(std::ostream& os)
{
  for (std::map<std::string, std::vector<vsol_polygon_2d_sptr> >::iterator iter = data_.begin(); iter != data_.end(); iter++) {
    for (unsigned i = 0; i < (iter->second).size(); i++) {
      os << "\t\t<instance>\n";
      os << "\t\t\t<category>" << iter->first << "</category>\n";
      os << "\t\t\t<polygon>";
      vsol_polygon_2d_sptr poly = (iter->second)[i];
      for (unsigned j = 0; j < poly->size(); j ++) 
        os << poly->vertex(j)->x() << " " << poly->vertex(j)->y() << " ";
      os << "</polygon>\n";
      os << "\t\t</instance>\n";
    }
  }
}

