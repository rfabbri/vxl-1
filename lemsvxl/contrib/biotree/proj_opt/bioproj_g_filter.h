// This is /contrib/biotree/proj/bioproj_g_filter.h

#ifndef bioproj_g_filter_h_
#define bioproj_g_filter_h_

//: 
// \file    bioproj_g_filter.h
// \brief   a class for the Gaussian smoothing filter
// \author  H. Can Aras
// \date    2006-07-15
// 

#include "bioproj_filter.h"
class bioproj_g_filter: public bioproj_filter{
public:
  //: constructor getting the parameters one-by-one
  bioproj_g_filter(double sigma, unsigned extent);
};

#endif
