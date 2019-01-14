#include "bioproj_nu_gyy_filter.h"
#include <cmath>
#include <iostream>
#include <vnl/vnl_math.h>
#include "error_functions.h"

bioproj_nu_gyy_filter::bioproj_nu_gyy_filter(double sigma, double theta, unsigned extent)
{
  error_functions err;
  half_kernel_size_ = int(std::ceil(extent * sigma));
  full_kernel_size_ = 2*half_kernel_size_ + 1;
  filter_ = new double[full_kernel_size_];
  int index = 0;
  double common = -std::pow(std::sin(theta),2.0) / (vnl_math::pi*std::pow(sigma, 4));
  for(int xi = -half_kernel_size_; xi <= half_kernel_size_; xi++)
  {
    double integral_value = std::exp(-std::pow(xi,2.0)/(2*std::pow(sigma,2.0))) * 
                            std::sqrt(vnl_math::pi_over_2) * sigma * 
                            err.erfi(xi/(std::sqrt(2.0)*sigma), 140);
    double term = std::pow(xi/sigma,2.0);
    filter_[index++] = common * (2 - term - xi/std::pow(sigma,2.0) * (3-term) * integral_value);
  }
}

bioproj_nu_gyy_filter::~bioproj_nu_gyy_filter()
{
  delete [] filter_;
}
