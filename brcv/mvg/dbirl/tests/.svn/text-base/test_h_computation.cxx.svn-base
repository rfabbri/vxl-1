// This is brl/bseg/dbinfo/tests/test_h_matrix.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <dbirl/dbirl_h_computation.h>
#include <vcl_cstdlib.h>


static void test_h_computation()
{
  // Make some homographies.
  int num_frames = 5;
  vcl_vector< vgl_h_matrix_2d<double> > h;
  vcl_cerr << "\nTrue homographies:\n";
  vnl_matrix_fixed<double,3,3> id(0.0); id(2,2) = id(1,1) = id(0,0) = 1;
  h.push_back( vgl_h_matrix_2d<double>(id) );
  vcl_cerr << vgl_h_matrix_2d<double>(id) << '\n';
  for( int i = 1; i < num_frames; i++ ){
    vnl_matrix_fixed<double,3,3> new_h(0.0);
    new_h(2,2) = 1;
    new_h(0,0) = rand()/(double)RAND_MAX-.5; 
    new_h(0,1) = rand()/(double)RAND_MAX-.5; 
    new_h(0,2) = rand()/(double)RAND_MAX-.5; 
    new_h(1,0) = rand()/(double)RAND_MAX-.5; 
    new_h(1,1) = rand()/(double)RAND_MAX-.5; 
    new_h(1,2) = rand()/(double)RAND_MAX-.5;
    vcl_cerr << new_h << '\n';
    h.push_back( new_h );
  }
  vcl_vector< vgl_h_matrix_2d<double> > h_inv;
  for( int i = 0; i < num_frames; i++ )
    h_inv.push_back( h[i].get_inverse() );

  // Make some tracks
  vcl_vector< dbinfo_track_geometry_sptr > tracks;
  for( int i = 0; i < 20; i++ ){
    dbinfo_track_geometry* new_track = new dbinfo_track_geometry;
    new_track->set_start_frame(0);
    new_track->set_end_frame(num_frames-1);
    vgl_homg_point_2d<double> first_point(
      rand()/(double)RAND_MAX-.5, rand()/(double)RAND_MAX-.5 );
    vsol_point_2d_sptr first_point_vsol = 
      new vsol_point_2d( first_point.x(), first_point.y() );
    new_track->insert_point( 0, first_point_vsol );
    for( int j = 1; j < num_frames; j++ ){
      vgl_homg_point_2d<double> new_point = h_inv[j]*first_point;
      vsol_point_2d_sptr new_point_vsol = 
        new vsol_point_2d( new_point.x(), new_point.y() );
      new_track->insert_point(j, new_point_vsol );
    }
    tracks.push_back( new_track );
  }

  // Estimate the homographies from the tracks.
  vcl_cerr << "\nEstimated homographies:\n";
  vcl_vector< vgl_h_matrix_2d<double> > h_est;
  dbirl_h_computation hc;
  hc.compute_affine( tracks, h_est, 0 );
  for( int i = 0; i < static_cast<int>(h_est.size()); i++ )
    vcl_cerr << h_est[i] << '\n';

  double d1 = (h[4].get_matrix() - h_est[4].get_matrix()).frobenius_norm();
  TEST_NEAR("Homographies to frame 0 with perfect correspondences", d1,  0 , 1.0e-4);

  // Now compute homographies to 3rd frame.
  vcl_cerr << "\nTrue homographies:\n";
  vcl_vector< vgl_h_matrix_2d<double> >  h2;
  for( int i = 0; i < num_frames; i++ ){
    h2.push_back( h_inv[2]*h[i] );
    vcl_cerr << h2[i] << '\n';
  }
  vcl_cerr << "\nEstimated homographies:\n";
  hc.compute_affine( tracks, h_est, 2 );
  for( int i = 0; i < static_cast<int>(h_est.size()); i++ )
    vcl_cerr << h_est[i] << '\n';
  double d2 = (h2[4].get_matrix() - h_est[4].get_matrix()).frobenius_norm();
  TEST_NEAR("Homographies to frame 2 with perfect correspondences", d2,  0 , 1.0e-4);

  // Now remove some of the track information.
  for( int i = 0; i < 20; i++ ){
    if( rand()/(double)RAND_MAX < .5 ) 
      tracks[i]->set_start_frame(0);
    else
      tracks[i]->set_start_frame(1);
    if( rand()/(double)RAND_MAX < .5 ) 
      tracks[i]->set_end_frame(4);
    else
      tracks[i]->set_end_frame(3);
  }
  vcl_cerr << "\nEstimated homographies:\n";
  hc.compute_affine( tracks, h_est, 2 );
  for( int i = 0; i < static_cast<int>(h_est.size()); i++ )
    vcl_cerr << h_est[i] << '\n';
  double d3 = (h2[4].get_matrix() - h_est[4].get_matrix()).frobenius_norm() +
    (h2[1].get_matrix() - h_est[1].get_matrix()).frobenius_norm();
  TEST_NEAR("Homographies to frame 2 with missing tracks", d3,  0 , 1.0e-4);


}
TESTMAIN(test_h_computation);
