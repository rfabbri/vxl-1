// This is /contrib/biotree/proj/bioproj_gz_filter.h

#ifndef bioproj_gz_filter_h_
#define bioproj_gz_filter_h_

//: 
// \file    bioproj_gz_filter.h
// \brief   a class for the first derivative of Gaussian filter
// \author  H. Can Aras
// \date    2006-07-16
// 

#include "bioproj_filter.h"
class bioproj_gz_filter: public bioproj_filter{
public:
  //: constructor getting the parameters one-by-one
  bioproj_gz_filter(double sigma, unsigned extent);
};

#endif
