#ifndef xmvg_ramp_compensation_2d_h_
#define xmvg_ramp_compensation_2d_h_
//: 
// \file  xmvg_ramp_compensation_2d.h
// \brief  templated function to compute dot product of a filter with a image
// \author    Kongbin Kang
// \date        2005-03-28
// 
#include <xmvg/xmvg_atomic_filter_2d.h>
#include <xmvg/xmvg_composite_filter_2d.h>
#include <xmvg/xmvg_filter_response.h>
#include <dbil/dbil_bounded_image_view.h>

template <class T>
void xmvg_ramp_compensation(xmvg_atomic_filter_2d<T> const &filter,
                            xmvg_atomic_filter_2d<T>& comp_filt); 

template <class T>
void xmvg_ramp_compensation(xmvg_composite_filter_2d<T> const &filter,
                            xmvg_composite_filter_2d<T>& comp_filt); 

#endif
