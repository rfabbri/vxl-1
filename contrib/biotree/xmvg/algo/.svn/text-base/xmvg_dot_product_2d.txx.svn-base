#ifndef xmvg_dot_product_2d_txx_
#define xmvg_dot_product_2d_txx_

#include "xmvg_dot_product_2d.h"
#include <vnl/vnl_vector.h>
#include <xmvg/xmvg_atomic_filter_2d.h>

#include <vcl_cstdio.h>


template <class T1, class T2>
xmvg_filter_response<T1> xmvg_dot_product_2d(xmvg_composite_filter_2d<T1> const& filter, 
    dbil_bounded_image_view<T2> const & img)
{
  unsigned num_filters = filter.size();
  
  vnl_vector<T1> res(num_filters);

  for(unsigned k = 0; k < num_filters; k++){
    T1 sum = 0;

    xmvg_atomic_filter_2d<T1> f = filter.atomic_filter(k);
    vnl_int_2 nxy = f.size();

    unsigned nx = nxy[0];
    unsigned ny = nxy[1];

    vnl_int_2 offset = f.location();

    unsigned ox = offset[0];
    unsigned oy = offset[1];
    
    for(unsigned i = 0; i < nx; i++)
    {
      for(unsigned j = 0; j < ny; j++)
      {
       sum += img.gpix(i+ox, j+oy)*f[i][j];
      }
    }
    
    res[k] = sum;
  }
  return res;
}

// Code for easy instantiation.
#undef XMVG_DOT_PRODUCT_2D_INSTANTIATE
#define XMVG_DOT_PRODUCT_2D_INSTANTIATE(T1, T2) \
template xmvg_filter_response<T1> xmvg_dot_product_2d( \
    xmvg_composite_filter_2d<T1> const & filter, \
    dbil_bounded_image_view<T2> const & img)

#endif
