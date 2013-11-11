// This is brcv/shp/dbmsh3d/dbmsh3d_richface.cxx
//:


#include "dbmsh3d_richface.h"
#include <vcl_utility.h>






// scalar property

// ----------------------------------------------------------------------------
vcl_vector<vcl_string > dbmsh3d_richface::
scalar_property_list() const
{
  vcl_vector<vcl_string > prop_list;
  prop_list.reserve(this->scalar_properties_.size());
  for (vcl_map<vcl_string, double >::const_iterator it = 
    this->scalar_properties_.begin(); it != this->scalar_properties_.end(); ++it)
  {
    prop_list.push_back(it->first);
  }
  return prop_list;
}

// ----------------------------------------------------------------------------
void dbmsh3d_richface::add_scalar_property(const vcl_string& tag, 
                                             double value)
{
  this->scalar_properties_.insert(vcl_make_pair(tag, value));
}

// ----------------------------------------------------------------------------
bool dbmsh3d_richface::
get_scalar_property(const vcl_string& tag, double& value)
{
  vcl_map<vcl_string, double >::iterator it = 
    this->scalar_properties_.find(tag);
  if (it == this->scalar_properties_.end()) return false;
  value = it->second;
  return true;
}
