//: Sep 25, 2005 
//  MingChing Chang

#ifndef dbmsh3d_mesh_triangulate_h_
#define dbmsh3d_mesh_triangulate_h_

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_triangulate.h>

//: Triangulate the input mesh face into a set of triangular faces.
//  The mesh face can be any contour/polygon without holes. 
//  Return true if success.
//  Results: 
//    tri_faces: a vector of triangular faces specified in their 
//               ids of the original face F, e.g.,
//               f0: (v0, v1, v2), f1: (v0, v1, v2), ..., etc.
//
bool dbmsh3d_triangulate_face (const vcl_vector<dbmsh3d_vertex*>& vertices, 
                               vcl_vector<vcl_vector<int> >& tri_faces);
 
dbmsh3d_mesh* generate_tri_mesh (dbmsh3d_mesh* M);


#endif


