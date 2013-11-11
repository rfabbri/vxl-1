#ifndef _dbmsh3d_textured_mesh_mc_h_
#define _dbmsh3d_textured_mesh_mc_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_mesh.h
//:
// \file
// \brief multiple connected (mc) mesh for
//        representing textured mesh faces with internal holes or curves.
//
// \author
//  Daniel Crispell  March 20, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_cassert.h>

#include <vcl_map.h>
#include <vcl_utility.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_textured_face_mc.h>

//: The mesh class that handles indexed face set.
class dbmsh3d_textured_mesh_mc : public dbmsh3d_mesh_mc
{
public:
  dbmsh3d_textured_mesh_mc() : dbmsh3d_mesh_mc(){}

  dbmsh3d_textured_mesh_mc(const dbmsh3d_mesh_mc* mesh);

  //: new/delete function of the class hierarchy
  virtual dbmsh3d_textured_face_mc* _new_face () { 
    return new dbmsh3d_textured_face_mc (face_id_counter_++);
  }

  virtual dbmsh3d_textured_mesh_mc* clone() const;

};


#endif

