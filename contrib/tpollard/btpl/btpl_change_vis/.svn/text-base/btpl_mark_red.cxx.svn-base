#include <vcl_string.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vgl/vgl_point_2d.h>

//-------------------------------------------
int main( int argc, char* argv[] )
{
  vcl_string input_dir = "D:\\images_multiview\\prov_c\\seq1";
  vcl_string prob_dir = "D:\\results\\prov_c_large\\raw";
  vcl_string output_dir = "D:\\results\\prov_c_large\\red";
  double prob_thresh = .50;
  double prob_image_scale = .5;

  // Get all the image names.
  vcl_vector< vcl_string > input_images, prob_images, output_images;
  char line_buffer[256];
  input_dir += "/*.*"; prob_dir += "/*.*";
  for( vul_file_iterator fit = input_dir; fit; ++fit ){
    if( vul_file::is_directory(fit()) )
      continue;
    vcl_string image_name = fit();
    if( image_name.find( ".jpg" ) > 1000 &&
        image_name.find( ".png" ) > 1000 &&
        image_name.find( ".tif" ) > 1000 ){
      continue;
    }
    input_images.push_back( image_name );
  }
  for( vul_file_iterator fit = prob_dir; fit; ++fit ){
    if( vul_file::is_directory(fit()) )
      continue;
    vcl_string image_name = fit();
    if( image_name.find( ".jpg" ) > 1000 &&
        image_name.find( ".png" ) > 1000 &&
        image_name.find( ".tif" ) > 1000 ){
      continue;
    }
    prob_images.push_back( image_name );
  }
  int num_images = input_images.size(); 
  if( num_images > prob_images.size() ) num_images = prob_images.size();
  for( int i = 0; i < num_images; i++ ){
    vcl_stringstream image_name;
    image_name << output_dir << "//" << i << ".png";
    output_images.push_back( image_name.str() );
  }

  // Process each image pair.
  for( int img = 0; img < num_images; img++ ){
    
    vcl_cerr << "Processing image: " << input_images[img] << '\n';
    vil_image_view<vxl_byte> input_image = vil_load( input_images[img].c_str() ); 
    vil_image_view<vxl_byte> prob_image = vil_load( prob_images[img].c_str() );
    unsigned image_height = input_image.nj();
    unsigned image_width = input_image.ni();

    vil_image_view<vxl_byte> output_image( image_width, image_height, 3 );
    for( int i = 0; i < image_width; i++ ){
      for( int j = 0; j < image_height; j++ ){
        vgl_point_2d<double> prob_img_pixel( 
          (int)floor( prob_image_scale*i ), (int)floor( prob_image_scale*j ) );
      //int this_color = (int)floor(input_image(i,j)*.75);
      //output_image(i,j,1) = output_image(i,j,2) = this_color;
      //output_image(i,j,0) = this_color + 
      //  (int)floor(.25*(255-input_image(i,image_height+j)));

        float this_prob = 1.0;
        if( prob_image(prob_img_pixel.x(), prob_img_pixel.y()) < 255*prob_thresh ) 
          this_prob = prob_image( prob_img_pixel.x(), prob_img_pixel.y() )/(255*prob_thresh);
        output_image(i,j,0) = 
          (int)floor( input_image(i,j)*this_prob + 255*(1-this_prob) );
        output_image(i,j,1) = (int)floor( input_image(i,j,1)*this_prob );
        output_image(i,j,2) = (int)floor( input_image(i,j,2)*this_prob );
      }
    }
    vil_save( output_image, output_images[img].c_str() );
  }

  return 0;
}

