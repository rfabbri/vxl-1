#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_point_2d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include <mbl/mbl_stats_1d.h>
#include <vifa/vifa_incr_var.h>

#include "btpl_hdr.h"


//----------------------------------------------------
btpl_hdr::btpl_hdr() :
  inspection_file( "NONE" )
{
  g_.set_size(256);
};


//----------------------------------------------------
void
btpl_hdr::estimate_g(
  const vcl_vector< vil_image_view<vxl_byte> >& imgs,
  const vcl_vector<float>& shutter_speeds )
{    
  // Parameters to be set.
  int sub_window_size = 25;
  float lambda = 2;

  int num_imgs = imgs.size();
  assert( shutter_speeds.size() == num_imgs );

  //get the smallest image size
  unsigned int img_ni = 640;
  unsigned int img_nj = 480;

  for (unsigned i = 0; i< imgs.size(); i++)
  {
    if (imgs[i].ni()<img_ni)
      img_ni = imgs[i].ni();
    
    if (imgs[i].nj()<img_nj)
      img_nj = imgs[i].nj();

  }
  

  // Get a bunch of sample pixels locations.
  vcl_vector< vgl_point_2d<int> > sample_pixels;
  int num_pixels = 0;
  for( unsigned int i = 0; i < img_ni-sub_window_size; i+= sub_window_size ){
    for( unsigned int j = 0; j < img_nj-sub_window_size; j+= sub_window_size ){

      num_pixels+=2;
      sample_pixels.push_back( vgl_point_2d<int>(
        i+(int)floor( sub_window_size*rand()/(float)RAND_MAX ),
        j+(int)floor( sub_window_size*rand()/(float)RAND_MAX ) ) );

      int max_int = 0, min_int = 255;
      vgl_point_2d<int> max_pixel, min_pixel;
      for( int di = 0; di < sub_window_size; di++ ){
        for( int dj = 0; dj < sub_window_size; dj++ ){
          int this_int = imgs[imgs.size()-1]( i+di, j+dj );
          if( this_int < min_int ){
            min_int = this_int; min_pixel = vgl_point_2d<int>( i+di, j+dj ); }
          if( this_int > max_int ){
            max_int = this_int; max_pixel = vgl_point_2d<int>( i+di, j+dj ); }
        }
      }
      sample_pixels.push_back( min_pixel ); sample_pixels.push_back( max_pixel );
    }
  }

  // Compile a list of sample pixel intensities from the images.
  vnl_matrix<int> Z( num_pixels, num_imgs );
  for( int img = 0; img < num_imgs; img++ ){
    for( int p = 0; p < num_pixels; p++ ){
      int t_x = sample_pixels[p].x(), ty = sample_pixels[p].y();
      int this_pixel = imgs[img]( sample_pixels[p].x(), sample_pixels[p].y() );
      if( this_pixel < z_min_ ) this_pixel = z_min_;
      if( this_pixel > z_max_ ) this_pixel = z_max_;
      Z(p,img) = this_pixel;
      //if( Z(p,img) < z_min_ ) z_min_ = Z(p,img);
      //if( Z(p,img) > z_max_ ) z_max_ = Z(p,img);
    }
  }

  // Get log shutter speeds.
  vnl_vector<float> S(num_imgs);
  for( int img = 0; img < num_imgs; img++ )
    S(img) = log( shutter_speeds[img] );

  // Set up the solution matrices and vectors.
  vnl_matrix<float> A( num_pixels*num_imgs+256+1, 256+num_pixels, 0.0 );
  vnl_vector<float> b( num_pixels*num_imgs+256+1, 0.0 );
  int k = 0;
  for( int i = 0; i < num_pixels; i++ ){
    for( int j = 0; j < num_imgs; j++ ){
      float w = zweight( Z(i,j)+1 );
      A( k, Z(i,j)+1 ) = w;
      A( k, 256+i ) = -w;
      b(k) = w*S(j);
      k++;
    }
  }

  A(k, 128) = 1.0;
  k++;

  for( int i = 0; i < 256-2; i++ ){
    float w = zweight( i+1 );
    A(k,i)=lambda*w;
    A(k,i+1)=-2*lambda*w;
    A(k,i+2)=lambda*w;
    k++;
  }

  // Solve and extract solution.
  vnl_svd<float> Asvd(A);
  vnl_vector<float> x = Asvd.solve(b);
  for( int i = 0; i < 255; i++ )
    g_(i) = x(i);

  // Output the function g and samples.
  if( inspection_file != "NONE" ){
    vcl_ofstream ofs( inspection_file.c_str() );
    for( int img = 0; img < num_imgs; img++ )
      for( int p = 0; p < num_pixels; p++ )
        ofs << x(256+p) + S(img) << ' ' << Z(p,img) << '\n';
  }
};


//----------------------------------------------------
void 
btpl_hdr::load_g( 
  vcl_string file_name )
{
  vcl_ifstream fs( file_name.c_str() );
  fs >> z_min_; fs >> z_max_;
  for( int z = 0; z < 256; z++ )
    fs >> g_(z);
};


//----------------------------------------------------
void 
btpl_hdr::save_g(
  vcl_string file_name )
{
  vcl_ofstream fs( file_name.c_str() );
  fs << z_min_ +4 << '\n' << z_max_ - 4<< '\n';
  for( int z = 0; z < 256; z++ )
    fs << g_(z) << '\n';
};


//----------------------------------------------------
void
btpl_hdr::compute_hdr(
  const vcl_vector< vil_image_view<vxl_byte> >& imgs,
  const vcl_vector<float>& shutter_speeds,
  vil_image_view<float>& hdr_img)
{
  int img_ni = imgs[0].ni(), img_nj = imgs[0].nj();
  int num_imgs = imgs.size();
  float temp;

  hdr_img = vil_image_view<float>( img_ni, img_nj );
  for( int i = 0; i < img_ni; i++ ){
    for( int j = 0; j < img_nj; j++ ){
      float lnE = 0;
      float w_sum = 0;
      bool over_exposed =true;
      for( int img = 0; img < num_imgs; img++ ){
        int z = imgs[img](i,j);
        over_exposed = (over_exposed && (z>z_max_));
        float w = zweight( z );
        lnE+= w*( g_(z)-log( shutter_speeds[img] ) );
        w_sum += w;
      
      }

      if (over_exposed)
        vcl_cerr<< "over exposed pixel!!! \n";
      if (w_sum == 0)
      {
        lnE = g_(z_min_) - log( shutter_speeds[num_imgs - 1]);
      }

      else
        lnE/=w_sum;
 
        
      if(vnl_math_isnan(exp(lnE)))
           hdr_img(i,j) = 0;
      hdr_img(i,j) = exp(lnE);
      temp = exp(lnE);
    }
  }
};

//----------------------------------------------------
void
btpl_hdr::compute_variance_image(
  const vcl_vector< vil_image_view<vxl_byte> >& imgs,
  const vcl_vector<float>& shutter_speeds,
  vil_image_view<float>& var_img,
  float &avg_var)
{
  avg_var = 0.0;

  int img_ni = imgs[0].ni(), img_nj = imgs[0].nj();
  int num_imgs = imgs.size();

  var_img = vil_image_view<float>( img_ni, img_nj );
  for( int i = 0; i < img_ni; i++ ){
    for( int j = 0; j < img_nj; j++ ){
      float mean = 0.0;
      float variance = 0.0;
      float lnE = 0;
      float w_sum = 0;
      for( int img = 0; img < num_imgs; img++ ){
        int z = imgs[img](i,j);
        float w = zweight( z );
        w_sum+= w;
        //calculate variance
        lnE= ( g_(z)-log( shutter_speeds[img] ) );
        mean = mean + w*lnE;
        variance = variance + w*(lnE * lnE);
       
      }
      mean = mean/w_sum;
      variance = variance/w_sum - mean*mean;
      var_img(i,j) = variance;
      avg_var = avg_var + variance;
    }
  }
  avg_var = avg_var/ (img_ni * img_nj);

};

void
btpl_hdr::compute_variance(
  const vcl_vector< vil_image_view<vxl_byte> >& imgs,
  const vcl_vector<float>& shutter_speeds,
  float &avg_var)
{
  avg_var = 0.0;

  int img_ni = imgs[0].ni(), img_nj = imgs[0].nj();
  int num_imgs = imgs.size();

  for( int i = 0; i < img_ni; i++ ){
    for( int j = 0; j < img_nj; j++ ){
      float mean = 0.0;
      float variance = 0.0;
      float lnE = 0;
      float w_sum = 0;
      for( int img = 0; img < num_imgs; img++ ){
        int z = imgs[img](i,j);
        float w = zweight( z );
        w_sum+= w;
        //calculate variance
        lnE= ( g_(z)-log( shutter_speeds[img] ) );
        mean = mean + w*lnE;
        variance = variance + w*(lnE * lnE);
       
      }
      mean = mean/w_sum;
      variance = variance/w_sum - mean*mean;
      avg_var = avg_var + variance;
    }
  }
  avg_var = avg_var/ (img_ni * img_nj);

};

//----------------------------------------------------
//void
//btpl_hdr::compute_variance1(
//  const vcl_vector< vil_image_view<vxl_byte> >& imgs,
//  const vcl_vector<float>& shutter_speeds,
//  vil_image_view<float>& var_img,
//  float &avg_var)
//{
//  mbl_stats_1d  stats;
//  mbl_stats_1d  global_stats;
//  
//  double variance;
//
//  int img_ni = imgs[0].ni(), img_nj = imgs[0].nj();
//  int num_imgs = imgs.size();
//
//  var_img = vil_image_view<float>( img_ni, img_nj );
//  for( int i = 0; i < img_ni; i++ ){
//    for( int j = 0; j < img_nj; j++ ){
//      float lnE = 0;
//      float w_sum = 0;
//      for( int img = 0; img < num_imgs; img++ ){
//        int z = imgs[img](i,j);
//        float w = zweight( z );
//        //calculate variance
//        lnE= w*( g_(z)-log( shutter_speeds[img] ) );
//        stats.obs(exp(lnE));
//      }
//      variance = stats.variance();
//      var_img(i,j) = variance;
//      global_stats.obs(variance);
//    }
//  }
//  avg_var = global_stats.mean();
//
//};



void
btpl_hdr::compute_variance1(
  const vcl_vector< vil_image_view<vxl_byte> >& imgs,
  const vcl_vector<float>& shutter_speeds,
  vil_image_view<float>& var_img,
  float &avg_var)
{
 
  //vifa_incr_var global_stats;
  //
  //double variance;

  //int img_ni = imgs[0].ni(), img_nj = imgs[0].nj();
  //int num_imgs = imgs.size();

  //var_img = vil_image_view<float>( img_ni, img_nj );
  //for( int i = 0; i < img_ni; i++ ){
  //  for( int j = 0; j < img_nj; j++ ){
  //    vifa_incr_var  stats;
  //    float lnE = 0;
  //    float w_sum = 0;
  //    for( int img = 0; img < num_imgs; img++ ){
  //      int z = imgs[img](i,j);
  //      float w = zweight( z );
  //      //calculate variance
  //      lnE= w*( g_(z)-log( shutter_speeds[img] ) );
  //      stats.add_sample(exp(lnE));
  //    }
  //    variance = stats.get_var();
  //    var_img(i,j) = variance;
  //    global_stats.add_sample(variance);
  //  }
  //}
  //avg_var = global_stats.get_mean();

};
////Triangular weigth
////-----------------------------------------------
float 
btpl_hdr::zweight( 
  int z )
{
  if( z <= z_min_ || z >= z_max_ ) return 0.0;
  float z_mid = .5*(z_max_+z_min_);
  float w;
  if( z <= z_mid ) w = z-z_min_;
  if( z > z_mid ) w = z_max_-z;
  return w/(z_max_-z_min_);
};

//Gaussian weight
//-----------------------------------------------
//float 
//btpl_hdr::zweight( 
//  int z )
//{
//  if( z <= z_min_ || z >= z_max_ ) return 0.0;
//  float z_mid = .5*(z_max_+z_min_);
//  float w;
//  float s = 50.0;  
//  w = exp(-4.0/(2.0*pow(s,2)) * pow((z - z_mid),2));
//  return w;
//};
