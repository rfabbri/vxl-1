#include "bioproj_gzz_filter.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>

bioproj_gzz_filter::bioproj_gzz_filter(double sigma, unsigned extent)
{
  half_kernel_size_ = int(vcl_ceil(extent * sigma));
  full_kernel_size_ = 2*half_kernel_size_ + 1;
  filter_ = new double[full_kernel_size_];
  int index = 0;
  double common = 1 / (vcl_sqrt(2*vnl_math::pi)*vcl_pow(sigma,5.0));
  for(int z = -half_kernel_size_; z <= half_kernel_size_; z++)
    filter_[index++] = common * ((z*z)-(sigma*sigma)) 
                       * vcl_exp(-(z*z)/(2*(sigma*sigma)));
}

