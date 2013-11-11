#include "bioproj_nu_gyy_filter.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include "error_functions.h"

bioproj_nu_gyy_filter::bioproj_nu_gyy_filter(double sigma, double theta, unsigned extent)
{
  error_functions err;
  half_kernel_size_ = int(vcl_ceil(extent * sigma));
  full_kernel_size_ = 2*half_kernel_size_ + 1;
  filter_ = new double[full_kernel_size_];
  int index = 0;
  double common = -(vcl_sin(theta)*vcl_sin(theta)) / (vnl_math::pi*(sigma*sigma*sigma*sigma));
  for(int xi = -half_kernel_size_; xi <= half_kernel_size_; xi++)
  {
    double integral_value = vcl_exp(-(xi*xi)/(2*(sigma*sigma))) * 
                            vcl_sqrt(vnl_math::pi_over_2) * sigma * 
                            err.erfi(xi/(vnl_math::sqrt2*sigma), 140);
    double term = ((xi/sigma)*(xi/sigma));
    filter_[index++] = common * (2 - term - xi/(sigma*sigma) * (3-term) * integral_value);
  }
}

