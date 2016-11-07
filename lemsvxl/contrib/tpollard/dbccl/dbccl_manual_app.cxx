#include <vcl_iostream.h>

#include <vcl_string.h>
#include <vcl_vector.h>
#include<vcl_cstdio.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_optimize_camera.h>
#include <vul/vul_awk.h>


/* APP WITH BUNDLE ADJUSTMENT
//-------------------------------------------
int main( int argc, char* argv[] )
{  
  vcl_string world_points_file( "D:\\images_multiview\\plasticville\\points.txt" );
  vcl_string image_points_file( "D:\\images_multiview\\plasticville/sunseq/020_image_pts.txt" );
  vcl_string camera_file( "D:\\images_multiview\\plasticville/sunseq/020_cameras.txt" );
  vcl_string refined_world_points_file( "D:\\images_multiview\\plasticville/sunseq/ERROR.txt" );
  int num_cameras = 8;
  bool known_calibration = false;
  vpgl_calibration_matrix<double> K( 2100, vgl_point_2d<double>( 640, 360 ) );
  bool bundle_adjust = false;

  // Read the world points.
  vcl_vector<vgl_point_3d<double> > world_points;
  vcl_ifstream wpsifs( world_points_file.c_str() );
  vul_awk wps( wpsifs );
  while( wps ){
    world_points.push_back( vgl_point_3d<double>(
      atof(wps[0]), atof(wps[1]), atof(wps[2]) ) );
    ++wps;
  }
  int num_world_points = world_points.size();

  // Determine a scaling for the world.
  vgl_vector_3d<double> mean_wp(0,0,0);
  for( int p = 0; p < num_world_points; p++ )
    mean_wp += ( world_points[p] - vgl_point_3d<double>(0,0,0) )/num_world_points;
  double wp_scale = 0;
  for( int p = 0; p < num_world_points; p++ ){
    vgl_point_3d<double> this_dist = world_points[p]-mean_wp;
    if( abs(this_dist.x()) > wp_scale ) wp_scale = abs(this_dist.x());
    if( abs(this_dist.y()) > wp_scale ) wp_scale = abs(this_dist.y());
    if( abs(this_dist.z()) > wp_scale ) wp_scale = abs(this_dist.z());
  }

  for( int p = 0; p < num_world_points; p++ ){
    world_points[p] = (world_points[p]-mean_wp);
    world_points[p].set( world_points[p].x()/wp_scale,
      world_points[p].y()/wp_scale, world_points[p].z()/wp_scale );
  }

  // Read the image_points
  vcl_vector<vgl_point_2d<double> > image_points;
  vcl_vector<vcl_vector<bool> > mask( num_cameras, vcl_vector<bool>(num_world_points,true) );
  vcl_ifstream ipsifs( image_points_file.c_str() );
  vul_awk ips( ipsifs );
  for( int c = 0; c < num_cameras; c++ ){
    for( int wp = 0; wp < num_world_points; wp++ ){
      if( ips.NF() == 0 ){ ++ips; wp--; continue; }
      vgl_point_2d<double> new_ip( atof(ips[0]), atof(ips[1]) );
      image_points.push_back( new_ip );
      if( new_ip.x() < 0 || new_ip.y() < 0 )
        mask[c][wp] = false;
      ++ips;
    }
  }

  // Get a initial set of cameras.
  vcl_ofstream cofs( camera_file.c_str() );
  vcl_vector<vpgl_perspective_camera<double> > cameras;
  vcl_vector<vpgl_proj_camera<double> > cameras_proj;
  for( int c = 0; c < num_cameras; c++ ){
    
    // Get a crude camera with its own calibration matrix.
    vpgl_proj_camera<double> this_cam;
    vcl_vector<vgl_point_2d<double> > these_image_points;
    vcl_vector<vgl_point_3d<double> > these_world_points;
    for( int dp = 0; dp < num_world_points; dp++ ){
      if( mask[c][dp] == false ) 
        continue;
      int p = num_world_points*c+dp;
      these_world_points.push_back( world_points[dp] );
      these_image_points.push_back( image_points[p] );
    }

    vpgl_proj_camera_compute::compute( these_image_points, these_world_points, this_cam );
    cameras_proj.push_back( this_cam );
    vpgl_perspective_camera<double> this_cam_perspective;
    vpgl_perspective_decomposition( this_cam.get_matrix(), this_cam_perspective );

    // Get a better initial camera with the known calibration.
    if( known_calibration ){
      vcl_vector< vgl_homg_point_3d<double> > these_world_points_h;
      for( int p =0; p < these_world_points.size(); p++ )
        these_world_points_h.push_back( vgl_homg_point_3d<double>( these_world_points[p] ) );
      this_cam_perspective.set_calibration( K );
      vpgl_perspective_camera<double> opt_cam = 
        vpgl_optimize_camera::opt_orient_pos(this_cam_perspective,these_world_points_h,these_image_points);
      cameras.push_back( opt_cam );
    }
    else cameras.push_back( this_cam_perspective );
  }
    
  // Compute image errors.
  for( int c = 0; c < num_cameras; c++ ){
    for( int wp = 0; wp < num_world_points; wp++ ){
      vgl_homg_point_2d<double> proj_wp_h;
      if( known_calibration ) proj_wp_h = cameras[c].project(
        vgl_homg_point_3d<double>( world_points[wp] ) );
      else proj_wp_h = cameras_proj[c].project(
        vgl_homg_point_3d<double>( world_points[wp] ) );
      vgl_point_2d<double> proj_wp( proj_wp_h.x()/proj_wp_h.w(), proj_wp_h.y()/proj_wp_h.w() );
      float this_error = vgl_distance( proj_wp, image_points[num_world_points*c+wp] );
      vcl_cerr << this_error << ' ';
    }
    vcl_cerr << "\n\n";
  }

  // Optimize the cameras and world points if needed.
  if( bundle_adjust ){
    vcl_vector< vgl_point_2d<double> > exp_img_points;
    for( int c = 0; c < num_cameras; c++ ){
      for( int dp = 0; dp < num_world_points; dp++ ){
        if( mask[c][dp] == false ) 
          continue;
        exp_img_points.push_back( image_points[c*num_world_points+dp] );
      }
    }
    vpgl_bundle_adjust::optimize( cameras, world_points, exp_img_points, mask );
  }

  // Unscale the cameras and world_points.
  for( int p = 0; p < num_world_points; p++ )
    world_points[p].set( 
      world_points[p].x()*wp_scale + mean_wp.x(),
      world_points[p].y()*wp_scale + mean_wp.y(),
      world_points[p].z()*wp_scale + mean_wp.z() );
  vnl_matrix_fixed<double,4,4> scale_matrix(0.0);
  scale_matrix(0,0) = scale_matrix(1,1) = scale_matrix(2,2) = 1.0/wp_scale;
  scale_matrix(0,3) = -mean_wp.x()/wp_scale;
  scale_matrix(1,3) = -mean_wp.y()/wp_scale;
  scale_matrix(2,3) = -mean_wp.z()/wp_scale;
  scale_matrix(3,3) = 1.0;

  vcl_vector< vpgl_proj_camera<double> > final_cameras;
  for( int c = 0; c < num_cameras; c++ )
    final_cameras.push_back( vpgl_proj_camera<double>( 
      cameras[c].get_matrix()*scale_matrix ) );

  // Compute errors after bundle adjustment.
  for( int c = 0; c < num_cameras; c++ ){
    for( int wp = 0; wp < num_world_points; wp++ ){
      vgl_homg_point_2d<double> proj_wp_h = final_cameras[c].project(
        vgl_homg_point_3d<double>( world_points[wp] ) );
      vgl_point_2d<double> proj_wp( proj_wp_h.x()/proj_wp_h.w(), proj_wp_h.y()/proj_wp_h.w() );
      float this_error = vgl_distance( proj_wp, image_points[num_world_points*c+wp] );
      vcl_cerr << this_error << ' ';
    }
    vcl_cerr << "\n\n";
  }

  // Write to file.
  for( int c = 0; c < num_cameras; c++ ){
    if( known_calibration ) cofs << "FRAME " << c << '\n' << final_cameras[c].get_matrix() << '\n';
    else cofs << "FRAME " << c << '\n' << cameras_proj[c].get_matrix() << '\n';
  }
  
  if( bundle_adjust ){
    vcl_ofstream wpofs( refined_world_points_file.c_str() );
    for( int wp = 0; wp < num_world_points; wp++ )
      wpofs << world_points[wp].x() << ' ' << world_points[wp].y() << ' ' << world_points[wp].z() << '\n';
  }

  return 1;
}
*/

//-------------------------------------------
int main( int argc, char* argv[] )
{  
  vcl_string world_points_file( "D:\\images_multiview\\plasticville\\points.txt" );
  vcl_string image_points_file( "D:\\images_multiview\\plasticville/sunseq/160_image_pts.txt" );
  vcl_string camera_file( "D:\\images_multiview\\plasticville/sunseq/160_cameras.txt" );
  int num_cameras = 8;

  // Read the world points.
  vcl_vector<vgl_point_3d<double> > world_points;
  vcl_ifstream wpsifs( world_points_file.c_str() );
  vul_awk wps( wpsifs );
  while( wps ){
    world_points.push_back( vgl_point_3d<double>(
      atof(wps[0]), atof(wps[1]), atof(wps[2]) ) );
    ++wps;
  }
  int num_world_points = world_points.size();

  // Read the image_points
  vcl_vector<vgl_point_2d<double> > image_points;
  vcl_vector<vcl_vector<bool> > mask( num_cameras, vcl_vector<bool>(num_world_points,true) );
  vcl_ifstream ipsifs( image_points_file.c_str() );
  vul_awk ips( ipsifs );
  for( int c = 0; c < num_cameras; c++ ){
    for( int wp = 0; wp < num_world_points; wp++ ){
      if( ips.NF() == 0 ){ ++ips; wp--; continue; }
      vgl_point_2d<double> new_ip( atof(ips[0]), atof(ips[1]) );
      image_points.push_back( new_ip );
      if( new_ip.x() < 0 || new_ip.y() < 0 )
        mask[c][wp] = false;
      ++ips;
    }
  }

  // Compute cameras.
  vcl_vector<vpgl_proj_camera<double> > cameras;
  for( int c = 0; c < num_cameras; c++ ){

    vpgl_proj_camera<double> this_cam;
    vcl_vector<vgl_point_2d<double> > these_image_points;
    vcl_vector<vgl_point_3d<double> > these_world_points;
    for( int dp = 0; dp < num_world_points; dp++ ){
      if( mask[c][dp] == false ) 
        continue;
      int p = num_world_points*c+dp;
      these_world_points.push_back( world_points[dp] );
      these_image_points.push_back( image_points[p] );
    }
    vpgl_proj_camera_compute::compute( these_image_points, these_world_points, this_cam );
    cameras.push_back( this_cam );
  }
    
  // Compute image errors.
  for( int c = 0; c < num_cameras; c++ ){
    for( int wp = 0; wp < num_world_points; wp++ ){
      vgl_homg_point_2d<double> proj_wp_h = cameras[c].project(
        vgl_homg_point_3d<double>( world_points[wp] ) );
      vgl_point_2d<double> proj_wp( proj_wp_h.x()/proj_wp_h.w(), proj_wp_h.y()/proj_wp_h.w() );
      float this_error = vgl_distance( proj_wp, image_points[num_world_points*c+wp] );
      vcl_cerr << proj_wp << ' ' << this_error << '\n';
    }
    vcl_cerr << "\n\n";
  }

  // Write to file.
  vcl_ofstream cofs( camera_file.c_str() );
  for( int c = 0; c < num_cameras; c++ )
    cofs << "FRAME " << c << '\n' << cameras[c].get_matrix() << '\n';
  
  return 1;
}
