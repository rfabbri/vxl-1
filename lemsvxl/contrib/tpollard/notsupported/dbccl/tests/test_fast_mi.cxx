#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include "../dbccl_fast_mi.h"

#include <vil/vil_load.h>
#include <vil/vil_save.h>

static void test_fast_mi()
{
  dbccl_fast_mi_params params;
  dbccl_fast_mi fmi( &params );

  // Testing compute_mi.
  if( true ){
    vil_image_view<vxl_byte> image( params.mi_mask_diameter(), 
      params.mi_mask_diameter() );
    vnl_matrix<int> mi_mask( params.mi_mask_diameter(), 
      params.mi_mask_diameter() );
    for( int i = 0; i < params.mi_mask_diameter(); i++ ){
      for( int j = 0; j < params.mi_mask_diameter(); j++ ){
        mi_mask(i,j) = (int)floor(255.99*rand()/(float)RAND_MAX);
        image(i,j) = mi_mask(j,i);
      }
    }
    // Step through below to make sure its working properly.
    float mi = fmi.compute_mi( image, vgl_point_2d<int>(0,0), mi_mask );
  }

  
  // Testing compute_region_mi.
  if( false ){
    vil_image_view<vxl_byte> image1 = vil_load( "f:\\testing\\input\\dbccl1.png" );
    vil_image_view<vxl_byte> image2 = vil_load( "f:\\testing\\input\\dbccl2.png" );
    vgl_point_2d<int> starting_point( 926, 133 );
    vnl_matrix<int> mi_mask( 
      params.mi_mask_diameter(), params.mi_mask_diameter() );
    for( int i = 0; i < params.mi_mask_diameter(); i++ ){
      for( int j = 0; j < params.mi_mask_diameter(); j++ ){
        mi_mask(j,i) = image1( 
          starting_point.x() - params.mi_mask_rad + i,
          starting_point.y() - params.mi_mask_rad + j );
      }
    }
    vnl_matrix<float> region_mi;
    fmi.compute_mi_region( image1, starting_point, mi_mask, region_mi );
    float region_mi_max = 0;
    for( int i = 0; i < region_mi.cols(); i++ )
      for( int j = 0; j < region_mi.rows(); j++ )
        if( region_mi(j,i) > region_mi_max ) region_mi_max = region_mi(j,i);
    vil_image_view<vxl_byte> image3( region_mi.cols(), region_mi.rows() );
    for( int i = 0; i < region_mi.cols(); i++ )
      for( int j = 0; j < region_mi.rows(); j++ )
        image3(i,j) = (int)floor( 255.99*region_mi(j,i)/region_mi_max );
    // Inspect the saved image.
    vil_save( image3, "f:\\testing\\output\\dbccl1.png" );
  }
}

TESTMAIN( test_fast_mi );
