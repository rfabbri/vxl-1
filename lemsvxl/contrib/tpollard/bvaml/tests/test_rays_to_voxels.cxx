#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <vpgl/vpgl_perspective_camera.h>
#include "../vpgl_rays_to_voxels.h"
#include <vgl/algo/vgl_rotation_3d.h>


static void test_rays_to_voxels()
{
  /*
  vpgl_rays_to_voxels rtv_dummy( vpgl_perspective_camera<double>(), 
    vgl_point_3d<double>(0,0,0), .3, vgl_vector_3d<int>(1,1,1) );

  // Test reorder
  vgl_point_3d<double> reordered_point( 1, 2, 3 );
  vgl_point_3d<double> unreordered_point = reordered_point;
  rtv_dummy.reorder( reordered_point, 1 );
  TEST( "\nreorder test 1.\n", 
    reordered_point.x() != unreordered_point.x(), true );
  rtv_dummy.reorder( reordered_point, -1 );
  TEST( "\nreorder test 2.\n", 
    reordered_point.x() == unreordered_point.x(), true );
  rtv_dummy.reorder( reordered_point, 2 );
  TEST( "\nreorder test 3.\n", 
    reordered_point.x() != unreordered_point.x(), true );
  rtv_dummy.reorder( reordered_point, -2 );
  TEST( "\nreorder test 4.\n", 
    reordered_point.x() == unreordered_point.x(), true );


  // Test find_middle_voxel
  double vx1 = 10.5;
  double vy1 = 4.5;
  double vx2 = 11.5;
  double vy2 = 5.6;
  vgl_point_3d<int> fmv_result = rtv_dummy.find_middle_voxel(
    vx1, vy1, vx2, vy2, 0 );
  TEST( "\nfind_middle_voxel test 1.\n", 
    fmv_result.x() == 10 && fmv_result.y() == 5, true );

  vx1 = 10.5;
  vy1 = 4.5;
  vx2 = 11.5;
  vy2 = 5.4;
  fmv_result = rtv_dummy.find_middle_voxel(
    vx1, vy1, vx2, vy2, 0 );
  TEST( "\nfind_middle_voxel test 2.\n", 
    fmv_result.x() == 11 && fmv_result.y() == 4, true );

  vx1 = 10.5;
  vy1 = 4.5;
  vx2 = 9.5;
  vy2 = 3.6;
  fmv_result = rtv_dummy.find_middle_voxel(
    vx1, vy1, vx2, vy2, 0 );
  TEST( "\nfind_middle_voxel test 3.\n", 
    fmv_result.x() == 9 && fmv_result.y() == 4, true );

  vx1 = 10.5;
  vy1 = 4.5;
  vx2 = 9.5;
  vy2 = 3.4;
  fmv_result = rtv_dummy.find_middle_voxel(
    vx1, vy1, vx2, vy2, 0 );
  TEST( "\nfind_middle_voxel test 4.\n", 
    fmv_result.x() == 10 && fmv_result.y() == 3, true );

  vx1 = 10.5;
  vy1 = 4.5;
  vx2 = 11.5;
  vy2 = 3.4;
  fmv_result = rtv_dummy.find_middle_voxel(
    vx1, vy1, vx2, vy2, 0 );
  TEST( "\nfind_middle_voxel test 5.\n", 
    fmv_result.x() == 10 && fmv_result.y() == 3, true );

  vx1 = 10.5;
  vy1 = 4.5;
  vx2 = 11.5;
  vy2 = 3.6;
  fmv_result = rtv_dummy.find_middle_voxel(
    vx1, vy1, vx2, vy2, 0 );
  TEST( "\nfind_middle_voxel test 6.\n", 
    fmv_result.x() == 11 && fmv_result.y() == 4, true );

  // Full test: test correctness of projection.
  {
    double P0_mat[12] = {
      3.50825, -2.43955, -0.414899, 268.533,
      -1.34703, -1.49979, -2.7268, 521.183,
      0.00120144, 0.00113598, -0.000974648, 1 };
    vpgl_proj_camera<double> P0( P0_mat );
    vpgl_rays_to_voxels rtv0( P0, 
      vgl_point_3d<double>(-20,-20,-20), 2.0, vgl_vector_3d<int>(40,40,40) );

    int incorrect_proj = 0;
    for( int i = 0; i < 1000; i++ ){
      for( int j = 0; j < 1000; j++ ){
        vcl_vector< vgl_point_3d<int> > these_ray_voxels;
        rtv0.get_ray_voxels( vgl_point_2d<int>(i,j), these_ray_voxels, true );
        if( these_ray_voxels.size() == 0 ) continue;
        for( int v = 0; v < these_ray_voxels.size(); v++ ){
          vgl_point_3d<int> this_voxel = these_ray_voxels[v];
          vgl_homg_point_3d<double> voxel_coord(
            -19+2.0*this_voxel.x(), -19+2.0*this_voxel.y(), -19+2.0*this_voxel.z() );
          vgl_homg_point_2d<double> img_coord = P0.project( voxel_coord );
          if( abs( img_coord.x()/img_coord.w() - i ) > 10 ||
            abs( img_coord.y()/img_coord.w() - j ) > 10 )
            incorrect_proj++;
        }
        
      }
    }
    TEST( "\nfull test: all voxels project correctly.\n", 
      incorrect_proj, 0 );
  }

  // Full test: test that all voxels are assigned to something.
  {
    double P1_mat[12] = {
      3.50825, -2.43955, -0.414899, 268.533,
      -1.34703, -1.49979, -2.7268, 521.183,
      0.00120144, 0.00113598, -0.000974648, 1 };
    vpgl_proj_camera<double> P1( P1_mat );
    vpgl_rays_to_voxels rtv1( P1, 
      vgl_point_3d<double>(-20,-20,-20), 2.0, vgl_vector_3d<int>(40,40,40) );

    bool* seen_voxels = new bool[40*40*40];
    int num_breaks = 0;
    for( int i = 0; i < 40*40*40; i++ )
      seen_voxels[i] = false;
    for( int i = 0; i < 1000; i++ ){
      for( int j = 0; j < 1000; j++ ){
        vcl_vector< vgl_point_3d<int> > these_ray_voxels;
        rtv1.get_ray_voxels( vgl_point_2d<int>(i,j), these_ray_voxels, false );
        if( these_ray_voxels.size() == 0 ) continue;
        for( int v = 0; v < these_ray_voxels.size(); v++ ){
          vgl_point_3d<int> this_voxel = these_ray_voxels[v];
          if( v > 0 ){
            vgl_point_3d<int> prev_voxel = these_ray_voxels[v-1];
            if( abs( this_voxel.x()-prev_voxel.x() ) > 1 ||
              abs( this_voxel.y()-prev_voxel.y() ) > 1 ||
              abs( this_voxel.z()-prev_voxel.z() ) > 1 ) num_breaks++;
          }

          seen_voxels[ this_voxel.x()+this_voxel.y()*40+this_voxel.z()*40*40 ] = true;
        }
      }
    }
    int num_unseen_voxels = 0;
    for( int i = 0; i < 40*40*40; i++ )
      if( seen_voxels[i] == false ) num_unseen_voxels++;
    TEST( "\nfull test: all voxels allocated to rays.\n", 
      num_unseen_voxels, 0 );
    TEST( "\nfull test: no breaks in rays.\n", 
      num_breaks, 0 ); 
  }
*/
  {
    vpgl_calibration_matrix<double> K( 20, vgl_point_2d<double>(5,5) );
    vpgl_perspective_camera<double> C( 
      K, vgl_point_3d<double>(0,0,10), vgl_rotation_3d<double>() );
    C.look_at( vgl_homg_point_3d<double>(0,0,0), vgl_vector_3d<double>(1,0,0) );

    vgl_point_3d<double> volume_corner(-1,-1,-1);
    double voxel_length = .4;
    vgl_vector_3d<int> num_voxels(5,5,5);

    vpgl_rays_to_voxels rtv( C, 
      volume_corner, voxel_length, num_voxels );

    for( double i = -1; i < 1; i+=.2 ){
      for( double j = -1; j < 1; j+=.2 ){
        vgl_homg_point_2d<double> p = C.project( vgl_homg_point_3d<double>(i,j,1) );
        vcl_cerr << '\n' << p.x()/p.w() << ' ' << p.y()/p.w();
      }
    }

    for( int i = 0; i < 10; i++ ){
      for( int j = 0; j < 10; j++ ){
        vcl_cerr << "\npoint " << i << ' ' << j << '\n';
        vcl_vector< vgl_point_3d<int> > ray_voxels;
        rtv.get_ray_voxels( vgl_point_2d<int>(i,j), ray_voxels, true );
        for( unsigned k = 0; k < ray_voxels.size(); k++ ){
          vgl_point_3d<double> voxel_coords = volume_corner + voxel_length*
            vgl_vector_3d<double>( ray_voxels[k].x(), ray_voxels[k].y(), ray_voxels[k].z() );
          vgl_homg_point_2d<double> proj = C.project( 
            vgl_homg_point_3d<double>( voxel_coords ) );
          vcl_cerr << ray_voxels[k] << "   " <<  proj.x()/proj.w() << ' ' << 
            proj.y()/proj.w() << '\n';
        }
      }
    }


  }
  // Full test: test uniqueness.
  {
    double P2_mat[12] = {
      3.50825, -2.43955, -0.414899, 268.533,
      -1.34703, -1.49979, -2.7268, 521.183,
      0.00120144, 0.00113598, -0.000974648, 1 };
    vpgl_perspective_camera<double> P2;
    vpgl_perspective_decomposition( 
      vnl_matrix_fixed<double,3,4>( P2_mat ), P2 );

    vpgl_rays_to_voxels rtv2( P2, 
      vgl_point_3d<double>(-20,-20,-20), 2.0, vgl_vector_3d<int>(40,40,40) );

    int uniqueness_errors = 0;
    bool* seen_voxels = new bool[40*40*40];
    for( int i = 0; i < 40*40*40; i++ )
      seen_voxels[i] = false;
    for( int i = 0; i < 1000; i++ ){
      for( int j = 0; j < 1000; j++ ){
        vcl_vector< vgl_point_3d<int> > these_ray_voxels;
        rtv2.get_ray_voxels( vgl_point_2d<int>(i,j), these_ray_voxels, true );
        if( these_ray_voxels.size() == 0 ) continue;
        for( unsigned v = 0; v < these_ray_voxels.size(); v++ ){
          vgl_point_3d<int> this_voxel = these_ray_voxels[v];
          if( seen_voxels[ this_voxel.x()+this_voxel.y()*40+this_voxel.z()*40*40 ] == true )
            uniqueness_errors++;
          seen_voxels[ this_voxel.x()+this_voxel.y()*40+this_voxel.z()*40*40 ] = true;
        }
      }
    }
    int num_unseen_voxels = 0;
    for( int i = 0; i < 40*40*40; i++ )
      if( seen_voxels[i] == false ) num_unseen_voxels++;
    TEST( "\nfull test: all voxels allocated to rays.\n", 
      num_unseen_voxels, 0 );
    TEST( "\nfull test: uniqueness of voxel assignment.\n", 
      uniqueness_errors, 0 );
    delete[] seen_voxels;
  }
}

TESTMAIN( test_rays_to_voxels );
