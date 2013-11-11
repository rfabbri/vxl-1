#ifndef xmvg_ramp_compensation_2d_txx_
#define xmvg_ramp_compensation_2d_txx_

#include "xmvg_ramp_compensation.h"
#include <vcl_complex.h>
#include <vcl_cassert.h>
#include <vnl/vnl_int_2.h>
#include <vnl/algo/vnl_fft_1d.h>
#include <vcl_iostream.h>
//The fft algorithm requires that the size of the 1-d array be expressable as
// 2^p * 3^q * 5^r. This function gives the nearest factorable size.
static unsigned nearest_prime_factor_size(const unsigned ncols, bool odd)
{
  unsigned odd_factored_sizes[19] ={3, 5, 9, 15, 25, 27, 45, 75, 81, 125,
                                     135, 225, 375, 405, 625, 675, 1125,
                                     1875, 2025};
  unsigned even_factored_sizes[38] ={4, 6, 8, 10, 12, 18, 20, 24, 30, 36,
                                     40, 50, 54, 60, 72, 90, 100, 108, 120,
                                     150, 180, 200, 250, 270, 300, 360, 450,
                                     500, 540, 600, 750, 900, 1000, 1080,
                                     1350, 1500, 1800, 2250};
  if(odd)
    for(unsigned k = 0; k<19; ++k)
      if(ncols<=odd_factored_sizes[k])
        return odd_factored_sizes[k];
  //otherwise even
  for(unsigned k = 0; k<38; ++k)
    if(ncols<=even_factored_sizes[k])
      return even_factored_sizes[k];
  return 0;
}

template <class T>
void xmvg_ramp_compensation(xmvg_atomic_filter_2d<T> const &filter,
                            xmvg_atomic_filter_2d<T>& comp_filt)
{
  vnl_int_2 size = filter.size();
  unsigned ncols = static_cast<unsigned>(size[0]),
    nrows = static_cast<unsigned>(size[1]);
  assert(ncols>=3);
  //find the next largest size, n,  that can be factored into 3^q * 5^r
  bool odd = ncols%2>0;
  unsigned n = nearest_prime_factor_size(ncols, odd);
  assert(n>=ncols);
  //Create a row of values corresponding to |omega|.
  //The FFT puts reflected negative frequencies at the right end of the array 
  vcl_vector<T> omega(n);
  unsigned rf = ncols/2;
  unsigned rp = n/2, dr = rp-rf;
  if(odd){
    rf = (ncols-1)/2; rp = (n-1)/2; dr = rp-rf;}
  //multiplying dc + positive frequencies
  for(unsigned i = 0; i<=rp; ++i)
    omega[i]=static_cast<T>(i);
  //multiplying negative frequencies
  for(unsigned i = rp+1; i<n; ++i)
    omega[i] = static_cast<T>(n-i);
  // end of |omega| multiplier
  // FFT operations

  // Note that atomic_filter_2d uses the transpose of the vbl_array_2d index
  // Thus the indices have to be backwards from the normal convention,
  // i.e. [i][j] vs [j][i]
  vbl_array_2d<T> out(ncols, nrows, (T)0);
  vnl_fft_1d<T> fft(n);
  double mag = 2.0*3.14159/n;
  T scale = static_cast<T>(mag);
  for(unsigned j = 0; j<nrows; ++j)
    {
      vcl_vector<vcl_complex<T> > row(n, vcl_complex<T>((T)0, (T)0));
      for(unsigned i = 0; i<ncols; ++i)
        row[i+dr]= vcl_complex<T>(filter[i][j], T(0));
      fft.fwd_transform(row);
      for(unsigned i = 0; i<n; ++i)
        row[i]*=omega[i];
      fft.bwd_transform(row);
      for(unsigned i = 0; i<ncols; ++i)
        out[i][j]=vcl_real(row[i+dr])*scale;
    }
  for(unsigned j = 0; j<nrows; ++j)
    for(unsigned i = 0; i<ncols; ++i)
      out[i][j]*=static_cast<T>(mag);
  comp_filt.set_location(filter.location());
  comp_filt.set_size(filter.size());
  comp_filt.set_weights(out);
}

template <class T>
void xmvg_ramp_compensation(xmvg_composite_filter_2d<T> const &filters,
                            xmvg_composite_filter_2d<T>& comp_filts)
{
  unsigned n = static_cast<unsigned>(filters.size());
  vcl_vector<xmvg_atomic_filter_2d<T> > out_filters;
  for(unsigned i=0; i<n; ++i)
    {
      xmvg_atomic_filter_2d<T> filt = filters.atomic_filter(i), compen_filt;
      xmvg_ramp_compensation<T>(filt, compen_filt);
      out_filters.push_back(compen_filt);
    }
  comp_filts.set_filters(out_filters);
}

// Code for easy instantiation.
#undef XMVG_RAMP_COMPENSATION_INSTANTIATE
#define XMVG_RAMP_COMPENSATION_INSTANTIATE(T) \
template void xmvg_ramp_compensation( \
    xmvg_atomic_filter_2d<T> const & filter, xmvg_atomic_filter_2d<T>& comp ); \
template void xmvg_ramp_compensation( \
    xmvg_composite_filter_2d<T> const & filter, xmvg_composite_filter_2d<T>& comp); 

#endif
