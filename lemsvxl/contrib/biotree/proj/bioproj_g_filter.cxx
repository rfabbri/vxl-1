#include "bioproj_g_filter.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>

bioproj_g_filter::bioproj_g_filter(double sigma, unsigned extent)
{
  half_kernel_size_ = int(vcl_ceil(extent * sigma));
  full_kernel_size_ = 2*half_kernel_size_ + 1;
  filter_ = new double[full_kernel_size_];
  int index = 0;
  double common = 1 / (vcl_sqrt(2*vnl_math::pi)*sigma);
  for(int z = -half_kernel_size_; z <= half_kernel_size_; z++)
    filter_[index++] = common * vcl_exp(-vcl_pow(z,2.0)/(2*vcl_pow(sigma,2.0)));
}

bioproj_g_filter::~bioproj_g_filter()
{
  delete [] filter_;
}
