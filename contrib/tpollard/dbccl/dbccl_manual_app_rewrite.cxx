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

#include <vcl_iomanip.h>

//-------------------------------------------
// CAMERA CALIBRATION
//   This is the process of computing camera models for each image.
//   It is assumed that the user has selected 3D points on the mesh model and
//   corresponding 2D points on the desired image for which a camera is to be
//   recovered. This correspondence information is in the form of ascii text files:
//
// INPUTS
//   world_pts.txt
//     A plain text file with coordinates of 3D points selected on the mesh. Each
//     point is described in a single line as x y z numbers separated by spaces. It
//     must contain at least 6 correspondences, but at least 12 is recommended to
//     achieve reasonable accuracy.
//
//   image_pts.txt
//     A plain text file with coordinates of 2D points selected on the image
//     corresponding to the 3D points. Each point is described in a single line as
//     x y numbers separated by spaces.  The order and number of the points must
//     match that of world_pts.txt file.
//
// OUTPUTS
//   cameras.txt
//     A plain text file with the entries of the 3x4 projection matrix P separated by
//     spaces, with each row of P on a separate line. 
//
// USAGE
//   1- Rename your files to conform to the input convention. Place the files in
//      the same directory as the program executable
//
//   2- Run the program dbccl_manual_app_rewrite
//
//   3- Rename the resulting cameras.txt file back to the convention needed by the
//      next piece of software.
//
//   Usually you will have to repeat this process for each image you want to calibrate
//
// TROUBLESHOOTING
//   The program outputs the reprojection errors for each of the image points. If
//   any of these errors is above 6px, then the correspondences are not precise
//   enough. You must either improve the precision of your selection, or else try
//   to include additional points so that the camera calibration becomes more robust.

// SOURCE CODE 
//   This uses the library vpgl from vxl to compute the cameras linearly, followed
//   by bundle adjustment to refine the cameras if enough correspondences are available.

// AUTHOR
//   Ricardo Fabbri  (rfabbri at lems  brown  edu) 
//   Brown University, Dec 2007
//   Based on code from Thom Pollard
//
int main( int argc, char* argv[] )
{  
  vcl_string world_points_file( "world_pts.txt" );
  vcl_string image_points_file( "image_pts.txt" );
  vcl_string camera_file( "cameras.txt" );
  vcl_string camera_center_direction_file( "cameras-center-direction.txt" );
  vcl_string refined_world_points_file( "world_pts_refined.txt" );
  const bool bundle_adjust = true;
  const bool use_normalization = true;
  int num_cameras = 1;
  bool known_calibration = false;
  // internal calibration used only if known_calibration == true
  vpgl_calibration_matrix<double> K(19800, vgl_point_2d<double>( 726, 455.44) );


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


  // Use normalization

  double wp_scale = 0;
  vgl_vector_3d<double> mean_wp(0,0,0);
  if (use_normalization) {
    // Determine a scaling for the world.
    for( int p = 0; p < num_world_points; p++ )
      mean_wp += ( world_points[p] - vgl_point_3d<double>(0,0,0) )/num_world_points;
      for( int p = 0; p < num_world_points; p++ ){
        vgl_point_3d<double> this_dist = world_points[p]-mean_wp;
        if( vcl_fabs(this_dist.x()) > wp_scale ) wp_scale = vcl_fabs(this_dist.x());
        if( vcl_fabs(this_dist.y()) > wp_scale ) wp_scale = vcl_fabs(this_dist.y());
        if( vcl_fabs(this_dist.z()) > wp_scale ) wp_scale = vcl_fabs(this_dist.z());
      }

    for( int p = 0; p < num_world_points; p++ ){
      world_points[p] = (world_points[p]-mean_wp);
      world_points[p].set( world_points[p].x()/wp_scale,
        world_points[p].y()/wp_scale, world_points[p].z()/wp_scale );
    }
  }

  vcl_vector< vgl_homg_point_3d<double> > world_points_h;
  for( unsigned p =0; p < world_points.size(); p++ )
    world_points_h.push_back( vgl_homg_point_3d<double>(world_points[p] ) );


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

  // Compute cameras linearly.
  vcl_vector<vpgl_proj_camera<double> > cameras;
  cameras.reserve(num_cameras);

  // this block outputs in the cameras vcl_vector
  {
    vcl_vector<vpgl_perspective_camera<double> > cameras_persp;
    cameras_persp.reserve(cameras.size());
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

      vpgl_perspective_camera<double> p_camera;
      unsigned val = vpgl_perspective_decomposition(cameras[c].get_matrix(), p_camera);
      if (!val) {
        vcl_cerr << "ERROR: CANNOT DECOMPOSE" << vcl_endl;
        abort();
      }
      
      // Get a better initial camera with the known calibration.
      if( known_calibration ){
        p_camera.set_calibration( K );
        vpgl_perspective_camera<double> opt_cam = 
          vpgl_optimize_camera::opt_orient_pos(p_camera,world_points_h,image_points);
        cameras_persp.push_back(opt_cam);
      } else
        cameras_persp.push_back(p_camera);
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
      vpgl_bundle_adjust::optimize( cameras_persp, world_points, exp_img_points, mask );
      for( int c = 0; c < num_cameras; c++ ){
        cameras[c] = vpgl_proj_camera<double>(cameras_persp[c].get_matrix());
      }
    }
  }


  if (use_normalization) {
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

    for( int c = 0; c < num_cameras; c++ ) {
      vpgl_proj_camera<double> final_camera(cameras[c].get_matrix()*scale_matrix);
      cameras[c] = final_camera;
    }
  }
    
  // Compute image errors.
  for( int c = 0; c < num_cameras; c++ ){
    for( int wp = 0; wp < num_world_points; wp++ ){
      vgl_homg_point_2d<double> proj_wp_h;
      proj_wp_h = cameras[c].project(vgl_homg_point_3d<double>( world_points[wp] ) );

      vgl_point_2d<double> proj_wp( proj_wp_h.x()/proj_wp_h.w(), proj_wp_h.y()/proj_wp_h.w() );
      float this_error = vgl_distance( proj_wp, image_points[num_world_points*c+wp] );
      vcl_cerr << proj_wp << ' ' << this_error << '\n';
    }
    vcl_cerr << "\n\n";
  }


  // Write to file.
  vcl_ofstream cofs( camera_file.c_str() );
  cofs << vcl_setprecision(20);
  for( int c = 0; c < num_cameras; c++ )
    cofs << 
//      "FRAME " << c << '\n' << 
      cameras[c].get_matrix() << '\n';



  vcl_ofstream c_cd_ofs( camera_center_direction_file.c_str() );
  c_cd_ofs << vcl_setprecision(20);
  for( int c = 0; c < num_cameras; c++ ) {
    vpgl_perspective_camera<double> p_camera;
    unsigned val = vpgl_perspective_decomposition(cameras[c].get_matrix(), p_camera);
    if (!val) {
      vcl_cerr << "ERROR: CANNOT DECOMPOSE" << vcl_endl;
      abort();
    }

    vgl_point_3d<double> cpt(p_camera.camera_center());
    vgl_vector_3d<double> direction_v = p_camera.principal_axis();

    c_cd_ofs <<  vnl_vector_fixed<double,3> (cpt.x(),cpt.y(),cpt.z()) << '\n' << 
      vnl_vector_fixed<double,3>(direction_v.x(),direction_v.y(),direction_v.z()) << '\n';
  }

  if( bundle_adjust ){
    vcl_ofstream wpofs( refined_world_points_file.c_str() );
    for( int wp = 0; wp < num_world_points; wp++ )
      wpofs << world_points[wp].x() << ' ' << world_points[wp].y() << ' ' << world_points[wp].z() << '\n';
  }

  
  return 0;
}
