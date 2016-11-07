#include "bioproj_nu_gxy_filter.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include "error_functions.h"

bioproj_nu_gxy_filter::bioproj_nu_gxy_filter(double sigma, double theta, unsigned extent)
{
  error_functions err;
  half_kernel_size_ = int(vcl_ceil(extent * sigma));
  full_kernel_size_ = 2*half_kernel_size_ + 1;
  filter_ = new double[full_kernel_size_];
  int index = 0;
  double common = vcl_cos(theta) * vcl_sin(theta) / (vnl_math::pi*vcl_pow(sigma, 4));
  for(int xi = -half_kernel_size_; xi <= half_kernel_size_; xi++)
  {
    double integral_value = vcl_exp(-vcl_pow(xi,2.0)/(2*vcl_pow(sigma,2.0))) * 
                            vcl_sqrt(vnl_math::pi_over_2) * sigma * 
                            err.erfi(xi/(vcl_sqrt(2.0)*sigma), 140);
    double term = vcl_pow(xi/sigma,2.0);
    filter_[index++] = common * (2 - term - xi/vcl_pow(sigma,2.0) * (3-term) * integral_value);
  }
}

bioproj_nu_gxy_filter::~bioproj_nu_gxy_filter()
{
  delete [] filter_;
}
