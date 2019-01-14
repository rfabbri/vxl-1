#include "bioproj_gz_filter.h"
#include <cmath>
#include <iostream>
#include <vnl/vnl_math.h>

bioproj_gz_filter::bioproj_gz_filter(double sigma, unsigned extent)
{
  half_kernel_size_ = int(std::ceil(extent * sigma));
  full_kernel_size_ = 2*half_kernel_size_ + 1;
  filter_ = new double[full_kernel_size_];
  int index = 0;
  double common = -1 / (std::sqrt(2*vnl_math::pi)*std::pow(sigma,3.0));
  for(int z = -half_kernel_size_; z <= half_kernel_size_; z++)
    filter_[index++] = common * z * std::exp(-std::pow(z,2.0)/(2*std::pow(sigma,2.0)));
}

bioproj_gz_filter::~bioproj_gz_filter()
{
  delete [] filter_;
}
