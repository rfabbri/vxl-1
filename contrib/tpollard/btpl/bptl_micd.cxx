//This is micd.cxx (mutual information for change detection)

//This file implements change detection through polynomial warping and mutual information
//References:
//
//Mark J. Carlotto, "Detection and analysis of change in remotely sensed imagery with application to wide area surveillance,"
//IEEE Transactions on Image Processing, vol 6, no 1, 1997.
//
//Joselito Chua and Peter Tischer, The use of local information gain for automatic detection of changes in a scene. 
//In Proceedings of the 2004 International Conference on Computational Intelligence for Modelling, Control and Automation (CIMCA'04), Queensland, Australia, July 12-14, 2004. ISBN 1740881885.
//
//

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_sstream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vgl/vgl_polygon.h>
#include <brip/brip_vil_float_ops.h>


int main()
{
//  //1. Read in base image and warped images
//
//  vcl_string scene = "embassy";
//  vcl_string date = "07JAN25";
//
//  vcl_stringstream base_img_ss;
//  vcl_stringstream warp_img_ss;
//  vcl_stringstream prob_img_ss;
//
//  base_img_ss << "C:\\baghdad\\micd\\" << scene << "\\" << date<< "\\base.png";
//  vcl_string base_img_name = base_img_ss.str();
//
//  warp_img_ss << "C:\\baghdad\\micd\\" << scene << "\\" << date<< "\\registered3.png";
//  vcl_string warp_img_name = warp_img_ss.str();
//
//  //output image
//  prob_img_ss << "C:\\baghdad\\micd\\" << scene << "\\" << date<< "\\changes_test2.png";  
//  vcl_string prob_img_name = prob_img_ss.str();
//
//  vil_image_view<vxl_byte> base_img_int = vil_convert_to_grey_using_average(
//    vil_load( base_img_name.c_str() )); 
//  
//  vil_image_view<vxl_byte> warp_img_int = vil_convert_to_grey_using_average(
//    vil_load( warp_img_name.c_str() )); 
// 
//  vil_image_view<float> base_img = brip_vil_float_ops::convert_to_float(base_img_int); 
//  //vil_image_view<float> warp_img = brip_vil_float_ops::convert_to_float(warp_img_int);
//
//  //delete
// vil_image_view<float> warp_img= base_img;
//
//  vil_image_view<float> prob_img_f( base_img.ni(), base_img.nj() );
//  vil_image_view<vxl_byte> prob_img( base_img.ni(), base_img.nj() );
//
//  //  2. Define bounding box
//
//  vcl_vector< vgl_point_2d<int> > region_bounds;
//  region_bounds.push_back(vgl_point_2d<int>(11, 11));
//  region_bounds.push_back(vgl_point_2d<int>(base_img.ni()-11,11));
//  region_bounds.push_back(vgl_point_2d<int>(base_img.ni()-11, base_img.nj()-11));
//  region_bounds.push_back(vgl_point_2d<int>(11, base_img.nj()-11));
//  
//  float x[4], y[4];
//
//  for( int i = 0; i < 4; i++ ){
//    x[i] = (float)region_bounds[i].x();
//    y[i] = (float)region_bounds[i].y();
//  }
//  vgl_polygon<float> pg( x, y, 4 );
//
// //  3. Iterate through the images
// //  4. if pixels are within bounding-box, calculate mutual info
// //  5  Convert float image into vxl-byte
//
//  float max_mi = 0.0f;
////entrpy of a
//  for( unsigned i = 0; i < base_img.ni(); i++ ){
//    for( unsigned j = 0; j < base_img.nj(); j++ ){
//      if( pg.contains(i,j) ){
//        float mi =  brip_vil_float_ops::minfo_i(i, j,i ,j, 0, 0, base_img, warp_img);
//        if (mi > max_mi)
//          max_mi = mi;
//        prob_img_f(i,j) = mi;
//        if (mi<0)
//        {
//           mi= 0;
//        }
//      }
//      else 
//        prob_img_f(i,j) = 0;
//    }
//  }
//
//
//  for( unsigned i = 0; i < base_img.ni(); i++ ){
//    for( unsigned j = 0; j < base_img.nj(); j++ ){
//      prob_img(i,j) = int(floor(255* (prob_img_f(i,j)/max_mi)));
//    }
//  }
//
// //  6 Save image
//  vil_save(prob_img, prob_img_name.c_str());


    //1. Read in base image and warped images

  vil_image_view<vxl_byte> img1 = vil_load("D:/images_multiview/baghdad/hiafa_warped/jan.png");
  vil_image_view<vxl_byte> img2 = vil_load("D:/images_multiview/baghdad/hiafa_warped/jan_warped_aff.png");
  vcl_string out("D:/images_multiview/baghdad/hiafa_warped/jan_change_aff.tiff");
  vgl_point_2d<int> img1pt(631,780), img2pt(553,807);
  vgl_vector_2d<int> d=img2pt-img1pt;

  int num_bins = 32;

  double num_samples = img1.ni()*img1.nj();

  // Get the joint histogram

  vnl_matrix<double> p12(num_bins, num_bins, 0.0 );

  vnl_vector<double> p1(num_bins,0.0), p2(num_bins,0.0);

  for( int i = 0; i < img1.ni(); i++ ){

    for( int j = 0; j < img1.nj(); j++ ){
      if( i+d.x() < 0 || j+d.y() < 0 || i+d.x() >= img2.ni() || j+d.y() >= img2.nj() ) continue;

      int b1 = (int)floor( num_bins*img1(i,j)/256.0 );

      int b2 = (int)floor( num_bins*img2(i+d.x(),j+d.y())/256.0 );

      p12(b1,b2) += 1.0;

      p1(b1) += 1.0;

      p2(b2) += 1.0;

    }

  }



  // Get the e1 errors.

  vil_image_view<float> img1e( img1.ni(), img1.nj() );
  float max_int = 0;

  for( int i = 0; i < img1.ni(); i++ ){
    for( int j = 0; j < img1.nj(); j++ ){
      if( i+d.x() < 0 || j+d.y() < 0 || i+d.x() >= img2.ni() || j+d.y() >= img2.nj() ) continue;

      int b1 = (int)floor( num_bins*img1(i,j)/256.0 );
      int b2 = (int)floor( num_bins*img2(i+d.x(),j+d.y())/256.0 );

      img1e(i,j) = p12(b1,b2)/(double)num_samples; 
      if (img1e(i,j) > max_int)
        max_int=img1e(i,j);
    }
  }

  vil_save( img1e, out.c_str() );
  vcl_cout<<max_int<<vcl_endl;

}
