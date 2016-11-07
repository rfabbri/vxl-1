// This is /contrib/biotree/proj/bioproj_nu_gxy_filter.h

#ifndef bioproj_nu_gxy_filter_h_
#define bioproj_nu_gxy_filter_h_

//: 
// \file    bioproj_nu_gxy_filter.h
// \brief   
// \author  H. Can Aras
// \date    2006-07-18
// 

class bioproj_nu_gxy_filter{
public:
  //: constructor getting the parameters one-by-one
  //  sigma is in sensor pixel units
  //  theta is the angle the source rotated from its starting position
  //  extent determines how far we will go along one side of the filter
  bioproj_nu_gxy_filter(double sigma, double theta, unsigned extent);
  ~bioproj_nu_gxy_filter();

  double *filter_;
  int half_kernel_size_;
  int full_kernel_size_;
};

#endif
