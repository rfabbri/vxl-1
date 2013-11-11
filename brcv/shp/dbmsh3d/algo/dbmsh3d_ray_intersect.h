// This is dbmsh3d/algo/dbmsh3d_ray_intersect.h

#ifndef dbmsh3d_ray_intersect_h_
#define dbmsh3d_ray_intersect_h_

//:
// \file
// \brief Functions to intersect a ray with elements of a mesh
//        
// \author MingChing Chang
// \date Aug 05, 2005
//
// \verbatim
//  Modifications
//      Nhon Trinh  :    Added dbmsh3d_intersect_ray_face
// \endverbatim


#include <dbmsh3d/dbmsh3d_face.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

//: 050805 MingChing Chang
//  Tomas Moller and Ben Trumbore's ray-triangle intersection
//  http://jgt.akpeters.com/papers/MollerTrumbore97/

//: Intersect a ray to a mesh, return the first found intersection.
dbmsh3d_face* intersect_ray_mesh (const vgl_point_3d<double>& v_orig, 
                                  const vgl_vector_3d<double>& vector, 
                                  dbmsh3d_mesh* mesh, vgl_point_3d<double>& R);

//: Intersect a ray to a mesh, return the closest found intersection.
dbmsh3d_face* intersect_ray_mesh_all (const vgl_point_3d<double>& v_orig, 
                                      const vgl_vector_3d<double>& vector, 
                                      dbmsh3d_mesh* M, vgl_point_3d<double>& iP);

//: Intersect a ray with a triangular face, 
// Return true if the ray intersects the face at point on the face 
// and the intersection point is saved in ``intersect_pt"
// Return false when there is no intersection or when the face is not triangular
// In this case, ``intersect_pt" is unchanged.
bool dbmsh3d_intersect_ray_face (const vgl_point_3d<double>& v_orig, 
                                 const vgl_vector_3d<double>& vector, 
                                 dbmsh3d_face* F, 
                                 vgl_point_3d<double>& intersect_pt);

#endif



