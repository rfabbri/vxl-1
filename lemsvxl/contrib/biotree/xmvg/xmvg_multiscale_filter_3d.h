#ifndef xmvg_multiscale_filter_3d_h_
#define xmvg_multiscale_filter_3d_h_

//: 
// \file  xmvg_multiscale_filter_3d.h
// \brief  a data structure storing multiscale filter 
// \author    Kongbin Kang
// \date        2006-05-10
// 
#include "xmvg_composite_filter_3d.h"

template<class T, class F> 
class xmvg_multiscale_filter_3d : public xmvg_composite_filter_3d<T, F> 
{

  public:
    xmvg_multiscale_filter_3d(vcl_vector<F > & filters) 
      : xmvg_composite_filter_3d<T, F>(filters)
      {
        num_filters_a_scale_ = filters_.size();
      }

    add_new_set_filter(double s)
    {
     
      for(int i = 0; i < num_filters_a_scale_; i++)
      {
        filters_.push_back(filters_[i]);
        filters_.end()->scale_filter(s);
      }
    }

  private:

    //: number of filters in a fixed scale
    int num_filters_a_scale_;
};

#endif
