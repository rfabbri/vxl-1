// This is brcv/shp/dbmsh3d/dbmsh3d_richface.h

#ifndef dbmsh3d_richface_h_
#define dbmsh3d_richface_h_
//---------------------------------------------------------------------
//:
// \file
// \brief 
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 27, 2006
// 
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbmsh3d/dbmsh3d_face.h>
#include <vcl_map.h>
#include <vcl_string.h>

class dbmsh3d_richface : public dbmsh3d_face
{
public:
  // ======== Constructors / Destructors ======================================
  dbmsh3d_richface(int id): dbmsh3d_face(id) {}
  virtual ~dbmsh3d_richface(){}

  // ======== Data access ======================================

  //: Return name of the class
  virtual vcl_string is_a() const
  { return "dbmsh3d_richface"; }

  // scalar properties
  vcl_vector<vcl_string > scalar_property_list() const;
  void add_scalar_property(const vcl_string& tag, double value);
  bool get_scalar_property(const vcl_string& tag, double& value);
  
protected:
  vcl_map<vcl_string, double > scalar_properties_;
};


#endif  // dbmsh3d_richface_h_

