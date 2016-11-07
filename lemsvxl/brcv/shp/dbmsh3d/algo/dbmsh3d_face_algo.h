#ifndef dbmsh3d_face_algo_h_
#define dbmsh3d_face_algo_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_face_algo.h
//:
// \file
// \brief Find the foot point on a mesh face
//
//
// \author
//  MingChing Chang  Mar 02, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_face.h>

#include <dbmsh3d/dbmsh3d_mesh.h>

bool faces_in_same_plane (dbmsh3d_face* F1, dbmsh3d_face* F2);

//Find the foot point from a point to a face.
bool dbmsh3d_get_footpt_face (const vgl_point_3d<double>& queryP, const dbmsh3d_face* F, 
                              vgl_point_3d<double>& footpt, double& dist);

#endif

