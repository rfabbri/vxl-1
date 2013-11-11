// This is brcv/seg/dbacm3d/dbacm3d_image_filters.h
#ifndef dbacm3d_image_filters_h_
#define dbacm3d_image_filters_h_

//:
// \file
// \brief Various image filters used in level set framework
// \author Jesse Funaro (jdfunaro@lems.brown.edu)
//         (after dbacm/dbacm_image_filters by  Nhon Trinh (ntrinh@lems.brown.edu))
// \date 12/22/2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vnl/vnl_math.h>

#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/vil3d_transform.h>
#include <dbil3d/algo/dbil3d_gauss_filter.h>
#include <vil3d/algo/vil3d_grad_1x3.h>



//: Functor class to compute malladi image force function
class dbacm3d_malladi_scale_functor
{
public:
  float operator()(float x) const {return x<0 ? 1.0f : 1.0f/(1.0f+x); }
  double operator()(double x) const {return x<0 ? 1.0 : 1.0/(1.0+x); }
};

//: Functor class to compute square of a number
class dbacm3d_square_functor
{
public:
  float operator()(float x) const {return x*x; }
  double operator()(double x) const {return x*x; }
};



//: Functor class to compute cubic of a number
class dbacm3d_cube_functor
{
public:
  float operator()(float x) const {return x*x*x; }
  double operator()(double x) const {return x*x*x; }
};

//: Functor class to compute power of a number
class dbacm3d_power_functor
{
public:
  dbacm3d_power_functor(int p):p_(p){}
  float operator()(float x) const {return vcl_pow(x, p_); }
  double operator()(double x) const {return vcl_pow(x, p_); }
private:
  int p_;
};



// ------------------------------------------------------------------
//: Compute image force using gradient of image
// g = 1 / (1 + (|dG*I|/M)^p
// dG : Gaussian smoothing
// M : down_scale value
// p: exponent
// (for now) keep gauss_sigma < 0.7
// Reference: Malladi, R., J. A. Sethian, and B. C. Vemuri, "Shape Modeling with front propagation: A Level Set Approach".
template <class inT, class outT>
inline void dbacm3d_malladi_image_force(const vil3d_image_view<inT >& src_im,
                                      double param_T,
                                      int exponent,
                                      int direction,
                                      double sigma,
                                      vil3d_image_view<outT >& gmap,
                                      vil3d_image_view<outT >& gx,
                                      vil3d_image_view<outT >& gy,
                                      vil3d_image_view<outT >& gz)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();
  unsigned np = src_im.nplanes();

  vil3d_image_view<outT > grad_i, grad_j, grad_k;

  {
  vil3d_image_view<outT> smoothed(ni,nj,nk,np);

  //blur by the appropriate gaussian
  dbil3d_gauss_filter(src_im,sigma,smoothed);

  //compute image gradient values
  vil3d_grad_1x3(smoothed,grad_i,grad_j,grad_k);
  }

  gmap.set_size(ni,nj,nk,np);
  gx.set_size(ni,nj,nk,np);
  gy.set_size(ni,nj,nk,np);
  gz.set_size(ni,nj,nk,np);


  //compute image gradient magnitude and values for G = 1 - 1/1+(|dG*I|/T)^power
  //G = 1/1+(|dG*I|/T)^power will give speeds for going "in", that is,
  //slowing near edges, while G = 1- 1/1+(|dG*I|/T)^power will give speeds for going "out"
  //i.e. slowing near non-edges

  bool out = (direction > 0);

  if(exponent == 2){
    float grad_squared;
    for(int p = 0; p < static_cast<int>(np); ++p){
    for(int z = 0; z < static_cast<int>(nk); ++z){
    for(int y = 0; y < static_cast<int>(nj); ++y){
    for(int x = 0; x < static_cast<int>(ni); ++x){
            grad_squared = grad_i(x,y,z)*grad_i(x,y,z) + 
                           grad_j(x,y,z)*grad_j(x,y,z) + 
                           grad_k(x,y,z)*grad_k(x,y,z);
            gmap(x,y,z) = (int)out - direction/(1 + grad_squared/(param_T*param_T));
    }
    }
    }
    }
  }
  else{
    float img_gradient_mag;
    for(int p = 0; p < static_cast<int>(np); ++p){
    for(int z = 0; z < static_cast<int>(nk); ++z){
    for(int y = 0; y < static_cast<int>(nj); ++y){
    for(int x = 0; x < static_cast<int>(ni); ++x){
            img_gradient_mag        = vcl_sqrt(grad_i(x,y,z)*grad_i(x,y,z) + 
                                               grad_j(x,y,z)*grad_j(x,y,z) + 
                                               grad_k(x,y,z)*grad_k(x,y,z));
            gmap(x,y,z) = (int)out - direction/(1 + vcl_pow(img_gradient_mag/param_T,exponent));
    }
    }
    }
    }
  }

  //compute gradients of G
  vil3d_grad_1x3(gmap,gx,gy,gz);
  return;
}






// ------------------------------------------------------------------
//: Compute image force using gradient of image and feature map
// similary to malladi_image_force but also take in an feature map
template <class inT, class outT>
inline void dbacm3d_malladi_image_force_with_feature_map(
  const vil3d_image_view<inT >& src_im,                                    
  double M,
  int exponent,
  double gauss_sigma,
  const vil3d_image_view<bool>& feature_map,
  vil3d_image_view<outT >& gmap,
  vil3d_image_view<outT >& gx,
  vil3d_image_view<outT >& gy)
{
vcl_cerr << "Not yet implemented " << vcl_endl;
/*
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned np = src_im.nplanes();

  vil3d_image_view<outT > smoothed;

  // smooth source image using gaussian filter
  vil3d_gauss_filter_5tap_params gauss_params(gauss_sigma);
  vil3d_gauss_filter_5tap<inT, outT>(src_im, smoothed, gauss_params);

  //compute image gradient values 
  vil3d_image_view<outT > grad_i, grad_j, grad_mag;
  vil3d_sobel_3x3(smoothed, grad_i, grad_j);

  // gradient magnitude
  vil3d_math_image_vector_mag(grad_i, grad_j, grad_mag);


  // set grad_mag to max wherever there is feature
  outT min_value, max_value;
  vil3d_math_value_range(grad_mag, min_value, max_value);
  for (unsigned p=0; p<grad_mag.nplanes(); ++p)
  {
    for (unsigned j=0; j<grad_mag.nj(); ++j)
    {
      for (unsigned i=0; i<grad_mag.ni(); ++i)
      {
        if (feature_map(i, j, p) == true)
        {
          grad_mag(i, j, p) = max_value;
        }
      }
    }
  }

  //compute image gradient magnitude and values for 
  // g = 1/1+(|dG*I|/M)^p
  //                 1
  // g = ---------------------------
  //       1 +   (|dG * I| /M)^p
  //            

  if (M == 0) M = 1;
  vil3d_math_scale_values(grad_mag, (1.0/M));


  // compute (|dG*I|/M)^p
  // special treatment for cases exponent=2 and exponent=3
  switch (exponent)
  {
  case (2):
    vil3d_transform(grad_mag, dbacm3d_square_functor());
    break;
  case (3):
    vil3d_transform(grad_mag, dbacm3d_cube_functor());
    break;
  default:
    vil3d_transform(grad_mag, dbacm3d_power_functor(exponent));
    break;
  };

  vil3d_transform(grad_mag, dbacm3d_malladi_scale_functor());

  // g
  gmap = grad_mag;

  // make sure the min is ZERO
  vil3d_math_value_range(gmap, min_value, max_value);
  vil3d_math_scale_and_offset_values(gmap, outT(1), -min_value);

  //compute gradients of g
  vil3d_sobel_3x3(gmap, gx, gy);
  return;
  */
}





#endif // dbacm3d_image_filters_h_

