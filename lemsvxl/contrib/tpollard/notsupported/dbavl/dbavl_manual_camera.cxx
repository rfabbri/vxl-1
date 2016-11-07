#ifndef _dbavl_manual_camera_cxx_
#define _dbavl_manual_camera_cxx_

#include "dbavl_manual_camera.h"

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>


//---------------------------------------------
dbavl_manual_camera::dbavl_manual_camera()
{

};


//---------------------------------------------
bool 
dbavl_manual_camera::compute(
  const vcl_vector< vgl_point_3d<double> >& world_points,
  const vcl_vector< vgl_point_2d<double> >& image_points,
  const vcl_vector< vgl_line_segment_2d<double> >& up_lines,
  vpgl_proj_camera<double>& camera )
{
  if( image_points.size() < 5 ) return false;

  // Find the dimensions of the solution matrix.
  int num_params = 11;
  int num_constraints = 2*image_points.size() + up_lines.size();
    
  vnl_matrix<double> S( num_constraints, num_params, 0 );
  vnl_vector<double> B( num_constraints, 0 );
  vnl_vector<double> V( num_params );

  // Fill in the solution matrix.
  int k = 0;
  for( int c = 0; c < image_points.size(); c++ ){
    S(k,0) = world_points[c].x();
    S(k,1) = world_points[c].y();
    S(k,2) = world_points[c].z();
    S(k,3) = 1;
    S(k,8) = -world_points[c].x()*image_points[c].x();
    S(k,9) = -world_points[c].y()*image_points[c].x();
    S(k,10) = -world_points[c].z()*image_points[c].x();
    B(k) = image_points[c].x();
    k++;
    S(k,4) = world_points[c].x();
    S(k,5) = world_points[c].y();
    S(k,6) = world_points[c].z();
    S(k,7) = 1;
    S(k,8) = -world_points[c].x()*image_points[c].y();
    S(k,9) = -world_points[c].y()*image_points[c].y();
    S(k,10) = -world_points[c].z()*image_points[c].y();
    B(k) = image_points[c].y();
    k++;
  }

  for( int c = 0; c < up_lines.size(); c++ ){
    vgl_point_2d<double> top = up_lines[c].point1();
    vgl_point_2d<double> bot = up_lines[c].point2();
    S(k,2) = top.y()-bot.y();
    S(k,6) = -( top.x()-bot.x() );
    S(k,10) = top.y()*( top.x()-bot.x() )-top.x()*( top.y()-bot.y() );
    k++;
  }
  
  vnl_svd<double> Ssvd(S);
  V = Ssvd.solve( B );
  
  // Reform the camera.
  vnl_matrix_fixed<double,3,4> P;
  P(0,0) = V(0); P(0,1) = V(1); P(0,2) = V(2); P(0,3) = V(3);
  P(1,0) = V(4); P(1,1) = V(5); P(1,2) = V(6); P(1,3) = V(7);
  P(2,0) = V(8); P(2,1) = V(9); P(2,2) = V(10); P(2,3) = 1;
  camera.set_matrix(P);
  return true;
};


#endif // _dbavl_manual_camera_cxx_
