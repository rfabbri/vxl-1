// This is basic/dbgl/dbgl_minimal_container.h

#ifndef dbgl_minimal_container_h_
#define dbgl_minimal_container_h_

//:
// \file
// \brief Functions to compute minimal containers of geometric objects
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date 07/13/06
//
// \verbatim  
//  Modifications:
//
// \endverbatim

#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>




//: Compute the smallest circle that encloses a set of 2D points
// Return false if computation fails or point list is empty
template<class T >
bool dbgl_minimal_enclosing_circle(const vcl_vector<vgl_point_2d<T > >& points,
                                 vgl_point_2d<double >& center, 
                                 double& radius);


//: Compute the smallest sphere that encloses a set of 3D points
// Return false if computation fails or point list is empty
template<class T >
bool dbgl_minimal_enclosing_sphere(const vcl_vector<vgl_point_3d<T > >& points,
                                 vgl_point_3d<double >& center, 
                                 double& radius);




#endif // dbgl_minimal_container_h_
