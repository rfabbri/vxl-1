// This is brcv/seg/dbdet/algo/dbdet_savitzky_golay_filter_2d.h
#ifndef dbdet_savitzky_golay_filter_2d_h
#define dbdet_savitzky_golay_filter_2d_h
//:
//\file
//\brief An implementation of the Savitzky-Golay filtering in 2d.
//       This algorithm performs a least squares fitting of a polynomial to the 
//       data via convolution with appropriate kernels
//\author Amir Tamrakar
//\date 08/26/07
//
//\verbatim
//  Modifications
//\endverbatim

#include <vil/vil_convert.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/algo/vil_convolve_2d.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>

//: Perform Savitzky Golay filtering of the data in src using a polynomial 
//  of the desired order oriented in the direction defined by theta
inline void Savitzky_Golay_filter_2d(vil_image_view<double>& src, vil_image_view<double>& dest, int order, int masksz, double theta)
{
  double ra = vcl_max(1.5, double(masksz/2.0));
  double rb = vcl_max(1.5, double(masksz/4.0));
  double ira2 = 1 / (ra*ra);
  double irb2 = 1 / (rb*rb);
  int wr = masksz;
  double wd = 2*wr+1;
  double sint = vcl_sin(theta);
  double cost = vcl_cos(theta);

  // 1. compute linear filters for coefficients
  // (a) compute inverse of least-squares problem matrix
  vnl_vector<double> xx(2*order+1, 0.0);
  for (int u=-wr; u<=wr; u++){
    for (int v=-wr; v<=wr; v++){
      double ai = -u*sint + v*cost; // distance along major axis
      double bi = u*cost + v*sint; // distance along minor axis
      if (ai*ai*ira2 + bi*bi*irb2 > 1) continue; // outside support
      double cum_prod = 1;
      for (int k=0; k<=2*order; k++){
        xx.put(k, xx.get(k)+cum_prod);
        cum_prod *= ai;
      }
    }
  }

  vnl_matrix<double> A(order+1,order+1);
  for (int i=0; i<=order; i++)
    for (int r=0; r<=order; r++)
      A(r,i) = xx.get(i+r); 

  //compute the inverse
  vnl_matrix_inverse<double> Ainv(A);

  // (b) solve least-squares problem for delta function at each pixel
  vil_image_view<double> filt((int)wd,(int)wd);//order+1 filters for the other coefficients
  filt.fill(0.0);

  vnl_vector<double> yy(order+1, 0.0);
  for (int u=-wr; u<=wr; u++){
    for (int v=-wr; v<=wr; v++){
      double ai = -u*sint + v*cost; // distance along major axis
      double bi = u*cost + v*sint; // distance along minor axis
      if (ai*ai*ira2 + bi*bi*irb2 > 1) continue; // outside support

      double cum_prod = 1;
      for (int k=0; k<=order; k++){
        yy.put(k, cum_prod);
        cum_prod *= ai;
      }

      vnl_vector<double> tmp(Ainv*yy);
      filt(v+wr, u+wr) = tmp.get(0); //only the first coeff filter needed
    }
  }

  // 2. apply the filter to get the fit coefficient at each pixel
  // create a padded image first (because 2d conv reduces the size)
  vil_image_resource_sptr padded_res = vil_new_image_resource(src.ni()+filt.ni()-1, 
                                                              src.nj()+filt.nj()-1, 
                                                              1, VIL_PIXEL_FORMAT_DOUBLE);
  vil_image_view<double> padded_img = padded_res->get_view();
  padded_img.fill(0);
  //padded_res->put_view(padded_img,0,0);
  padded_res->put_view(src,wr,wr);

  //: convolve the image with the kernels to get gradients in x and y directions
  vil_convolve_2d(padded_img, dest, filt, double());
}


#endif // dbdet_savitzky_golay_filter_2d_h
