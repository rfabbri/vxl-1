#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include "../bvaml_ray.h"
#include "../bvaml_params.h"
#include "../bvaml_supervoxel.h"

static void test_ray()
{
  bvaml_params params;
  params.appearance_model = 0;
  params.supervoxel_length = 10;

  vnl_vector<float> light(3);
  for( int l = 0; l < 3; l++ )
    light(l) = rand()/(float)RAND_MAX;

  int good_voxel_index = 5;
  bvaml_ray r1( &params, 10 );
  bvaml_ray r2( &params, 10-1 );

  bvaml_supervoxel sv( &params, "D:\\testing\\voxeltest" );
  for( unsigned i = 0; i < 10; i++ ){
 
    bvaml_voxel* v = new bvaml_voxel( &params );
    sv.get_voxel( vgl_point_3d<int>(0,0,i), *v, true, false, true, &light );
    r1.voxels.push_back( v );
    if( i != good_voxel_index ) r2.voxels.push_back( v );
  }
  
  for( int s = 0; s < 100; s++ ){
    if( s % 2 == 0 )
      r1.update( .5, light );
    else
      r2.update( rand()/(float)RAND_MAX, light );
    for( int v = 0; v < 10; v++ )
      vcl_cerr << (*r1.voxels[v]->occupancy_prob) << ' ';
    vcl_cerr << '\n';
  }

}

TESTMAIN( test_ray );
