// This is brcv/shp/dbmsh3d/dbmsh3d_richvertex.h

#ifndef dbmsh3d_richvertex_h_
#define dbmsh3d_richvertex_h_
//---------------------------------------------------------------------
//:
// \file
// \brief 
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date August 14, 2006
// 
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <vcl_map.h>
#include <vcl_string.h>

class dbmsh3d_richvertex : public dbmsh3d_vertex
{
public:
  // ======== Constructors / Destructors ======================================
  dbmsh3d_richvertex(int id): dbmsh3d_vertex(id) {}
  virtual ~dbmsh3d_richvertex(){}

  // ======== Data access ======================================

  //: Return name of the class
  virtual vcl_string is_a() const
  { return "dbmsh3d_richvertex"; }

  // scalar properties
  vcl_vector<vcl_string > scalar_property_list() const;
  void add_scalar_property(const vcl_string& tag, double value);
  bool get_scalar_property(const vcl_string& tag, double& value);


  


  
protected:
  vcl_map<vcl_string, double > scalar_properties_;
};


#endif  // dbmsh3d_richvertex_h_

