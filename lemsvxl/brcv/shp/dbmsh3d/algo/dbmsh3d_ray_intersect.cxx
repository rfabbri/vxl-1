
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbgl/dbgl_triangle.h>
#include <ray_tri/ray_tri.h>
#include <dbmsh3d/algo/dbmsh3d_ray_intersect.h>

//: Intersect a ray to a mesh, return the first found intersection.
//  if no intersection, return NULL.
dbmsh3d_face* intersect_ray_mesh (const vgl_point_3d<double>& v_orig, 
                                  const vgl_vector_3d<double>& vector, 
                                  dbmsh3d_mesh* M, vgl_point_3d<double>& iP)
{
  vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
  for (; it != M->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;
    bool result = dbmsh3d_intersect_ray_face (v_orig, vector, F, iP);
    if (result)
      return F;
  }
  return NULL;
}

//: Intersect a ray to a mesh, return the closest found intersection.
//  if no intersection, return NULL.
dbmsh3d_face* intersect_ray_mesh_all (const vgl_point_3d<double>& v_orig, 
                                      const vgl_vector_3d<double>& vector, 
                                      dbmsh3d_mesh* M, vgl_point_3d<double>& input_iP)
{
  double d_min = DBL_MAX;
  vgl_point_3d<double> closest_iP, iP;
  dbmsh3d_face* closestF = NULL;

  vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
  for (; it != M->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;
    bool result = dbmsh3d_intersect_ray_face (v_orig, vector, F, iP);
    if (result) {
      double d = vgl_distance (v_orig, iP);
      if (d < d_min) {
        d_min = d;
        closest_iP = iP;
        closestF = F;
      }
    }
  }

  input_iP = closest_iP;
  return closestF;
}

// ----------------------------------------------------------------------------
//: Intersect a ray with a mesh face, 
//  Return true if the ray intersects the face at point on the face 
//  and the intersection point is saved in iP
//  Return false otherwise. In this case, iP is unchanged.
//  1) For a triangular face, call intersect_triangle().
//  2) For a convex polygon, call intersect_triangle() n-2 times and done.
//     Be careful when a ray intersect exactly at the dividing boundary of tris.
//  3) For a general polygon (convex or non-convex), need to triangulate the polygon
//     and apply intersect_triangle() several times.
//
bool dbmsh3d_intersect_ray_face (const vgl_point_3d<double>& v_orig, 
                                 const vgl_vector_3d<double>& vector, 
                                 dbmsh3d_face* F, vgl_point_3d<double>& iP)
{
  vcl_vector<dbmsh3d_vertex*> vertices;
  F->get_bnd_Vs (vertices);
  int nv = vertices.size();
  double ori[3];
  ori[0] = v_orig.x();
  ori[1] = v_orig.y();
  ori[2] = v_orig.z();
  double vec[3];
  vec[0] = vector.x();
  vec[1] = vector.y();
  vec[2] = vector.z();

  if (nv == 3) { //1) The triangular face case.
    double v1[3], v2[3], v3[3];
    vgl_point_3d<double > pts[3];
    for (unsigned i=0; i<3; ++i) {
      dbmsh3d_vertex* V = vertices[i];
      pts[i] = V->pt();
    }

    v1[0] = pts[0].x();
    v1[1] = pts[0].y();
    v1[2] = pts[0].z();

    v2[0] = pts[1].x();
    v2[1] = pts[1].y();
    v2[2] = pts[1].z();
    
    v3[0] = pts[2].x();
    v3[1] = pts[2].y();
    v3[2] = pts[2].z();

    double t, u, v;
    int result = intersect_triangle (ori, vec, v1, v2, v3, &t, &u, &v); 
    //check t>0 to make sure correct ray direction.
    if (result && t >0) {
      double rx = (1-u-v)*v1[0] + u*v2[0] + v*v3[0];
      double ry = (1-u-v)*v1[1] + u*v2[1] + v*v3[1];
      double rz = (1-u-v)*v1[2] + u*v2[2] + v*v3[2];
      iP.set (rx, ry, rz);
      return true;
    }
    else
      return false;
  }
  else { //2) The polygon case.
    vcl_cerr << "\tnon-triangular face id: " << F->id() << "#vertices: " <<  nv << vcl_endl;

    //bool dbmsh3d_triangulate_face (const dbmsh3d_face* F, vcl_vector<vcl_vector<int> >& tri_faces)
    assert (0);

    return false;
  }
}
