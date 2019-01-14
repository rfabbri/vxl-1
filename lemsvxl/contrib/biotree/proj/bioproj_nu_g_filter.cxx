#include "bioproj_nu_g_filter.h"
#include <cmath>
#include <iostream>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_adaptsimpson_integral.h>
#include "bioproj_filter_integrant.h"

bioproj_nu_g_filter::bioproj_nu_g_filter(double sigma, unsigned extent, double increment)
{
  double half_extent = int(std::ceil(extent * sigma));
  half_kernel_size_ = int(std::ceil(extent * sigma) / increment);
  full_kernel_size_ = 2*half_kernel_size_ + 1;
  filter_ = new double[full_kernel_size_];
  int index = 0;
  for(double xi = -half_extent; xi <= double(half_extent); xi += increment)
  {
    double integral_val = 0;
    if(xi)
    {
      bioproj_filter_integrant f(sigma, double(xi));
      vnl_adaptsimpson_integral simpson_integral;
      integral_val = simpson_integral.integral(&f, 0.0, double(xi), 1e-06);
    }
    double term1 = 1 / (vnl_math::pi * (std::pow(sigma,2.0)));
    double term2 = xi / (std::pow(sigma,2.0));
    double val = term1 * (1 - term2 * integral_val);
    filter_[index++] = val;
  }
}

bioproj_nu_g_filter::~bioproj_nu_g_filter()
{
  delete [] filter_;
}
