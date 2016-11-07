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

// AUTHOR
//   Ricardo Fabbri  (rfabbri at lems  brown  edu) 
//   Based on code from Thom Pollard
//
int main( int /*argc*/, char* /*argv*/[] )
{  
  vcl_string world_points_file( "world_pts.txt" );
  vcl_string image_points_file( "image_pts.txt" );
//  vcl_string camera_file( "calib_intrinsic_in.txt" );
  vcl_string camera_out_file( "camera_refined.txt" );
  vcl_string RC_in_file( "camera_RC.txt" );
  vcl_string K_in_file( "camera_K.txt" );
  vcl_string RC_out_file( "camera_RC_refined.txt" ); // rot and transl
  vcl_string K_out_file( "camera_K.txt" );
  vcl_string refined_world_points_file( "world_pts_refined.txt" );

  // Read the world points.
  vcl_vector<vgl_point_3d<double> > world_points;

  vcl_ifstream wpsifs( world_points_file.c_str() );
  if (!wpsifs) {
    vcl_cerr << " Error opening file  " << world_points_file << vcl_endl;
    return 1;
  }
  vul_awk wps( wpsifs );
  while( wps ){
    world_points.push_back( vgl_point_3d<double>(
      atof(wps[0]), atof(wps[1]), atof(wps[2]) ) );
    ++wps;
  }
  unsigned num_world_points = world_points.size();

  vcl_vector< vgl_homg_point_3d<double> > world_points_h;
  for( unsigned p =0; p < num_world_points; p++ )
    world_points_h.push_back( vgl_homg_point_3d<double>(world_points[p] ) );

  // Read the image_points
  vcl_vector<vgl_point_2d<double> > image_points;
  vcl_ifstream ipsifs( image_points_file.c_str() );
  if (!ipsifs) {
    vcl_cerr << " Error opening file  " << world_points_file << vcl_endl;
    return 1;
  }
  vul_awk ips( ipsifs );
  for( unsigned wp = 0; wp < num_world_points; wp++ ){
    if( ips.NF() == 0 ){ ++ips; wp--; continue; }
    vgl_point_2d<double> new_ip( atof(ips[0]), atof(ips[1]) );
    image_points.push_back( new_ip );
    ++ips;
  }

  // Read a camera. Hardcoded for now.

  // Capitol2 output from RANSAC and point-tangents:
//  K_gt =

//        2200           0         -60
//           0        2200         190
//           0           0           1

//  Rot =

//   0.952842927512352   0.270685375971845   0.137185213213953
//   0.243128852901616  -0.410412760752826  -0.878891191614735
//  -0.181600430550948   0.870798939506831  -0.456870321401466

//  C =

//   1.0e+02 *

//   0.454942228559678
//  -3.080739406727877
//   1.577163390842495


  
//  For capitol2 building
//
//  const double k_block[] = {
//        2200,           0,         -60,
//           0,        2200,         190,
//           0,           0,           1
//  };
//  vnl_matrix_fixed<double,3,3> k_vnl(k_block);

//  vpgl_calibration_matrix<double> K(k_vnl);

//  vgl_point_3d<double> C(0.454942228559678e2, -3.080739406727877e2 , 1.577163390842495e2);

//  const double rot_block[] = {
//   0.952842927512352,   0.270685375971845,   0.137185213213953,
//   0.243128852901616,  -0.410412760752826,  -0.878891191614735,
//  -0.181600430550948,   0.870798939506831,  -0.456870321401466
//  };

//  vnl_matrix_fixed<double,3,3> rot_matrix(rot_block);
//  vgl_rotation_3d<double> Rot(rot_matrix);


  vnl_matrix_fixed<double,3,3> k_vnl;

  vcl_ifstream kifs( K_in_file.c_str() );
  if (!kifs) {
    vcl_cerr << " Error opening file  " << K_in_file << vcl_endl;
    return 1;
  }

  bool retval = k_vnl.read_ascii(kifs);
  if (!retval) {
    vcl_cerr << " Error reading file  " << K_in_file << vcl_endl;
    return 1;
  }

  vcl_cout << "Read K: " << k_vnl << vcl_endl;

  vpgl_calibration_matrix<double> K(k_vnl);

  vnl_matrix_fixed<double,3,3> rot_matrix;

  vcl_ifstream rotifs( RC_in_file.c_str() );
  if (!rotifs) {
    vcl_cerr << " Error opening file  " << RC_in_file << vcl_endl;
    return 1;
  }

  retval = rot_matrix.read_ascii(rotifs);
  if (!retval) {
    vcl_cerr << " Error reading file  " << RC_in_file << vcl_endl;
    return 1;
  }

  vcl_cout << "Read R: " << rot_matrix << vcl_endl;

  vgl_rotation_3d<double> Rot(rot_matrix);

  vnl_vector_fixed<double,3> c_v;
  retval = c_v.read_ascii(rotifs);
  if (!retval) {
    vcl_cerr << " Error reading C from file  " << RC_in_file << vcl_endl;
    return 1;
  }

  vcl_cout << "Read C: " << c_v << vcl_endl;

  vgl_point_3d<double> C(c_v[0],c_v[1],c_v[2]);

  vpgl_perspective_camera<double> camera_in(K,C,Rot);

  // Optimize the cameras and world points 

  vpgl_perspective_camera<double>  refined_camera;

  vcl_vector< vgl_point_2d<double> > exp_img_points;
  for( unsigned dp = 0; dp < num_world_points; dp++ ){
    exp_img_points.push_back( image_points[dp] );

  refined_camera = 
    vpgl_optimize_camera::opt_orient_pos(camera_in, world_points_h, image_points);

  // perhaps we could have used vpgl_optimize_camera::opt_orient_pos,
  // without optimizing the 3D points.
  }

  // Compute image errors.
  for( unsigned wp = 0; wp < num_world_points; wp++ ){
    vgl_homg_point_2d<double> proj_wp_h;
    proj_wp_h = refined_camera.project(vgl_homg_point_3d<double>( world_points[wp] ) );

    vgl_point_2d<double> proj_wp( proj_wp_h.x()/proj_wp_h.w(), proj_wp_h.y()/proj_wp_h.w() );
    float this_error = vgl_distance( proj_wp, image_points[wp] );
    vcl_cerr << proj_wp << ' ' << this_error << '\n';
  }
  vcl_cerr << "\n\n";


  // Write to file.
  vcl_ofstream cofs( camera_out_file.c_str() );
  cofs << vcl_setprecision(20);
  cofs << refined_camera.get_matrix();

  vcl_ofstream c_rc_ofs( RC_out_file.c_str() );
  c_rc_ofs << vcl_setprecision(20);

  vgl_point_3d<double> cpt(refined_camera.camera_center());
  c_rc_ofs << refined_camera.get_rotation().as_matrix();
  c_rc_ofs <<  vnl_vector_fixed<double,3> (cpt.x(),cpt.y(),cpt.z()) << '\n';

  // Should be the same as the input one.
  vcl_ofstream c_k_ofs( K_out_file.c_str() );
  c_k_ofs << vcl_setprecision(20);
  c_k_ofs << refined_camera.get_calibration().get_matrix() << vcl_endl;

  vcl_ofstream wpofs( refined_world_points_file.c_str() );
  for( unsigned wp = 0; wp < num_world_points; wp++ )
    wpofs << world_points[wp].x() << ' ' << world_points[wp].y() << ' ' << world_points[wp].z() << '\n';

  return 0;
}
