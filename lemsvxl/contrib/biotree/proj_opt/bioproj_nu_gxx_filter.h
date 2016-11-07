// This is /contrib/biotree/proj/bioproj_nu_gxx_filter.h

#ifndef bioproj_nu_gxx_filter_h_
#define bioproj_nu_gxx_filter_h_

//: 
// \file    bioproj_nu_gxx_filter.h
// \brief   
// \author  H. Can Aras
// \date    2006-07-18
// 

#include "bioproj_filter.h"
class bioproj_nu_gxx_filter: public bioproj_filter{
public:
  //: constructor getting the parameters one-by-one
  //  sigma is in sensor pixel units
  //  theta is the angle the source rotated from its starting position
  //  extent determines how far we will go along one side of the filter
  bioproj_nu_gxx_filter(double sigma, double theta, unsigned extent);
};

#endif
