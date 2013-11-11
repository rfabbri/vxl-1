// This is /contrib/biotree/xmvg/xmvg_splat_filter.cxx

#include "xmvg_splat_filter.h"
#include <vnl/vnl_math.h>

xmvg_splat_filter::xmvg_splat_filter(int kernel_size, double cutoff_freq, int type) :
kernel_size_(kernel_size), cutoff_freq_(cutoff_freq)
{
  kernel_.resize(2*kernel_size_+1);
  if(type == RAMLAK)
  {
    int index = 0;
    for(int i=-kernel_size_; i<=kernel_size_; i++)
    {
      if(i == 0)
        kernel_[index] = vcl_pow(cutoff_freq_,2.0);
      else if(i % 2 == 0)
        kernel_[index] = 0.0;
      else
        kernel_[index] = -4*vcl_pow(cutoff_freq_,2.0) / (vcl_pow((vnl_math::pi  * i), 2.0));
      index++;
    }
  }
}
