#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include "../bvaml_voxel.h"
#include "../bvaml_params.h"
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>


static void test_voxel()
{
  /*
  bvaml_params params;
  params.appearance_model = 2;

  // Test non-noisy normal estimation.
  bvaml_voxel v1( &params );
  vgl_vector_3d<float> true_n1( .2, -.4, .8 );
  float last_error = 0;

  for( int i = 0; i < 50; i++ ){
    vgl_vector_3d<float> light( 
      2*rand()/(float)RAND_MAX-1, 2*rand()/(float)RAND_MAX-1, .5+.5*rand()/(float)RAND_MAX );
    normalize(light); light*=.25+.5*rand()/(float)RAND_MAX;
    if( i == 0 || i == 1 ) light.set( .5, 0, .5 );
    float color = dot_product( true_n1, light );
    if( color < 0 ) continue;
    v1.update( color, .01, light );
    float dist_to_fit = color - dot_product( light, vgl_vector_3d<float>(
      v1.appearance_model[1], v1.appearance_model[2], v1.appearance_model[3] ) );
    last_error = 
      ( true_n1.x()-v1.appearance_model[1] )*( true_n1.x()-v1.appearance_model[1] ) +
      ( true_n1.y()-v1.appearance_model[2] )*( true_n1.y()-v1.appearance_model[2] ) +
      ( true_n1.z()-v1.appearance_model[3] )*( true_n1.z()-v1.appearance_model[3] );
    vnl_matrix_fixed<float,3,3> cov;
    cov(0,0) = v1.appearance_model[4];
    cov(0,1) = v1.appearance_model[5];
    cov(0,2) = v1.appearance_model[6];
    cov(1,0) = v1.appearance_model[5];
    cov(1,1) = v1.appearance_model[7];
    cov(1,2) = v1.appearance_model[8];
    cov(2,0) = v1.appearance_model[6];
    cov(2,1) = v1.appearance_model[8];
    cov(2,2) = v1.appearance_model[9];
    vnl_vector_fixed<float,3> light_vnl( light.x(), light.y(), light.z() );
    float var = dot_product( light_vnl, cov*light_vnl );
    vcl_cerr << "Error: " << last_error << '\n' 
      << "Distance to last light: " << dist_to_fit << '\n'
      << "Variance: " << var << '\n'
      << "Color prob: " << v1.prob( color, light ) << '\n'
      << "Light: " << light_vnl << '\n' << 
      v1.appearance_model << "\n\n";
  } 
  TEST_NEAR( "\ntesting normal estimation from nonnoisy data.\n", last_error, 0, .1 );


  // Test noisy normal estimation.
  bvaml_voxel v2( &params );
  vgl_vector_3d<float> true_n2( .1, 1, .6 );

  for( int i = 0; i < 50; i++ ){
    vgl_vector_3d<float> light( 
      rand()/(float)RAND_MAX, rand()/(float)RAND_MAX, rand()/(float)RAND_MAX );
    normalize(light); light*=.25+.5*rand()/(float)RAND_MAX;
    float color = dot_product( true_n2, light );
    if( rand()/(float)RAND_MAX < .5 ) 
      color = rand()/(float)RAND_MAX;
    v2.update( color, 1.0, light );

    last_error = 
      ( true_n2.x()-v2.appearance_model[1] )*( true_n2.x()-v2.appearance_model[1] ) +
      ( true_n2.y()-v2.appearance_model[2] )*( true_n2.y()-v2.appearance_model[2] ) +
      ( true_n2.z()-v2.appearance_model[3] )*( true_n2.z()-v2.appearance_model[3] );
    vcl_cerr << last_error << '\n' << v2.appearance_model << "\n\n";
  } 

  TEST_NEAR( "\ntesting normal estimation from noisy data.\n", last_error, 0, .1 );


  // Test bimodal normal estimation.
  bvaml_voxel v3( &params );
  vgl_vector_3d<float> true_n3( .1, 1, .6 );
  vgl_vector_3d<float> true_n4( .2, .5, .1 );

  for( int i = 0; i < 200; i++ ){
    vgl_vector_3d<float> light( 
      rand()/(float)RAND_MAX, rand()/(float)RAND_MAX, rand()/(float)RAND_MAX );
    normalize(light); light*=.25+.5*rand()/(float)RAND_MAX;
    float color = dot_product( true_n3, light );
    if( rand()/(float)RAND_MAX < .5 ) 
      color = dot_product( true_n4, light );
    v3.update( color, 1.0, light );

    last_error = 
      ( true_n3.x()-v3.appearance_model[1] )*( true_n3.x()-v3.appearance_model[1] ) +
      ( true_n3.y()-v3.appearance_model[2] )*( true_n3.y()-v3.appearance_model[2] ) +
      ( true_n3.z()-v3.appearance_model[3] )*( true_n3.z()-v3.appearance_model[3] );
    float last_error2 = 
      ( true_n3.x()-v3.appearance_model[6] )*( true_n3.x()-v3.appearance_model[6] ) +
      ( true_n3.y()-v3.appearance_model[7] )*( true_n3.y()-v3.appearance_model[7] ) +
      ( true_n3.z()-v3.appearance_model[8] )*( true_n3.z()-v3.appearance_model[8] );
    if( last_error2 < last_error ) last_error = last_error2;
    vcl_cerr << last_error << '\n' << v3.appearance_model << "\n\n";
  } 

  TEST_NEAR( "\ntesting normal estimation from bimodal data.\n", last_error, 0, .1 );
*/
}

TESTMAIN( test_voxel );
