#ifndef dbil3d_gauss_filter_txx_
#define dbil3d_gauss_filter_txx_

#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_gauss_filter.txx>
#include <vil3d/vil3d_image_view.h>
#include <vcl_iostream.h>
#include "dbil3d_gauss_filter.h"


template<class T,class U>
void dbil3d_gauss_filter_i(const T* src_im,
                          unsigned src_ni, unsigned src_nj, unsigned src_nk,
                          vcl_ptrdiff_t s_i_step, vcl_ptrdiff_t s_j_step, vcl_ptrdiff_t s_k_step,
                          const vil_gauss_filter_5tap_params& params,
                          U* dest_im,
                          vcl_ptrdiff_t d_i_step, vcl_ptrdiff_t d_j_step, vcl_ptrdiff_t d_k_step)
{
  vil_image_view<T> work(src_ni,src_nj);

  for (unsigned k=0;k<src_nk;++k)
  {
    vil_gauss_filter_5tap(src_im, s_i_step, s_j_step,
                          dest_im, d_i_step, d_j_step, 
                          src_ni, src_nj,
                          params, 
                          work.top_left_ptr(), work.jstep());
    dest_im += d_k_step;
    src_im  += s_k_step;
  }
}

//: Smooth to produce dest_im
//  Applies filter in i,j and k directions
template<class T,class U>
void dbil3d_gauss_filter(const vil3d_image_view<T>& src_im,
                        const float & sigma, 
                        vil3d_image_view<U>& dest_im,
                        vil3d_image_view<U>& work_im1,
                        vil3d_image_view<U>& work_im2)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();
  unsigned n_planes = src_im.nplanes();

  vil_gauss_filter_5tap_params params(sigma);

  // Smooth in i, result in work_im1
  if (work_im1.ni()<ni || work_im1.nj()<nj || work_im1.nk()<nk|| work_im1.nplanes()<n_planes)
          work_im1.set_size(ni,nj,nk,n_planes);
  for (unsigned p=0;p<n_planes;++p)
  {

    dbil3d_gauss_filter_i(src_im.origin_ptr()+p*src_im.planestep(),
                         ni,nj,nk,
                         src_im.istep(),src_im.jstep(),src_im.kstep(),
                         params,
                         work_im1.origin_ptr()+p*work_im1.planestep(),
                         work_im1.istep(),work_im1.jstep(),work_im1.kstep());
  }

  // Smooth in j (by implicitly transposing), result in work_im2
  if (work_im2.ni()<ni || work_im2.nj()<nj || work_im2.nk()<nk|| work_im2.nplanes()<n_planes)
          work_im2.set_size(ni,nj,nk,n_planes);
  for (unsigned p=0;p<n_planes;++p)
  {
    dbil3d_gauss_filter_i(work_im1.origin_ptr()+p*work_im1.planestep(),
                         nj,ni,nk,
                         work_im1.jstep(),work_im1.istep(),work_im1.kstep(),
                         params,
                         work_im2.origin_ptr()+p*work_im2.planestep(),
                         work_im2.jstep(),work_im2.istep(),work_im2.kstep());
  }

  // Smooth in k (by implicitly transposing)
  dest_im.set_size(ni,nj,nk,n_planes);
  for (unsigned p=0;p<n_planes;++p)
  {
          dbil3d_gauss_filter_i(work_im2.origin_ptr()+p*work_im2.planestep(),
                               nk,ni,nj,
                               work_im2.kstep(),work_im2.istep(),work_im2.jstep(),
                               params,
                               dest_im.origin_ptr()+p*dest_im.planestep(),
                               dest_im.kstep(),dest_im.istep(),dest_im.jstep());
  }
}

template<class T,class U>
void dbil3d_gauss_filter(const vil3d_image_view<T>& src_im,
                        const float & sigma, 
                        vil3d_image_view<U>& dest_im)
{
  vil3d_image_view<U> work1;
  vil3d_image_view<U> work2;
  dbil3d_gauss_filter(src_im,sigma,dest_im,work1,work2);
}
#undef DBIL3D_GAUSS_FILTER_INSTANTIATE
#define DBIL3D_GAUSS_FILTER_INSTANTIATE(T,U) \
template void dbil3d_gauss_filter_i(const T* src_im,\
                          unsigned src_ni, unsigned src_nj, unsigned src_nk,\
                          vcl_ptrdiff_t s_i_step, vcl_ptrdiff_t s_j_step, vcl_ptrdiff_t s_k_step,\
                          const vil_gauss_filter_5tap_params& params,\
                          U* dest_im,\
                          vcl_ptrdiff_t d_i_step, vcl_ptrdiff_t d_j_step, vcl_ptrdiff_t d_k_step);\
template void dbil3d_gauss_filter(const vil3d_image_view<T>& src_im,\
                        const float & sigma, \
                        vil3d_image_view<U>& dest_im,\
                        vil3d_image_view<U>& work_im1,\
                        vil3d_image_view<U>& work_im2);\
template void dbil3d_gauss_filter(const vil3d_image_view<T>& src_im,\
                        const float & sigma, \
                        vil3d_image_view<U>& dest_im);
#endif // dbil3d_gauss_filter_txx_
