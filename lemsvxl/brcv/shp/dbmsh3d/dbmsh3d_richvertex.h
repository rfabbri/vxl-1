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
#include <map>
#include <string>

class dbmsh3d_richvertex : public dbmsh3d_vertex
{
public:
  // ======== Constructors / Destructors ======================================
  dbmsh3d_richvertex(int id): dbmsh3d_vertex(id) {}
  virtual ~dbmsh3d_richvertex(){}

  // ======== Data access ======================================

  //: Return name of the class
  virtual std::string is_a() const
  { return "dbmsh3d_richvertex"; }

  // scalar properties
  std::vector<std::string > scalar_property_list() const;
  void add_scalar_property(const std::string& tag, double value);
  bool get_scalar_property(const std::string& tag, double& value);


  


  
protected:
  std::map<std::string, double > scalar_properties_;
};


#endif  // dbmsh3d_richvertex_h_

