#include <vcl_string.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

//-------------------------------------------
int main( int argc, char* argv[] )
{
  if( argc!=6 ) {
    vcl_cout<<"Usage : line_extractor image_dir comp_image line_number degrees_rotated image_skip\n";
    return -1;
  }
  vcl_string image_dir(argv[1]); image_dir += "/*.*";
  vcl_string comp_image(argv[2]);
  int line_number = atoi(argv[3]);
  float degrees_rotated = atof(argv[4]);
  int image_skip = atoi(argv[5]); if( image_skip < 1 ) image_skip = 1;

  // Get the image names.
  vcl_vector< vcl_string > image_names;
  int image_counter = image_skip;
  for( vul_file_iterator fit = image_dir; fit; ++fit ){
    if (vul_file::is_directory(fit()))
      continue;
    vcl_string image_name = fit();
    if( image_name.find( ".jpg" ) > 1000 &&
        image_name.find( ".png" ) > 1000 &&
        image_name.find( ".tif" ) > 1000 ){
      vcl_cerr << " cannot read image " << image_name << '\n';
      continue;
    }
    if( image_counter == image_skip ){
      image_names.push_back( image_name );
      image_counter = 1;
    }
    else image_counter++;
  }

  int line_length = vil_load_image_resource( image_names[0].c_str() )->ni();
  vil_image_view<vxl_byte> comp_view( line_length+2, image_names.size() );
  for( int i = 0; i < image_names.size(); i++ ){

    int image_number = i;
    vcl_cerr << '\n' << image_names[image_number];

    // Get the angle for this image.
    float this_angle = i*degrees_rotated/(image_names.size()-1.0);
    while( this_angle < 0 || this_angle > 360 ) 
      this_angle -= 360*this_angle/fabs(this_angle);
    int bit1 = (int)floor(this_angle/10);
    int bit2 = (int)floor(((this_angle/10)-bit1)*100);
    assert( fabs( bit1*10+bit2/10.0-this_angle ) < 1.0 );
    comp_view(0,i) = bit1; comp_view(1,i) = bit2;

    // Copy the line from the image.
    vil_image_view<vxl_byte> this_image = vil_load( image_names[image_number].c_str() );
    for( int j = 0; j < line_length; j++ )
      comp_view(2+j,i) = this_image( j, line_number );
  }
  vil_save( comp_view, comp_image.c_str() );

  return 0;
}

