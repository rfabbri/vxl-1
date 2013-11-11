#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include "../dbrcl_compute_constrained_cameras.h"
#include <vnl/vnl_vector_fixed.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vcl_cstdlib.h>

static void test_constrained_camera()
{
  // Make a test scene.
  vnl_matrix_fixed<double,3,4> m1;
  m1(0,0)=10; m1(0,1)=4; m1(0,2)=2; m1(0,3)=15;
  m1(1,0)=-1; m1(1,1)=5; m1(1,2)=-1; m1(1,3)=7;
  //m1(2,0)=.4; m1(2,1)=-.2; m1(2,2)=.7; m1(2,3)=1;
  m1(2,0)=0; m1(2,1)=0; m1(2,2)=0; m1(2,3)=1;

  vnl_matrix_fixed<double,3,4> m2 = m1;
  m2(0,2) = -1; m2(1,2) = .1;
  dbrcl_camera_constraints constraints1, constraints2;
  constraints1.frame_number = 0;
  constraints2.frame_number = 1;
  vpgl_proj_camera<double> c1( m1 ), c2( m2 );
  vcl_cerr << "True camera matrices:\nFrame 0:\n" << m1 << "Frame 1:\n" << m2;

  // Create some world image correspondences.
  for( int i = 0; i < 6; i++ ){
    vgl_point_3d<double> new_world_point;
    if( i == 0 ) new_world_point = vgl_point_3d<double>( 0, 0, 0 );
    if( i == 1 ) new_world_point = vgl_point_3d<double>( 0, 20, 0 );
    if( i == 2 ) new_world_point = vgl_point_3d<double>( 0, 0, 30 );
    if( i == 3 ) new_world_point = vgl_point_3d<double>( 0, 20, 30 );
    if( i == 4 ) new_world_point = vgl_point_3d<double>( 10, 0, 30 );
    if( i == 5 ) new_world_point = vgl_point_3d<double>( 10, 20, 30 );
    if( i == 6 ) new_world_point = vgl_point_3d<double>( 0, 20, 30 );
    vgl_point_2d<double> new_image_point1 = c1.project( 
      vgl_homg_point_3d<double>(new_world_point) );
    vgl_point_2d<double> new_image_point2 = c2.project( 
      vgl_homg_point_3d<double>(new_world_point) );
    vgl_point_3d<double> noisy_world_point(
      new_world_point.x()+.2*(-.5+vcl_rand()/(double)RAND_MAX),
      new_world_point.y()+.2*(-.5+vcl_rand()/(double)RAND_MAX),
      new_world_point.z()+.2*(-.5+vcl_rand()/(double)RAND_MAX) );
    constraints1.world_points.push_back( noisy_world_point );
    constraints1.image_points.push_back( new_image_point1 );
    constraints2.world_points.push_back( noisy_world_point );
    constraints2.image_points.push_back( new_image_point2 );
  }

  // Create some heights.
  for( int i = 0; i < 5; i++ ){
    vgl_point_3d<double> new_bot_point(0,0,0), new_top_point(0,0,0);
    if( i == 0 ) new_bot_point = vgl_point_3d<double>( -1, 4, 5 );
    if( i == 1 ) new_bot_point = vgl_point_3d<double>( 0, -2, -1 );
    if( i == 2 ) new_bot_point = vgl_point_3d<double>( 8, -1, 2 );
    if( i == 3 ) new_bot_point = vgl_point_3d<double>( 2, 2, 2 );
    if( i == 4 ) new_bot_point = vgl_point_3d<double>( 15, -20, 5 );
    if( i == 0 ) new_top_point = vgl_point_3d<double>( -1, 4, 15 );
    if( i == 1 ) new_top_point = vgl_point_3d<double>( 0, -2, 2 );
    if( i == 2 ) new_top_point = vgl_point_3d<double>( 8, -1, 8 );
    if( i == 3 ) new_top_point = vgl_point_3d<double>( 2, 2, 9 );
    if( i == 4 ) new_top_point = vgl_point_3d<double>( 15, -20, 10 );
    vgl_point_2d<double> new_bot_point1 = c1.project( 
      vgl_homg_point_3d<double>(new_bot_point) );
    vgl_point_2d<double> new_top_point1 = c1.project( 
      vgl_homg_point_3d<double>(new_top_point) );
    vgl_point_2d<double> new_bot_point2 = c2.project( 
      vgl_homg_point_3d<double>(new_bot_point) );
    vgl_point_2d<double> new_top_point2 = c2.project( 
      vgl_homg_point_3d<double>(new_top_point) );
    constraints1.height_top_points.push_back( new_top_point1 );
    constraints1.height_bot_points.push_back( new_bot_point1 );
    constraints2.height_top_points.push_back( new_top_point2 );
    constraints2.height_bot_points.push_back( new_bot_point2 );
  }

  vcl_vector< vpgl_proj_camera<double> > estimated_cameras1;
  vcl_vector<int> frames1;
  dbrcl_compute_constrained_cameras cc1;
  cc1.add_constraint( constraints1 );
  cc1.add_constraint( constraints2 );
  cc1.compute_cameras( estimated_cameras1, frames1, true );
  vnl_matrix_fixed<double,3,4> em1 = estimated_cameras1[0].get_matrix();
  vnl_matrix_fixed<double,3,4> em2 = estimated_cameras1[1].get_matrix();

  vcl_cerr << "\nEstimated camera matrices:\nFrame 0:\n" << em1 << "Frame 1:\n" << em2;

  TEST_NEAR( "Estimated camera from perfect world-image correspondences", 
    (c1.get_matrix()-em1).frobenius_norm() , 0, 2 );



}

TESTMAIN(test_constrained_camera);
