#include <iostream>

#include <string>
#include <vector>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include "btpl_hdr.h"


// Convert a directory of stacked png images to hdr.

//int main( int argc, char* argv[] )
//{  
//  // Set these:
//  std::string current_file = "E:\\dome_images\\00_calibration\\cropped_imgs\\png_11108\\11108_1_16.png";
//
// //variance images
//  std::vector<std::string> variance_imgs;
//
//  variance_imgs.push_back( "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11\\variance\\cam14\\all.tif");
//  variance_imgs.push_back( "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11\\variance\\cam14\\05.tif");
//  variance_imgs.push_back( "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11\\variance\\cam14\\07.tif");
//  variance_imgs.push_back( "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11\\variance\\cam14\\13.tif");
//  variance_imgs.push_back( "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11\\variance\\cam14\\23.tif");
//  variance_imgs.push_back( "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11\\variance\\cam14\\43.tif");
//  variance_imgs.push_back( "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11\\variance\\cam14\\84.tif");
//  variance_imgs.push_back( "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11\\variance\\cam14\\16.tif");
//  variance_imgs.push_back( "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11\\variance\\cam14\\32.tif");
//
//  //std::string  variance_img = "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\18\\variance\\cam14_g32.tif";
//
//  //shutter speeds' files
//
//  std::vector<std::string> shutter_files;
//  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds.txt");
//  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds05.txt");
//  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds07.txt");
//  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds13.txt");
//  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds23.txt");
//  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds43.txt");
//  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds84.txt");
//  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds16.txt");
//  shutter_files.push_back("E:\\dome_images\\shutter_speed\\shutter_speeds32.txt");
// 
//  // g_files
//  std::vector<std::string> g_files;
//  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam14\\all_g.txt");
//  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam14\\05_g.txt");
//  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam14\\07_g.txt");
//  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam14\\13_g.txt");
//  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam14\\23_g.txt");
//  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam14\\43_g.txt");
//  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam14\\84_g.txt");  
//  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam14\\16_g.txt");
//  g_files.push_back("E:\\dome_images\\response_curves\\trian_11108\\test\\cam14\\32_g.txt");  
//
//  //variance info file
//  std::string variance_file = "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11\\variance\\cam14\\cam14.txt" ;
// 
//  for (unsigned i= 0; i< g_files.size(); i++)
//  {
//    std::string shutter_file =shutter_files[i];
//    std::string g_file = g_files[i];
//    std::string variance_img = variance_imgs[i];
//
//    btpl_hdr hdr_converter;
//    hdr_converter.load_g( g_file );
//
//    // Parse the png image into sub images.
//    vil_image_view<vxl_byte> stacked_image = vil_load( current_file.c_str() );
//    int image_width = stacked_image.ni();
//    int image_height = stacked_image.nj()/12;
//    std::vector< vil_image_view<vxl_byte> > imgs;
//    std::vector<float> shutter_speeds;
//    std::ifstream ssif( shutter_file.c_str() );
//    for( int img = 0; img < 12; img++ ){
//      float new_shutter; ssif >> new_shutter;
//      if( new_shutter == 0 ) continue;
//
//      vil_image_view<vxl_byte> new_img( image_width, image_height );
//      bool is_missing = true;
//      for( int x = 0; x < image_width; x++ ){
//        for( int y = 0; y < image_height; y++ ){
//          new_img(x,y) = stacked_image( x, image_height*img + y );
//          if( new_img(x,y) != 255 ) is_missing = false;
//        }
//      }
//      if( is_missing ) 
//        continue;
//      imgs.push_back( new_img );
//      shutter_speeds.push_back( new_shutter );
//    }
//
//    // compute variance and save.
//    vil_image_view<float> var_img;
//    float avg_var;
//    hdr_converter.compute_variance_image(imgs, shutter_speeds, var_img, avg_var);
//    vil_save( var_img, variance_img.c_str() );
//    std::ofstream ofs;
//    ofs.open(variance_file.c_str(), std::ios::app);
//    ofs << avg_var << "\n" ;
//  }
//
//};
