#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include "../bvaml_supervoxel.h"
#include "../bvaml_voxel.h"
#include "../bvaml_params.h"
#include <baml/baml_mog.h>


static void test_supervoxel()
{
  bvaml_params params;
  params.appearance_model = 0;
  params.supervoxel_length = 30;

  // Test reading/writing to disk.
  vcl_vector< vgl_point_3d<int> > pts;
  pts.push_back( vgl_point_3d<int>(0, 0, 0 ) );
  pts.push_back( vgl_point_3d<int>( 29, 29, 29 ) );
  pts.push_back( vgl_point_3d<int>( 29, 28, 29 ) );
  pts.push_back( vgl_point_3d<int>( 29, 29, 28 ) );

  vnl_vector<float> light(3);
  for( int l = 0; l < 3; l++ )
    light(l) = rand()/(float)RAND_MAX;

  bvaml_supervoxel sv( &params, "D:\\testing\\voxeltest" );

  vcl_cerr << "Initial data:\n";
  for( unsigned i = 0; i < pts.size(); i++ ){
 
    bvaml_voxel v( &params );
    sv.get_voxel( pts[i], v, true, true, true, &light );
    v.appearance->update((float)( rand()/(float)RAND_MAX ), (float).1, vnl_vector<float>() );
    (*v.occupancy_prob) = rand()/(float)RAND_MAX;
    v.misc_storage[0] = (float)( 55.0*rand()/(float)RAND_MAX );
    vcl_cerr << v.appearance->prob( .5, vnl_vector<float>() ) << ' ' <<
      (*v.occupancy_prob) << ' ' << v.misc_storage[0] << '\n';
  }
  sv.save();

  vcl_cerr << "\n\nData reloaded:\n";
  for( unsigned i = 0; i < pts.size(); i++ ){
    bvaml_voxel v( &params );
    sv.get_voxel( pts[i], v, true, true, true, &light );
    vcl_cerr << v.appearance->prob( .5, vnl_vector<float>() ) << ' ' <<
      (*v.occupancy_prob) << ' ' << v.misc_storage[0] << '\n';
  }


  /* MEMORY LEAK TESTING
  for( int i = 0; i < 20; i++ ){
    bvaml_supervoxel sv2( &params, "D:\\testing\\voxeltest" );
    sv2.load_occupancies();
    sv2.load_misc_storage();
    sv2.load_local_appearance(light);
    //sv2.save();
  }*/

}

TESTMAIN( test_supervoxel );
