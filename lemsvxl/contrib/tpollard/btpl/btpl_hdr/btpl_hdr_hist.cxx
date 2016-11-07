// This file computes some usfull histograms of hdr data
// the values to a text file
// @author : Isa Restrepo
// Created : 09/1/07


#include <vcl_iostream.h>
//#include <vcl_stringstream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cstdio.h>
#include <vcl_vector.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>

#include <dbsta/dbsta_histogram.h>

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_crop.h>
#include <vil/algo/vil_histogram.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>



void hdr_image_hist()
{
  vcl_string file_in = "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\11808_1_06.tif";
  vcl_string file_out = "E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\hdr_histo.txt";
  vcl_ofstream ofs(file_out.c_str());

  vil_image_view<float> hdr_im(vil_load(file_in.c_str()));

  float min = 1.0;
  float max = 0.0;

  //find the range of image intensity
  for( int i = 0; i < 640; i++ ){
    for( int j = 0; j < 480; j++ ){
      if( hdr_im(i,j)< min) min =hdr_im(i,j);
      if( hdr_im(i,j)> max) max =hdr_im(i,j);
    }
  }

  //get the histogram of image
  vcl_vector<double> histo;
  unsigned nbins = 1000;
  vil_histogram(hdr_im, histo, min, max, nbins);

  //write histogram to file
  ofs<<min <<"\n" << max<<"\n" << nbins <<"\n" ;

  for (unsigned i=0; i < histo.size(); i++) {
    ofs<<histo[i]<<"\n";
  }

}

void hdr_response_hist(vcl_string const &cam)
{
 
  //main image directory

  vcl_string img_dir = "E:\\dome_images\\00_calibration\\cropped_imgs\\png_11006";

  //shutter speeds' files
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
  
    
  vcl_stringstream ss_hdr_file;
  vcl_stringstream ss_png_file;
  ss_hdr_file.clear();
  ss_png_file.clear();
  ss_hdr_file << "E:\\dome_images\\00_calibration\\cropped_imgs\\png_11006\\11006_1_" << cam << ".tif";
  ss_png_file <<  "E:\\dome_images\\00_calibration\\cropped_imgs\\png_11006\\11006_1_" << cam <<".png ";
  vcl_string hdr_file = ss_hdr_file.str();
  vcl_string png_file = ss_png_file.str();
  vcl_string file_out = "E:\\dome_images\\test_hdr_analysis\\all_results\\hdr_response_histo.txt";

  vcl_string index_file = img_dir + "\\index_file_used.txt";
  vcl_ifstream fs( index_file.c_str() );
  vcl_vector<unsigned> indeces;
  unsigned temp_ind = 0;
  for(unsigned i=0; i<16; i++)
  {
    fs >> temp_ind;
    indeces.push_back(temp_ind);
  }
  
  unsigned cam_i = atoi(cam.c_str());
  vcl_string shutter_file = shutter_files[indeces[cam_i - 1]];
  
  //all
  vcl_stringstream ss_out;
  ss_out.clear();
  ss_out<<"E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\10\\new_histos\\hdr_histo_cam" << cam << ".tif";
  vcl_string image_out = ss_out.str();
  vcl_ofstream ofs(file_out.c_str());

  //store shutter speeds
  vcl_vector<float> ss;
  vcl_ifstream ssfs( shutter_file.c_str() );
  float temp_ss = 0.0;

  for (unsigned i= 0; i <12; i++)
  {
    ssfs >> temp_ss;
    ss.push_back(temp_ss);
  }

  //ss.push_back(0.30827);
  //ss.push_back(0.32407);
  //ss.push_back(0.35566);
  //ss.push_back(0.41885);
  //ss.push_back(0.54522);
  //ss.push_back(0.79797);
  //ss.push_back(1.300);
  //ss.push_back(2.310);
  //ss.push_back(4.370);
  //ss.push_back(8.410);
  //ss.push_back(16.610);
  //ss.push_back(32.610);

  //png image intensity
  int y;

  //image definitions
  vil_image_view<float> hdr_image(vil_load(hdr_file.c_str()));
  vil_image_view<vxl_byte> png_strip(vil_load(png_file.c_str()));
  vil_image_view<vxl_byte> png_image;

  //variable instantiation
  float xmax = 9.0;
  float xmin = -9.0;
  int ymax = 256;
  int ymin = -1;

  float x = 0.0;
  float ln_x_t = 0.0;
  //image that defines exposure time being tested
  //unsigned img_num = 7;



  // create histogram 

  unsigned int xbins = 500;
  unsigned int ybins = 255;
  vil_image_view<float> hist_image(xbins, ybins);

  vcl_vector<float> min; 
  min.push_back(xmin);
  min.push_back(ymin);

  vcl_vector<float> max; 
  max.push_back(xmax);
  max.push_back(ymax);

  vcl_vector<unsigned int> nbins;
  nbins.push_back(xbins);
  nbins.push_back(ybins);

  dbsta_histogram<float> hist(2,min,max,nbins);


  float prob;


  ////get max and min values for histogram 
  //for( unsigned int i = 0; i < 640; i++ ){
  //  for( unsigned int j = 0; j < 480; j++ ){
  //    x = hdr_image(i,j);
  //    ln_x_t = log(x * t);
  //    y = png_image(i,j);
  //    vnl_vector_fixed<float, 2> data_temp(ln_x_t, y);
  //    
  //    //test for max and min hist values
  //    if(ln_x_t < xmin) xmin = ln_x_t;
  //    if(ln_x_t > xmax) xmax = ln_x_t;
  //     if(y < ymin) ymin = y;
  //    if(y> ymax) ymax = y;
  //   
  //  }
  //}
  //xmin = xmin - 1;
  //xmax = xmax + 1;
  //ymin = ymin - 1;
  //ymax = ymax + 1;

  for(unsigned img_num = 0; img_num < 12; img_num++)
  {

   // get exposure time and ith exposed image
    double t = ss[img_num];
    if (t == 0) continue;
    int widht = png_strip.ni();
    int height = png_strip.nj()/12;
    png_image = vil_crop(png_strip, 0,widht, (img_num * height), height);

    //individual exposures
   /* vcl_stringstream ss_out;
    ss_out.clear();
    ss_out<<"E:\\dome_images\\test_hdr_analysis\\all_results\\trian_lambda2\\18\\hdr_histo_cam9_im" << img_num << ".tif";
    vcl_string image_out = ss_out.str();
    vcl_ofstream ofs(file_out.c_str());*/

    bool success = false;

    for( unsigned int i = 0; i < widht; i++ ){
      for( unsigned int j = 0; j < height; j++ ){
        x = hdr_image(i,j);
        ln_x_t = log(x * t);
        y = png_image(i,j);

        //check proper range
        if (vnl_math_isnan(ln_x_t))
        {
          vcl_cerr<<"warning, data coordinate out of range\n";
          continue;
        }

        vnl_vector_fixed<float, 2> data_temp(ln_x_t, y);

        //histogram related 
        success = hist.update(data_temp);
        if(!success)
          vcl_cerr<<"histogram update error at data " << i <<vcl_endl;
    
       
      }
    }
  // if no all 
  }
  //vcl_cout << "max x: "<< xmax <<"\n" <<"min x: "<< xmin<< "\n"
  // << " max y: "<< ymax << "\n" << " min y: "<< ymin <<"\n";


  float hist_x = xmin;
  for(unsigned i=0; i<xbins; i++)
  {
    //unsigned ii = i*4;
    hist_x = hist_x + (xmax - xmin)/ xbins;

    float hist_y = ymin;
    for (unsigned j = 0; j < ybins; j++)
    {
      hist_y=  hist_y + (ymax - ymin)/ybins;
      prob = hist.probability(vnl_vector_fixed<float,2>(hist_x,hist_y));

      // hist_image(i-1,(ybins-1)-(j-1)) = prob;
      hist_image(i,(ybins-1)-j) = prob;
      ofs << i <<' ' << j << ' ' << prob<<"\n";

    }
  }
  vil_save(hist_image, image_out.c_str());
  //individual
  //}

}
//int main( int argc, char* argv[] )
//{  
//
//  //hdr_image_hist();
//  
//  hdr_response_hist("01");
//  hdr_response_hist("02");
//  hdr_response_hist("03");
//  hdr_response_hist("04");
//  hdr_response_hist("05");
//  hdr_response_hist("06");
//  hdr_response_hist("07");
//  hdr_response_hist("08");
//  hdr_response_hist("09");
//  hdr_response_hist("10");
//  hdr_response_hist("11");
//  hdr_response_hist("12");
//  hdr_response_hist("13");
//  hdr_response_hist("14");
//  hdr_response_hist("15");
//  hdr_response_hist("16");
//
//
//}

