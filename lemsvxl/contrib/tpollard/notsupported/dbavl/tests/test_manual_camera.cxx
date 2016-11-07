#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include "../dbavl_manual_camera.h"

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>


static void test_manual_camera()
{
  vpgl_calibration_matrix<double> K;
  K.set_focal_length(100); K.set_principal_point( vgl_point_2d<double>(100,100));
  vpgl_perspective_camera<double> P_true;
  P_true.set_calibration(K);
  P_true.set_camera_center( vgl_point_3d<double>(1,2,4) );

  vcl_vector< vgl_point_3d<double> > world_points;
  vcl_vector< vgl_point_2d<double> > image_points;
  vcl_vector< vgl_line_segment_2d<double> > up_lines;
  for( int i = 0; i < 8; i++ ){
    vgl_point_3d<double> wp;
    if( i == 0 ) wp.set( 1, -1, 2 );
    if( i == 1 ) wp.set( 2, 0, 2 );
    if( i == 2 ) wp.set( -2, 1, -1 );
    if( i == 3 ) wp.set( -1, -2, 0 );
    if( i == 4 ) wp.set( 0, 2, -1 );
    if( i == 5 ) wp.set( 1, -1, 1 );
    if( i == 6 ) wp.set( -1, 1, 2 );
    if( i == 7 ) wp.set( 2, 0, -2 );
    vgl_homg_point_2d<double> ip = P_true.project( vgl_homg_point_3d<double>(wp) );
    world_points.push_back( wp );
    image_points.push_back( vgl_point_2d<double>( ip.x()/ip.w(), ip.y()/ip.w() ) );
  }
  for( int i = 0; i < 4; i++ ){
    vgl_point_3d<double> bp;
    if( i == 0 ) bp.set( -1, 2, 2 );
    if( i == 1 ) bp.set( 2, 1, -1 );
    if( i == 2 ) bp.set( -2, -1, 0 );
    if( i == 3 ) bp.set( 1, 0, 0 );
    vgl_point_3d<double> tp( bp.x(), bp.y(), bp.z() + 3 );
    vgl_homg_point_2d<double> ibp = P_true.project( vgl_homg_point_3d<double>(bp) );
    vgl_homg_point_2d<double> itp = P_true.project( vgl_homg_point_3d<double>(tp) );
    vgl_line_segment_2d<double> ls( 
      vgl_point_2d<double>( ibp.x()/ibp.w(), ibp.y()/ibp.w() ),
      vgl_point_2d<double>( itp.x()/itp.w(), itp.y()/itp.w() ) );
    up_lines.push_back( ls );
  }

  vpgl_proj_camera<double> P_est;
  dbavl_manual_camera mc;
  mc.compute( world_points, image_points, up_lines, P_est );
  vnl_matrix<double> P_true_norm = P_true.get_matrix();
  P_true_norm /= P_true_norm(2,3);
  vnl_matrix<double> P_est_norm = P_est.get_matrix();
  P_est_norm /= P_est_norm(2,3);
  vcl_cerr << P_true_norm << '\n' << P_est_norm;
  TEST_NEAR( "Testing full computation:", (P_true_norm-P_est_norm).frobenius_norm(), 0.0, .1 );
}

TESTMAIN( test_manual_camera );
