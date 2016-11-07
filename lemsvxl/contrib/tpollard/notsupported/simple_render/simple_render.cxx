#include <vcl_string.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include "simple_scene.h"


//-------------------------------------------
int main( int argc, char* argv[] )
{
  vcl_string image_directory( "f:\\images" );
  vcl_string camera_file( "f:\\images\\cameras.txt" );
  vcl_string light_file( "f:\\images\\lights.txt" );
  int image_width = 400, image_height = 400;
  int num_images = 40;

  simple_scene scene;
  scene.image_height_ = image_height; scene.image_width_ = image_width;
  scene.directional_lighting_ = true;
  scene.render_360( num_images, image_directory, camera_file, light_file, false );
  return 0;
}

