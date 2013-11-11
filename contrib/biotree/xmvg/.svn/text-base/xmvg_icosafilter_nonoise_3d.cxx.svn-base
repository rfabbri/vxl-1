#include "xmvg_icosafilter_nonoise_3d.h"

xmvg_icosafilter_nonoise_3d::xmvg_icosafilter_nonoise_3d(
                            double radius, double length,
                            vgl_point_3d<double> &center)
{

  double a = (1. + vcl_sqrt(5.0))/2.; 
  vgl_vector_3d<double> orientation1(1, a, 0);

  //vgl_vector_3d<double> orientation2(1, -a, 0);
  vgl_vector_3d<double> orientation2(-1, a, 0);

  vgl_vector_3d<double> orientation3(a, 0, 1);
  vgl_vector_3d<double> orientation4(a, 0, -1);
  vgl_vector_3d<double> orientation5(0, 1, a);

  //vgl_vector_3d<double> orientation6(0, 1, -a);
  vgl_vector_3d<double> orientation6(0, -1, a);

  xmvg_no_noise_filter_descriptor nnfd1(radius, length, center, orientation1);
  xmvg_no_noise_filter_descriptor nnfd2(radius, length, center, orientation2);
  xmvg_no_noise_filter_descriptor nnfd3(radius, length, center, orientation3);
  xmvg_no_noise_filter_descriptor nnfd4(radius, length, center, orientation4);
  xmvg_no_noise_filter_descriptor nnfd5(radius, length, center, orientation5);
  xmvg_no_noise_filter_descriptor nnfd6(radius, length, center, orientation6);

  xmvg_no_noise_filter_3d f1(nnfd1);
  xmvg_no_noise_filter_3d f2(nnfd2);
  xmvg_no_noise_filter_3d f3(nnfd3);
  xmvg_no_noise_filter_3d f4(nnfd4); 
  xmvg_no_noise_filter_3d f5(nnfd5);
  xmvg_no_noise_filter_3d f6(nnfd6);

  vcl_vector<xmvg_no_noise_filter_3d> filters;
  filters_.push_back(f1);
  filters_.push_back(f2);
  filters_.push_back(f3);
  filters_.push_back(f4);
  filters_.push_back(f5);
  filters_.push_back(f6);
  
}
