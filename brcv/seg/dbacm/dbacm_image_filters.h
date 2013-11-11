// This is brcv/seg/dbacm/dbacm_image_filters.h
#ifndef dbacm_image_filters_h_
#define dbacm_image_filters_h_

//:
// \file
// \brief Various image filters used in level set framework
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 12/03/2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vnl/vnl_math.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_sobel_3x3.h>


//: Functor class to compute malladi image force function
class dbacm_malladi_scale_functor
{
public:
  float operator()(float x) const {return x<0 ? 1.0f : 1.0f/(1.0f+x); }
  double operator()(double x) const {return x<0 ? 1.0 : 1.0/(1.0+x); }
};

//: Functor class to compute square of a number
class dbacm_square_functor
{
public:
  float operator()(float x) const {return x*x; }
  double operator()(double x) const {return x*x; }
};



//: Functor class to compute cubic of a number
class dbacm_cube_functor
{
public:
  float operator()(float x) const {return x*x*x; }
  double operator()(double x) const {return x*x*x; }
};

//: Functor class to compute power of a number
class dbacm_power_functor
{
public:
  dbacm_power_functor(int p):p_(p){}
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
inline void dbacm_malladi_image_force(const vil_image_view<inT >& src_im,
                                      double M,
                                      int exponent,
                                      double gauss_sigma,
                                      vil_image_view<outT >& gmap,
                                      vil_image_view<outT >& gx,
                                      vil_image_view<outT >& gy)
{
  //unsigned ni = src_im.ni();
  //unsigned nj = src_im.nj();
  //unsigned np = src_im.nplanes();

  vil_image_view<outT > smoothed;

  // smooth source image using gaussian filter
  vil_gauss_filter_5tap_params gauss_params(gauss_sigma);
  vil_gauss_filter_5tap<inT, outT>(src_im, smoothed, gauss_params);

  //compute image gradient values 
  vil_image_view<outT > grad_i, grad_j, grad_mag;
  vil_sobel_3x3(smoothed, grad_i, grad_j);

  // gradient magnitude
  vil_math_image_vector_mag(grad_i, grad_j, grad_mag);

  //compute image gradient magnitude and values for 
  // g = 1/1+(|dG*I|/M)^p
  //                 1
  // g = ---------------------------
  //       1 +   (|dG * I| /M)^p
  //            

  if (M == 0) M = 1;
  vil_math_scale_values(grad_mag, (1.0/M));


  // compute (|dG*I|/M)^p
  // special treatment for cases exponent=2 and exponent=3
  switch (exponent)
  {
  case (2):
    vil_transform(grad_mag, dbacm_square_functor());
    break;
  case (3):
    vil_transform(grad_mag, dbacm_cube_functor());
    break;
  default:
    vil_transform(grad_mag, dbacm_power_functor(exponent));
    break;
  };

  vil_transform(grad_mag, dbacm_malladi_scale_functor());

  // g
  gmap = grad_mag;

  //compute gradients of g
  vil_sobel_3x3(gmap, gx, gy);
  return;
}






// ------------------------------------------------------------------
//: Compute image force using gradient of image and feature map
// similary to malladi_image_force but also take in an feature map
template <class inT, class outT>
inline void dbacm_malladi_image_force_with_feature_map(
  const vil_image_view<inT >& src_im,                                    
  double M,
  int exponent,
  double gauss_sigma,
  const vil_image_view<bool>& feature_map,
  vil_image_view<outT >& gmap,
  vil_image_view<outT >& gx,
  vil_image_view<outT >& gy)
{
  //unsigned ni = src_im.ni();
  //unsigned nj = src_im.nj();
  //unsigned np = src_im.nplanes();

  vil_image_view<outT > smoothed;

  // smooth source image using gaussian filter
  vil_gauss_filter_5tap_params gauss_params(gauss_sigma);
  vil_gauss_filter_5tap<inT, outT>(src_im, smoothed, gauss_params);

  //compute image gradient values 
  vil_image_view<outT > grad_i, grad_j, grad_mag;
  vil_sobel_3x3(smoothed, grad_i, grad_j);

  // gradient magnitude
  vil_math_image_vector_mag(grad_i, grad_j, grad_mag);


  // set grad_mag to max wherever there is feature
  outT min_value, max_value;
  vil_math_value_range(grad_mag, min_value, max_value);
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
  vil_math_scale_values(grad_mag, (1.0/M));


  // compute (|dG*I|/M)^p
  // special treatment for cases exponent=2 and exponent=3
  switch (exponent)
  {
  case (2):
    vil_transform(grad_mag, dbacm_square_functor());
    break;
  case (3):
    vil_transform(grad_mag, dbacm_cube_functor());
    break;
  default:
    vil_transform(grad_mag, dbacm_power_functor(exponent));
    break;
  };

  vil_transform(grad_mag, dbacm_malladi_scale_functor());

  // g
  gmap = grad_mag;

  // make sure the min is ZERO
  vil_math_value_range(gmap, min_value, max_value);
  vil_math_scale_and_offset_values(gmap, outT(1), -min_value);

  //compute gradients of g
  vil_sobel_3x3(gmap, gx, gy);
  return;
}





#endif // dbacm_image_filters_h_

