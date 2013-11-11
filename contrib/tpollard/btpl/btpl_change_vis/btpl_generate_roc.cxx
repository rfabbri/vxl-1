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
    vcl_cout<< "Usage : vam_generate_roc truechange_dir change_dir roc_file\n";
    return -1;
  }

  vcl_ofstream roc_stream( argv[3] );

  // Get the image names.
  vcl_vector< vcl_string > truechange_names, change_names;
  vcl_string s1(argv[1]);
  s1 += "/*.*";
  vcl_cerr << "reading images from " << argv[1] << '\n';
  for( vul_file_iterator fit = s1; fit; ++fit ){
    if( vul_file::is_directory(fit()) )
      continue;
    vcl_string image_name = fit();
    if( image_name.find( ".jpg" ) > 1000 &&
        image_name.find( ".png" ) > 1000 &&
        image_name.find( ".tif" ) > 1000 ){
      vcl_cerr << " cannot read image " << image_name << '\n';
      continue;
    }
    truechange_names.push_back( image_name );
  }

  vcl_string s2(argv[2]);
  s2 += "/*.*";
  vcl_cerr << "reading images from " << argv[2] << '\n';
  for( vul_file_iterator fit = s2; fit; ++fit ){
    if (vul_file::is_directory(fit()))
      continue;
    vcl_string image_name = fit();
    if( image_name.find( ".jpg" ) > 1000 &&
        image_name.find( ".png" ) > 1000 &&
        image_name.find( ".tif" ) > 1000 ){
      vcl_cerr << " cannot read image " << image_name << '\n';
      continue;
    }
    change_names.push_back( image_name );
  }

  // Calculate the errors for each roc point.
  int num_roc_points = 20;
  for( int r = 0; r < num_roc_points; r++ ){
    vcl_cerr << "processing roc point " << r << ", image ";
    int change_marked_change = 0, change_marked_nonchange = 0,
      nonchange_marked_nonchange = 0, nonchange_marked_change = 0;
    double this_roc_thresh = (r+1)/(double)num_roc_points;

    for( int img = 0; img < change_names.size(); img++ ){
      vcl_cerr << img << ' ';

      // Load the true change and detected change images.
      vil_image_view<double> true_change = vil_convert_to_grey_using_average( 
        vil_load( truechange_names[img].c_str() ) ); 
      vil_image_view<double> prob_change = vil_convert_to_grey_using_average( 
        vil_load( change_names[img].c_str() ) ); 
      unsigned image_height = true_change.nj();
      unsigned image_width = true_change.ni();

      // Threshold this image.
      vil_image_view<vxl_byte> detected_change(image_width, image_height);
      for( int x = 0; x < image_width; x++ ){
        for( int y = 0; y < image_height; y++ ){
          if( prob_change(x,y) < this_roc_thresh*255 ) 
            detected_change(x,y) = 255;
          else
            detected_change(x,y) = 0;
        }
      }
    
      // Tally all of the changes.
      for( int x = 0; x < image_width; x++ ){
        for( int y = 0; y < image_height; y++ ){
          if( true_change(x,y) == 255.0 ) continue;
          if( true_change(x,y) == 0 && detected_change(x,y) == 255 )
            change_marked_change++;
          if( true_change(x,y) == 0 && detected_change(x,y) == 0 )
            change_marked_nonchange++;
          if( true_change(x,y) != 0 && detected_change(x,y) == 255 )
            nonchange_marked_change++;
          if( true_change(x,y) != 0 && detected_change(x,y) == 0 )
            nonchange_marked_nonchange++;
        }
      }

    } //img
    
    double percent_change_marked_change = change_marked_change/
      (double)(change_marked_change+change_marked_nonchange);
    double percent_nonchange_marked_change = nonchange_marked_change/
      (double)(nonchange_marked_change+nonchange_marked_nonchange);
    roc_stream << percent_change_marked_change << "\t" <<
      percent_nonchange_marked_change << '\n';
    vcl_cerr << '\n';

  } // r

  return 0;
}

