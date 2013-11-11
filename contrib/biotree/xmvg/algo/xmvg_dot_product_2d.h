#ifndef xmvg_dot_product_2d_h_
#define xmvg_dot_product_2d_h_
//: 
// \file  xmvg_dot_product_2d.h
// \brief  templated function to compute dot product of a filter with a image
// \author    Kongbin Kang
// \date        2005-03-28
// 
#include <xmvg/xmvg_composite_filter_2d.h>
#include <xmvg/xmvg_filter_response.h>
#include <dbil/dbil_bounded_image_view.h>

template <class T1, class T2>
xmvg_filter_response<T1> xmvg_dot_product_2d(xmvg_composite_filter_2d<T1> const &filter, 
    dbil_bounded_image_view<T2> const & img);

#endif
