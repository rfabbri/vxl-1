#include <vcl_string.h>

//TEMP
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

//-------------------------------------------
int main( int argc, char* argv[] )
{
  if( argc != 4 ) {
    vcl_cout<< "Usage : vam_threshold input_image output_image thresh\n";
    return -1;
  }

  vil_image_view<double> prob_change = vil_convert_to_grey_using_average( 
    vil_load( argv[1] ) ); 
  unsigned image_height = prob_change.nj();
  unsigned image_width = prob_change.ni();

  // Threshold this image.
  vil_image_view<vxl_byte> detected_change(image_width, image_height);
  for( int x = 0; x < image_width; x++ ){
    for( int y = 0; y < image_height; y++ ){
      if( prob_change(x,y) < atof( argv[3] )*255 ) 
        detected_change(x,y) = 255;
      else
        detected_change(x,y) = 0;
    }
  }
    
  vil_save( detected_change, argv[2] );

  return 0;
}

