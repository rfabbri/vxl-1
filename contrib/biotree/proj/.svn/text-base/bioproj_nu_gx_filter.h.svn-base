// This is /contrib/biotree/proj/bioproj_nu_gx_filter.h

#ifndef bioproj_nu_gx_filter_h_
#define bioproj_nu_gx_filter_h_

//: 
// \file    bioproj_nu_gx_filter.h
// \brief   a class for the edge filter along x
// \author  H. Can Aras
// \date    2006-07-13
// 

class bioproj_nu_gx_filter{
public:
  //: constructor getting the parameters one-by-one
  //  sigma_r is in sensor pixel units
  //  theta is the angle the source rotated from its starting position
  //  extent determines how far we will go along one side of the filter
  bioproj_nu_gx_filter(double sigma, double theta, unsigned extent, double increment);
  ~bioproj_nu_gx_filter();

  double *filter_;
  int half_kernel_size_;
  int full_kernel_size_;
};

#endif
