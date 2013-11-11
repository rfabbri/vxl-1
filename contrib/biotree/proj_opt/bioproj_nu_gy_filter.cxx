#include "bioproj_nu_gy_filter.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_adaptsimpson_integral.h>
#include "bioproj_filter_integrant.h"

bioproj_nu_gy_filter::bioproj_nu_gy_filter(double sigma, double theta, unsigned extent, double increment)
{
  double half_extent = int(vcl_ceil(extent * sigma));
  half_kernel_size_ = int(vcl_ceil(extent * sigma) / increment);
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
    double term1 = -vcl_sin(theta) / (vnl_math::pi*(sigma*sigma*sigma*sigma));
    double term2 = (1-(xi*xi)/(sigma*sigma)) * integral_val;
    filter_[index++] = term1 * (xi + term2);
  }
}

