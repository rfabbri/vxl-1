#ifndef _dbavl_camera_estimator_cxx_
#define _dbavl_camera_estimator_cxx_

#include "dbavl_camera_estimator.h"

#include <vnl/vnl_inverse.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vpgl/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_construct_cameras.h>
#include <dvpgl/algo/dvpgl_bundle_adjust.h>
#include <brct/brct_algos.h>

#include <vgl/vgl_distance.h>

//---------------------------------------------
dbavl_camera_estimator::dbavl_camera_estimator()
{

};


//---------------------------------------------
bool
dbavl_camera_estimator::estimate(
  const vcl_vector< vcl_vector< vgl_point_2d<double> > >& tracks,
  const vcl_vector< vcl_vector<bool> >& track_masks,
  const vpgl_calibration_matrix<double>& K,
  vcl_vector< vpgl_perspective_camera<double> >& cameras )
{
  cameras.clear();
  vpgl_calibration_matrix<double> K_copy = K;
  int num_frames = tracks.size();
  int num_tracks = tracks[num_frames-1].size();

  int start_frame = 0;
  int end_frame = 0;
  vpgl_perspective_camera<double> start_cam;
  start_cam.set_calibration( K );
  cameras.push_back( start_cam );

  while( end_frame != num_frames-1 ){

    // Find the furthest frame from the start frame that still has the
    // required 8 tracks all the way to it.  In the future we may want
    // to pick an end frame somewhere in between based on a max base-line
    // criteria.
    end_frame = start_frame;
    for( int f = start_frame+1; f < num_frames; f++ ){
      int num_tracks_from_start = 0;
      for( int t = 0; t < num_tracks; t++ ){
        if( track_masks[start_frame][t] == true && track_masks[f][t] == true )
          num_tracks_from_start++;
      }
      if( num_tracks_from_start < 8 )
        break;
      end_frame++;
    }

    // Abort if not enough tracks between two adjacent frames.
    if( end_frame == start_frame ) return false;

    // Get the cameras for the end frame, and world points.
    vcl_vector< vgl_point_2d<double> > start_pts, end_pts;
    for( int t = 0; t < num_tracks; t++ ){
      if( track_masks[start_frame][t] == true && track_masks[end_frame][t] == true ){
        start_pts.push_back( tracks[start_frame][t] );
        end_pts.push_back( tracks[end_frame][t] );
      }
    }
    vpgl_construct_cameras cc( start_pts, end_pts, &K_copy );
    cc.construct();
    vpgl_perspective_camera<double> end_cam = vpgl_align_up( 
      start_cam, cc.get_camera2() );

    // Note that these wps are in the coordinate system of start_camera.
    vcl_vector< vgl_point_3d<double> > world_pts;
    cc.get_world_points( world_pts );

    // Get all of the cameras between start_frame and end_frame.
    for( int f = start_frame+1; f < end_frame; f++ ){

      // Get a list of correspondences between frames.
      start_pts.clear(); end_pts.clear();
      for( int t = 0; t < num_tracks; t++ ){
        if( track_masks[start_frame][t] == true && track_masks[f][t] == true ){
          start_pts.push_back( tracks[start_frame][t] );
          end_pts.push_back( tracks[f][t] );
        }
      }

      vpgl_construct_cameras cc2( start_pts, end_pts, &K_copy );
      cc2.construct();
      vpgl_perspective_camera<double> mid_cam = cc2.get_camera2();

      // Get the projection of the world points in this frame.
      vcl_vector< vgl_point_2d<double> > mid_pts;
      for( int t = 0; t < num_tracks; t++ )
        if( track_masks[start_frame][t] == true && track_masks[end_frame][t] == true )
          mid_pts.push_back( tracks[f][t] );

      double this_error = find_best_camera( mid_cam, world_pts, mid_pts );
      cameras.push_back( vpgl_align_up( start_cam, mid_cam ) );
    }

    cameras.push_back( end_cam );
    start_frame = end_frame;
    start_cam = end_cam;
  }


/*
  // Do one last BA with all of the cameras.
  vcl_vector< vgl_point_2d<double> > track_ips;
  for( int f = 0; f < num_frames; f++ )
    for( int t = 0; t < num_tracks; t++ )
      track_ips.push_back( tracks[f][t] );
  dvpgl_bundle_adjust::optimize( cameras, iter_wps, track_ips, track_masks );
*/
  return true;
};


//-------------------------------------------------
double dbavl_camera_estimator::find_best_camera(
  vpgl_perspective_camera<double>& camera,
  const vcl_vector< vgl_point_3d<double> >& world_points,
  const vcl_vector< vgl_point_2d<double> >& image_points )
{
  // Find the optimal scale on translation.
  vgl_h_matrix_3d<double> R = camera.get_rotation_matrix();
  vgl_point_3d<double> t = camera.get_camera_center();
  vgl_homg_point_3d<double> Rt = R*vgl_homg_point_3d<double>( t );

  vnl_vector<double> a( 2*world_points.size() ), b( 2*world_points.size() );
  for( int p = 0; p < world_points.size(); p++ ){
    vgl_homg_point_3d<double> RX = R*vgl_homg_point_3d<double>( world_points[p] );
    a(2*p+0) = image_points[p].x()*RX.z() - RX.x();
    b(2*p+0) = image_points[p].x()*Rt.z() - Rt.x();
    a(2*p+1) = image_points[p].y()*RX.z() - RX.y();
    b(2*p+1) = image_points[p].y()*Rt.z() - Rt.y();
  }
  double scale = dot_product(a,b)/dot_product(b,b);

  vgl_point_3d<double> new_camera_center( 
    scale*camera.get_camera_center().x(), 
    scale*camera.get_camera_center().y(), 
    scale*camera.get_camera_center().z() );
  camera.set_camera_center( new_camera_center );

  // Compute the error.
  double error_sum = 0;
  for( int i = 0; i < a.size(); i++ )
    error_sum += fabs( a(i)-scale*b(i) );
  return error_sum;
};


#endif // _dbavl_camera_estimator_cxx_
