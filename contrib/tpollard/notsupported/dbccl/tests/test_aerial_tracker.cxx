#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include "../dbccl_aerial_tracker.h"

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vpgl/vpgl_perspective_camera.h>


static void test_aerial_tracker()
{
  dbccl_aerial_tracker tracker;

  // Testing transform_window.
  if( true ){
    vnl_matrix<int> window( 
      tracker.params.nbhd_diameter(), tracker.params.nbhd_diameter(), 0 );
    for( int i = 0; i < tracker.params.nbhd_diameter(); i++ )
      for( int j = 0; j < tracker.params.nbhd_diameter(); j++ )
        if( (i+j) % 2 == 0 ) window(i,j) = 255;
    vnl_matrix<int> mi_mask_true( 
      tracker.mi_params.mi_mask_diameter(), tracker.mi_params.mi_mask_diameter(), 0 );
    for( int i = 0; i < tracker.mi_params.mi_mask_diameter(); i++ )
      for( int j = 0; j < tracker.mi_params.mi_mask_diameter(); j++ )
        if( (i+j) % 2 == 0 ) mi_mask_true(i,j) = 255;
    vnl_matrix<float> identity(2,2,0); identity(0,0) = 1.0; identity(1,1) = 1.0;
      
    vnl_matrix<int> mi_mask_comp;
    tracker.transform_window( window, identity, mi_mask_comp );

    float error_sum = 0;
    for( int i = 0; i < tracker.mi_params.mi_mask_diameter(); i++ )
      for( int j = 0; j < tracker.mi_params.mi_mask_diameter(); j++ )
        error_sum += abs( mi_mask_true(i,j)-mi_mask_comp(i,j) );
    TEST_NEAR( "\nTesting transform_window.\n", 
      error_sum , 0, .001 );
  }

  // Test the full tracker.
  if( true ){
    vpgl_perspective_camera<double> P1, P2;
    vpgl_calibration_matrix<double> K;
    K.set_principal_point( vgl_point_2d<double>( 200, 200 ) );
    K.set_focal_length( 200 );
    P1.set_calibration( K ); P2.set_calibration( K );
    P2.set_camera_center( vgl_point_3d<double>(.1,.2,0) );
    vcl_vector< vgl_homg_point_3d<double> > world_points;
    world_points.push_back( vgl_homg_point_3d<double>(-.5,-.3,1) );
    world_points.push_back( vgl_homg_point_3d<double>(-.4,-.1,2) );
    world_points.push_back( vgl_homg_point_3d<double>(.5,-.2,1) );
    world_points.push_back( vgl_homg_point_3d<double>(-.3,.3,3) );
    world_points.push_back( vgl_homg_point_3d<double>(.2,.3,1.5) );
    vcl_vector< vgl_point_2d<int> > img1_points, img2_points, empty_points;
    for( int i = 0; i < world_points.size(); i++ ){
      vgl_homg_point_2d<double> img1_point = P1.project( world_points[i] );
      vgl_homg_point_2d<double> img2_point = P2.project( world_points[i] );
      img1_points.push_back( vgl_point_2d<int>( 
        (int)floor(img1_point.x()/img1_point.w()),
        (int)floor(img1_point.y()/img1_point.w()) ) );
      img2_points.push_back( vgl_point_2d<int>( 
        (int)floor(img2_point.x()/img2_point.w()),
        (int)floor(img2_point.y()/img2_point.w()) ) );
    }
    vil_image_view<vxl_byte> img2( 400, 400 );
    for( int i = 0; i < 400; i++ )
      for( int j = 0; j < 400; j++ )
        img2(i,j) = 255;
    for( int i = 0; i < world_points.size(); i++ )
      img2( img2_points[i].y(), img2_points[i].x() ) = 0;
    
    tracker.track_frame( img2, img1_points );
    tracker.track_frame( img2, empty_points );
    vcl_vector< vgl_point_2d<int> > img2_points_comp = tracker.get_tracks();
    for( int i = 0; i < img2_points.size(); i++ )
      vcl_cerr << img1_points[i] << img2_points[i] << img2_points_comp[i] << '\n';

  }
}

TESTMAIN( test_aerial_tracker );
