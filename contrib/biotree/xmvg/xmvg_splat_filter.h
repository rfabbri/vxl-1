// This is /contrib/biotree/xmvg/xmvg_splat_filter.h

#ifndef xmvg_splat_filter_h_
#define xmvg_splat_filter_h_

//: 
// \file    xmvg_splat_filter.h
// \brief   class for the convolution filters applied on the splat image rows
// \author  H. Can Aras
// \date    2005-12-02
// 

#include <vcl_vector.h>

enum { RAMLAK, SHEPPLOGAN };

class xmvg_splat_filter
{
  public:
    //: constructor for Ram-Lak
    xmvg_splat_filter(int kernel_size, double cutoff_freq, int kernel_type);

    //: return the kernel size value
    int kernel_size() { return kernel_size_; };

    //: return the full kernel size value
    int full_kernel_size() { return (2*kernel_size_+1); };
    
    //: return the cut-off frequency value
    double cutoff_freq() { return cutoff_freq_; };

    //: return the kernel values
    vcl_vector<double> kernel() { return kernel_; };
    
  protected:
    // kernel size in one direction, full kernel size is 2*kernel_size_+1
    int kernel_size_;
    // cut-off frequency for the filter
    double cutoff_freq_;
    // filter kernel values
    vcl_vector<double> kernel_;
};

#endif
