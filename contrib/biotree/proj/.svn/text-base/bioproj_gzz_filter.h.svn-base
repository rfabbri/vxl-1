// This is /contrib/biotree/proj/bioproj_gzz_filter.h

#ifndef bioproj_gzz_filter_h_
#define bioproj_gzz_filter_h_

//: 
// \file    bioproj_gzz_filter.h
// \brief   a class for the second derivative of Gaussian filter
// \author  H. Can Aras
// \date    2006-07-18
// 

class bioproj_gzz_filter{
public:
  //: constructor getting the parameters one-by-one
  bioproj_gzz_filter(double sigma, unsigned extent);
  ~bioproj_gzz_filter();

  double *filter_;
  int half_kernel_size_;
  int full_kernel_size_;
};

#endif
