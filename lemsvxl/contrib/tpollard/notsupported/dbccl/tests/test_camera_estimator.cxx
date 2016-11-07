#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include "../dbccl_camera_estimator.h"

#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_distance.h>

static void test_camera_estimator()
{
  // Test find_best_camera.
  if( true ){
    vpgl_perspective_camera<double> true_cam;
    true_cam.set_camera_center( vgl_point_3d<double>(1.0,2.0,3.0) );
    
    vcl_vector< vgl_point_3d<double> > wps;
    wps.push_back( vgl_point_3d<double>(0,-1,5) );
    wps.push_back( vgl_point_3d<double>(1,2,6) );
    wps.push_back( vgl_point_3d<double>(-3,1,10) );
    wps.push_back( vgl_point_3d<double>(5,-4,4) );
    wps.push_back( vgl_point_3d<double>(2,2,2) );
    wps.push_back( vgl_point_3d<double>(-1,2,8) );
    wps.push_back( vgl_point_3d<double>(-6,5,7) );
    wps.push_back( vgl_point_3d<double>(2,-1,2) );
    wps.push_back( vgl_point_3d<double>(3,0,11) );
    wps.push_back( vgl_point_3d<double>(0,3,4) );

    vcl_vector< vgl_point_2d<double> > ips;
    for( int t = 0; t < 10; t++ ){
      vgl_homg_point_2d<double> new_pt = true_cam.project( 
        vgl_homg_point_3d<double>( wps[t] ) );
      ips.push_back( vgl_point_2d<double>(
        new_pt.x()/new_pt.w(), new_pt.y()/new_pt.w() ) );
    }

    vpgl_perspective_camera<double> off_cam = true_cam;
    off_cam.set_camera_center( vgl_point_3d<double>( 
      .2*off_cam.get_camera_center().x(), 
      .2*off_cam.get_camera_center().y(), 
      .2*off_cam.get_camera_center().z() ) );
    dbccl_camera_estimator ce;
    ce.find_best_camera( off_cam, wps, ips );

    vcl_cerr << "\nTrue camera:\n" << true_cam.get_matrix()
      << "\nEstimated camera:\n" << off_cam.get_matrix() << '\n';
    TEST_NEAR( "Testing find_best_camera:", 
      (true_cam.get_matrix()-off_cam.get_matrix()).frobenius_norm(), 0.0, .01 );
  }

  // Testing estimate on continuous tracks.
  if( false ){

    // Set up the cameras.
    int num_cameras = 100;
    vcl_vector< vpgl_perspective_camera<double> > true_cams;
    true_cams.push_back( vpgl_perspective_camera<double>() );
    for( int f = 1; f < num_cameras; f++ ){
      vpgl_perspective_camera<double> new_cam = true_cams[f-1];
      vgl_point_3d<double> cc = new_cam.get_camera_center();
      cc.set( cc.x()+.5-rand()/(double)RAND_MAX, 
        cc.y()+.5-rand()/(double)RAND_MAX, cc.z() );
      if( f == num_cameras-1 ) cc.set(0,1,0);
      new_cam.set_camera_center( cc );

      vgl_rotation_3d<double> new_rot(1*rand()/(double)RAND_MAX, .1*rand()/(double)RAND_MAX, 
        .1*rand()/(double)RAND_MAX);
      
      new_cam.set_rotation( new_rot );

      true_cams.push_back( new_cam );
    }

    // Set up the world points.
    int num_tracks = 10;
    vcl_vector< vgl_point_3d<double> > wps;
    wps.push_back( vgl_point_3d<double>(0,-1,5) );
    wps.push_back( vgl_point_3d<double>(1,2,6) );
    wps.push_back( vgl_point_3d<double>(-3,1,10) );
    wps.push_back( vgl_point_3d<double>(5,-4,3) );
    wps.push_back( vgl_point_3d<double>(2,2,2) );
    wps.push_back( vgl_point_3d<double>(-1,2,8) );
    wps.push_back( vgl_point_3d<double>(-6,5,7) );
    wps.push_back( vgl_point_3d<double>(2,-1,2) );
    wps.push_back( vgl_point_3d<double>(3,0,11) );
    wps.push_back( vgl_point_3d<double>(0,3,4) );

    // Project into all frames.
    vcl_vector< vcl_vector< vgl_point_2d<double> > > tracks;
    vcl_vector< vcl_vector<bool> > track_masks;
    for( int f = 0; f < num_cameras; f++ ){
      vcl_vector< vgl_point_2d<double> > new_track;
      vcl_vector<bool> new_track_mask;
      for( int t = 0; t < num_tracks; t++ ){
        vgl_homg_point_2d<double> new_pt = true_cams[f].project( 
          vgl_homg_point_3d<double>( wps[t] ) );
        new_track.push_back( vgl_point_2d<double>(
          new_pt.x()/new_pt.w(), new_pt.y()/new_pt.w() ) );
        new_track_mask.push_back( true );
      }
      tracks.push_back( new_track );
      track_masks.push_back( new_track_mask );
    }
    
    vcl_vector< vpgl_perspective_camera<double> > est_cams;
    dbccl_camera_estimator ce;
    ce.estimate( tracks, track_masks, 
      true_cams[0].get_calibration(), est_cams );

    double max_error = 0;
    for( int i = 0; i < num_cameras; i++ ){
      double this_error = 
        (true_cams[i].get_matrix()-est_cams[i].get_matrix()).frobenius_norm();
      if( max_error < this_error ) max_error = this_error;
    }
    TEST_NEAR( "Testing estimate with continuous tracks:", 
      max_error, 0.0, .01 );
  }

  // Testing estimate on disjoint tracks.
  if( true ){

    // Set up the cameras.
    int num_cameras = 100;
    vcl_vector< vpgl_perspective_camera<double> > true_cams;
    true_cams.push_back( vpgl_perspective_camera<double>() );
    for( int f = 1; f < num_cameras; f++ ){
      vpgl_perspective_camera<double> new_cam = true_cams[f-1];
      vgl_point_3d<double> cc = new_cam.get_camera_center();
      cc.set( cc.x()+.5-rand()/(double)RAND_MAX, 
        cc.y()+.5-rand()/(double)RAND_MAX, cc.z() );
      if( f == num_cameras-1 ) cc.set(0,1,0);
      new_cam.set_camera_center( cc );

      vgl_rotation_3d<double> new_rot(.1*rand()/(double)RAND_MAX, .1*rand()/(double)RAND_MAX, 
        .1*rand()/(double)RAND_MAX );        
      new_cam.set_rotation( new_rot );

      true_cams.push_back( new_cam );
    }

    // Set up the world points.
    int num_tracks = 16;
    vcl_vector< vgl_point_3d<double> > wps;
    wps.push_back( vgl_point_3d<double>(0,-1,5) );
    wps.push_back( vgl_point_3d<double>(1,2,6) );
    wps.push_back( vgl_point_3d<double>(-3,1,10) );
    wps.push_back( vgl_point_3d<double>(5,-4,3) );
    wps.push_back( vgl_point_3d<double>(2,2,2) );
    wps.push_back( vgl_point_3d<double>(-1,2,8) );
    wps.push_back( vgl_point_3d<double>(-6,5,7) );
    wps.push_back( vgl_point_3d<double>(2,-1,2) );
    wps.push_back( vgl_point_3d<double>(3,0,11) );
    wps.push_back( vgl_point_3d<double>(0,3,4) );
    wps.push_back( vgl_point_3d<double>(-2,4,3) );
    wps.push_back( vgl_point_3d<double>(1,-2,6) );
    wps.push_back( vgl_point_3d<double>(0,1,2) );
    wps.push_back( vgl_point_3d<double>(5,-1,8) );
    wps.push_back( vgl_point_3d<double>(2,2,3) );
    wps.push_back( vgl_point_3d<double>(-1,2,10) );

    // Project into all frames.
    int split_frame = 50;
    vcl_vector< vcl_vector< vgl_point_2d<double> > > tracks;
    vcl_vector< vcl_vector<bool> > track_masks;
    for( int f = 0; f < num_cameras; f++ ){

      vcl_vector< vgl_point_2d<double> > new_track;
      for( int t = 0; t < num_tracks; t++ ){
        vgl_homg_point_2d<double> new_pt = true_cams[f].project( 
          vgl_homg_point_3d<double>( wps[t] ) );
        new_track.push_back( vgl_point_2d<double>(
          new_pt.x()/new_pt.w(), new_pt.y()/new_pt.w() ) );
      }
      
      vcl_vector<bool> new_track_mask;
      for( int t = 0; t < 8; t++ ){
        if( f <= split_frame ) new_track_mask.push_back( true );
        else new_track_mask.push_back( false );
      }
      new_track_mask.push_back(true);
      for( int t = 9; t < num_tracks; t++ ){
        if( f < split_frame ) new_track_mask.push_back( false );
        else new_track_mask.push_back( true );
      }

      tracks.push_back( new_track );
      track_masks.push_back( new_track_mask );
    }
    
    vcl_vector< vpgl_perspective_camera<double> > est_cams;
    dbccl_camera_estimator ce;
    ce.estimate( tracks, track_masks, 
      true_cams[0].get_calibration(), est_cams );

    double max_error = 0;
    double scale = true_cams[num_cameras-1].get_matrix()(2,3)/
      est_cams[num_cameras-1].get_matrix()(2,3);
    for( int i = 0; i < 55; i++ ){
      vpgl_perspective_camera<double> c_true = true_cams[i], c_est = est_cams[i];
      c_est.set_camera_center( vgl_point_3d<double>( c_est.get_camera_center().x()*scale,
        c_est.get_camera_center().y()*scale, c_est.get_camera_center().z()*scale  ));
      double this_error = 
        (c_true.get_matrix()-c_est.get_matrix()).frobenius_norm();
      if( max_error < this_error ) max_error = this_error;
      vcl_cerr << c_true.get_matrix() << c_est.get_matrix();
      vcl_cerr << i << ' ' << this_error << '\n';
    }
    TEST_NEAR( "Testing estimate with split tracks:", 
      max_error, 0.0, .01 );
  }

}

TESTMAIN( test_camera_estimator );
