#include <vcl_iostream.h>

#include <vcl_string.h>
#include <vcl_vector.h>
#include<vcl_cstdio.h>
#include <vcl_vector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include "btpl_hdr.h"

// Estimate the transfer function from a stacked png image.
int main( int argc, char* argv[] )
{ 
  vcl_vector<vcl_string> image_files;
  vcl_vector<vcl_string> g_files;

  image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_16.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_02.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_03.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_01.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_05.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_06.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_07.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_08.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_09.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_10.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_11.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_12.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_13.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_14.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_15.png");
  //image_files.push_back("E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_16.png");
  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam16\\all_g.txt");
  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam16\\05_g.txt");
  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam16\\07_g.txt");
  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam16\\13_g.txt");
  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam16\\23_g.txt");
  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam16\\43_g.txt");
  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam16\\84_g.txt");  
  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam16\\16_g.txt");
  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam16\\32_g.txt");  

  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam02_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam03_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam01_g05.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam05_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam06_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam07_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam08_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam09_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam10_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam11_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam12_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam13_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam14_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam15_g.txt");
  //g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\lambda_2\\cam16_g.txt");

  vcl_vector<vcl_string> shutter_files;
  //vcl_string shutter_file = "E:\\dome_images\\shutter_speed\\shutter_speeds32.txt";
  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds.txt");
  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds05.txt");
  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds07.txt");
  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds13.txt");
  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds23.txt");
  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds43.txt");
  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds84.txt");
  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds16.txt");
  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds32.txt");
 
  vcl_string debug_file = "E:\\dome_images\\debug.txt";
  int zmin = 30, zmax = 230;

  /*for (unsigned i = 0 ; i<image_files.size(); i++)*/
  for (unsigned i = 0 ; i<9; i++)
  {
    vcl_string  image_file = image_files[0];
    vcl_string  g_file = g_files[i];
    vcl_string  shutter_file = shutter_files[i];
    // Parse the png image into sub images.
    
    vil_image_view<vxl_byte> stacked_image = vil_load( image_file.c_str() );
    int image_width = stacked_image.ni();
    int image_height = stacked_image.nj()/12;
    vcl_vector< vil_image_view<vxl_byte> > imgs;
    vcl_vector<float> shutter_speeds;
    vcl_ifstream ssif( shutter_file.c_str() );
    for( int img = 0; img < 12; img++ ){

      float new_shutter; ssif >> new_shutter;
      if( new_shutter == 0 ) continue;

      bool is_missing = true;
      vil_image_view<vxl_byte> new_img( image_width, image_height );
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

    btpl_hdr hdr_converter;
    hdr_converter.z_min_ = zmin; hdr_converter.z_max_ = zmax;
    hdr_converter.inspection_file = debug_file;
    hdr_converter.estimate_g( imgs, shutter_speeds );
    hdr_converter.save_g( g_file );
    vcl_cout<< g_file<< vcl_endl;
  }
};
