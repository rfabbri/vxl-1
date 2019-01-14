// This is /contrib/biotree/proj/bioproj_filter_integrant.h

#ifndef bioproj_filter_integrant_h_
#define bioproj_filter_integrant_h_

//: 
// \file    bioproj_filter_integrant.h
// \brief   a class for the projected filter integrant
// \author  H. Can Aras
// \date    2006-08-31
// 

#include <vnl/vnl_analytic_integrant.h>
#include <cmath>

class bioproj_filter_integrant : public vnl_analytic_integrant{
public:
  bioproj_filter_integrant(double sigma, double xi) : sigma_(sigma), xi_(xi)
  { 
  }

  double f_(double zeta)
  {
    return std::exp(-(std::pow(xi_,2.0) - std::pow(zeta,2.0)) / (2*std::pow(sigma_,2.0)));
  }
protected:
  double sigma_;
  double xi_;
};

#endif
