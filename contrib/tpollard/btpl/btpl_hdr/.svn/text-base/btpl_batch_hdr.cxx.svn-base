#include <vcl_iostream.h>

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include "btpl_hdr.h"


// Convert a directory of stacked png images to hdr.



//This function recevieve a series of files with different exposure times and
//finds which file resolves in a minumun variance
//i.e one file ignores the 5th exposure, another may ignore the 6th, etc...
void get_min_variance(btpl_hdr hdr_converter,
                      vil_image_view<vxl_byte>stacked_image, 
                      vcl_vector<vcl_string>shutter_files, 
                      unsigned &min_var_index)
{
  float min_var = 1.0;

  for(unsigned i=0; i<shutter_files.size(); i++)
  {
    vcl_string shutter_file = shutter_files[i];
    // Parse the png image into sub images.
    int image_width = stacked_image.ni();
    int image_height = stacked_image.nj()/12;
    vcl_vector< vil_image_view<vxl_byte> > imgs;
    vcl_vector<float> shutter_speeds;
    vcl_ifstream ssif( shutter_file.c_str() );
    for( int img = 0; img < 12; img++ ){
      float new_shutter; ssif >> new_shutter;
      if( new_shutter == 0 ) continue;

      vil_image_view<vxl_byte> new_img( image_width, image_height );
      bool is_missing = true;
      for( int x = 0; x < image_width; x++ ){
        for( int y = 0; y < image_height; y++ ){
          new_img(x,y) = stacked_image( x, image_height*img + y );
          if( new_img(x,y) != 255 ) is_missing = false;
        }
      }
      if( is_missing ) 
        continue;
      imgs.push_back( new_img );
      shutter_speeds.push_back( new_shutter );
    }

    // compute variance.
    float avg_var;
    hdr_converter.compute_variance(imgs, shutter_speeds, avg_var);
    if (avg_var < min_var)
    {
      min_var = avg_var;
      min_var_index = i;

    }
  }

}


int main( int argc, char* argv[] )
{  
  // Set these:
  vcl_vector<vcl_string> dirs;
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_109");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_110");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_111");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_112");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_113");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_114");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_115");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_116");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_117");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_118");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_119");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_207");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_208");
  dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_209");


  for( unsigned d= 0; d< dirs.size(); d++)
  {
    vcl_string img_dir = dirs[d];

    //shutter speeds' files : different files ignore different exposures

    vcl_vector<vcl_string> shutter_files;
    shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds.txt");
    shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds05.txt");
    shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds07.txt");
    shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds13.txt");
    shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds23.txt");
    shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds43.txt");
    shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds84.txt");
    shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds16.txt");
    shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds32.txt");

    //g files: one per camera

    vcl_string g1 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_1_g.txt";
    vcl_string g2 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_2_g.txt";
    vcl_string g3 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_3_g.txt";
    vcl_string g4 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_4_g.txt";
    vcl_string g5 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_5_g.txt";
    vcl_string g6 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_6_g.txt";
    vcl_string g7 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_7_g.txt";
    vcl_string g8 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_8_g.txt";
    vcl_string g9 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_9_g.txt";
    vcl_string g10 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_10_g.txt";
    vcl_string g11 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_11_g.txt";
    vcl_string g12 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_12_g.txt";
    vcl_string g13 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_13_g.txt";
    vcl_string g14 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_14_g.txt";
    vcl_string g15 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_15_g.txt";
    vcl_string g16 = "E:\\dome_images\\response_curves\\best\\fitted\\cam_16_g.txt";



    int last_slash, last_period, last_underscore;
    for( vul_file_iterator f= (img_dir+"\\*").c_str(); f; ++f ){

      // Get all needed filenames.
      vcl_string current_file( f() );
      for( int c = current_file.size()-1; c > 0; c-- )
        if( current_file[c] == '.' ){ last_period = c; break; }
        vcl_string current_file_ext, new_file;

        vcl_string camera_string;
        camera_string += current_file[last_period-2];
        camera_string += current_file[last_period-1];
        int camera_index = atoi( camera_string.c_str() );
        vcl_string g_file;

        if( camera_index == 1 ) g_file = g1;
        if( camera_index == 2 ) g_file = g2;
        if( camera_index == 3 ) g_file = g3; 
        if( camera_index == 4 ) g_file = g4;
        if( camera_index == 5 ) g_file = g5;
        if( camera_index == 6 ) g_file = g6;
        if( camera_index == 7 ) g_file = g7;
        if( camera_index == 8 ) g_file = g8;
        if( camera_index == 9 ) g_file = g9;
        if( camera_index == 10 ) g_file = g10;
        if( camera_index == 11 ) g_file = g11;
        if( camera_index == 12 ) g_file = g12;
        if( camera_index == 13 ) g_file = g13;
        if( camera_index == 14 ) g_file = g14;
        if( camera_index == 15 ) g_file = g15;
        if( camera_index == 16 ) g_file = g16;


        btpl_hdr hdr_converter;
        hdr_converter.load_g( g_file );

        for(unsigned int c = 0; c < last_period; c++ )
          new_file += current_file[c];
        new_file += ".tif";

        for(unsigned  int c = last_period+1; c < current_file.size(); c++ )
          current_file_ext += current_file[c];
        if( current_file_ext != "png" )
          continue;

        vcl_cerr << new_file << '\n';

        // Parse the png image into sub images.
        vil_image_view<vxl_byte> stacked_image = vil_load( current_file.c_str() );
        int image_width = stacked_image.ni();
        int image_height = stacked_image.nj()/12;
        vcl_vector< vil_image_view<vxl_byte> > imgs;
        vcl_vector<float> shutter_speeds;

        // here we need to determine which shutter speeds to use
        unsigned min_index = 0;
        get_min_variance( hdr_converter, stacked_image, shutter_files, min_index);
        vcl_string shutter_file = shutter_files[min_index];

        //write a file that indicates which file index was used
        vcl_string index_file = img_dir + "\\index_file_used.txt";
        vcl_ofstream fs;
        fs.open(index_file.c_str() , vcl_ios::app);
        fs << new_file << ' ' << min_index << vcl_endl;

        // get proper png images 

        vcl_ifstream ssif( shutter_file.c_str() );
        for( int img = 0; img < 12; img++ ){
          float new_shutter; ssif >> new_shutter;
          if( new_shutter == 0 ) continue;

          vil_image_view<vxl_byte> new_img( image_width, image_height );
          bool is_missing = true;
          for( int x = 0; x < image_width; x++ ){
            for( int y = 0; y < image_height; y++ ){
              new_img(x,y) = stacked_image( x, image_height*img + y );
              if( new_img(x,y) != 255 ) is_missing = false;
            }
          }
          if( is_missing ) 
            continue;
          imgs.push_back( new_img );
          shutter_speeds.push_back( new_shutter );
        }

        // Convert to HDR and save.
        vil_image_view<float> hdr_img;
        hdr_converter.compute_hdr( imgs, shutter_speeds, hdr_img );
        vil_save( hdr_img, new_file.c_str() );
    }
  }
};
