#ifndef psm_image_sample_h_
#define psm_image_sample_h_

#include <vil/vil_image_view.h>



template<class T>
void psm_set_image_sample(vil_image_view<T> &pix, T const& val)
{
  pix(0,0) = val;
}

template<class T, unsigned N>
void psm_set_image_sample(vil_image_view<T> &pix, vnl_vector_fixed<T,N> const& val)
{
  for (unsigned i=0; i<N; ++i) {
    pix(0,0,i) = val[i];
  }
}



template<class T>
void psm_get_image_sample(vil_image_view<T> const& pix, T &val)
{
  val = pix(0,0);
}

template<class T, unsigned N>
void psm_get_image_sample(vil_image_view<T> const& pix, vnl_vector_fixed<T,N> &val)
{
  for (unsigned i=0; i<N; ++i) {
    val[i] = pix(0,0,i);
  }
}

#endif

