#ifndef _dbmsh3d_m2t_mesh_h_
#define _dbmsh3d_m2t_mesh_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_m2t_mesh.h
//:
// \file
// \brief 2-manifold triangular mesh
//
//
// \author
//  MingChing Chang  Aug 22, 2005
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

#include <dbmsh3d/dbmsh3d_pt_set.h>
#include <dbmsh3d/dbmsh3d_face.h>

class dbmsh3d_tri_edge : public vispt_elm
{
};

class dbmsh3d_tri_face : public vispt_elm
{
protected:
  int   id_;

  //:
  ///dbmsh3d_edge* 

public:
  dbmsh3d_tri_face (int id) {
    id_         = id;
  }

};



#endif

