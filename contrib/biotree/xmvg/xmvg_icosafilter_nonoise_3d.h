#ifndef xmvg_icosafilter_nonoise_3d_
#define xmvg_icosafilter_nonoise_3d_
//: 
// \file  xmvg_icosafilter_nonoise_3d.h
// \brief  construct a composite filter 3d with 6 atomic filters uniformly scattering on a sphere
// \author    Kongbin Kang
// \date        2005-08-22
// 
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>

class xmvg_icosafilter_nonoise_3d : 
  public xmvg_composite_filter_3d<double, xmvg_no_noise_filter_3d >
{
  private:
    
  public:
    xmvg_icosafilter_nonoise_3d(double radius, 
                                double length, 
                                vgl_point_3d<double> &c);
};
#endif

