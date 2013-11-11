// This is /contrib/biotree/proj/bioproj_nu_g_filter.h

#ifndef bioproj_nu_g_filter_h_
#define bioproj_nu_g_filter_h_

//: 
// \file    bioproj_nu_gx_filter.h
// \brief   a class for the nu_g filter
// \author  H. Can Aras
// \date    2006-07-16
// 

class bioproj_nu_g_filter{
public:
  //: constructor getting the parameters one-by-one
  //  sigma is in sensor pixel units
  //  extent determines how far we will go along one side of the filter
  bioproj_nu_g_filter(double sigma, unsigned extent, double increment);
  ~bioproj_nu_g_filter();

  double *filter_;
  int half_kernel_size_;
  int full_kernel_size_;
};

#endif
