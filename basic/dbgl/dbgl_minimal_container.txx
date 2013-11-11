// This is basic/dbgl/dbgl_minimal_container.txx

#ifndef dbgl_minimal_container_txx_
#define dbgl_minimal_container_txx_


//:
// \file


#include "dbgl_minimal_container.h"
#include <vcl_cmath.h>
#include "internals/miniball.txx"


// ----------------------------------------------------------------------------
//: Compute the smallest circle that encloses a set of 2D points
// Return false if computation fails or point list is empty
template<class T >
bool dbgl_minimal_enclosing_circle(const vcl_vector<vgl_point_2d<T > >& points,
                                 vgl_point_2d<double >& center, 
                                 double& radius)
{
  if (points.empty()) return false;

  // Miniball instance
  const int d = 2;
  Miniball<d > mb;
  typename Miniball<d >::Point p;

  // check in the points
  unsigned int num_pts = points.size();
  for (unsigned int i=0; i<num_pts; ++i)
  {
    vgl_point_2d<T > vgl_pt = points[i];
    
    p[0] = double(vgl_pt.x());
    p[1] = double(vgl_pt.y());
    mb.check_in(p);           
  }

  // construct ball, using the pivoting method
  mb.build();

  // Return values
  radius = vcl_sqrt(mb.squared_radius());
  typename Miniball<d>::Point mb_center(mb.center());
  center.set(mb_center[0], mb_center[1]);
  return true;
}


// ----------------------------------------------------------------------------
//: Compute smallest sphere that encloses a set of 3D points
// Return false if computation fails or point list is empty
template<class T >
bool dbgl_minimal_enclosing_sphere(const vcl_vector<vgl_point_3d<T > >& points,
                                 vgl_point_3d<double >& center, 
                                 double& radius)
{
  if (points.empty()) return false;

  // Miniball instance
  const int d = 3;
  Miniball<d > mb;
  typename Miniball<d >::Point p;

  // check in the points
  unsigned int num_pts = points.size();
  for (unsigned int i=0; i<num_pts; ++i)
  {
    vgl_point_3d<T > vgl_pt = points[i];
    
    p[0] = double(vgl_pt.x());
    p[1] = double(vgl_pt.y());
    p[2] = double(vgl_pt.z());
    mb.check_in(p);           
  }

  // construct ball, using the pivoting method
  mb.build();

  // Return values
  radius = vcl_sqrt(mb.squared_radius());
  typename Miniball<d>::Point mb_center(mb.center());
  center.set(mb_center[0], mb_center[1], mb_center[2]);
  return true;
}



#undef DBGL_MINIMAL_CONTAINER_INSTANTIATE
#define DBGL_MINIMAL_CONTAINER_INSTANTIATE(T) \
template bool dbgl_minimal_enclosing_circle(const vcl_vector<vgl_point_2d<T > >&, vgl_point_2d<double >&, double&);\
template bool dbgl_minimal_enclosing_sphere(const vcl_vector<vgl_point_3d<T > >&, vgl_point_3d<double >&, double&)

#endif // dbgl_minimal_container_txx_


