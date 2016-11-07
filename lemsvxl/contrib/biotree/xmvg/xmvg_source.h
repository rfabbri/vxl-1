// This is /contrib/biotree/xmvg/xmvg_source.h

#ifndef xmvg_source_h_
#define xmvg_source_h_

//: 
// \file    xmvg_source.h
// \brief   a data structure to hold composite filter response
// \author  J.L. Mundy
// \date    2005-02-26
// 
// \Modifications 
//          Gamze Tunali 12/23/2005 added x_write method and also public getters

#include <vcl_iostream.h>
#include <vcl_string.h>

class xmvg_source
{
  public:
  xmvg_source(double spot_radius=10.0, double kv=20.0, double ma=100, double lambda=10)
    : spot_radius_(spot_radius), kv_(kv), ma_(ma), poisson_lambda_(lambda){}

    double spot_radius() { return spot_radius_; }
    double kv() { return kv_; }
    double ma() {return ma_; }
    double poisson_lambda() { return poisson_lambda_; };
  private:
    double spot_radius_;
    double kv_;
    double ma_;
    double poisson_lambda_;
};

void x_write(vcl_ostream& os, xmvg_source source);

#endif
