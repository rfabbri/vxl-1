// This is lemsvxlsrc/basics/dbil3d/algo/dbil3d_gauss_filter.h
#ifndef dbil3d_gauss_filter_h_
#define dbil3d_gauss_filter_h_
//:
//\file
//\brief 3d Gaussian smoothing
//\author  Jesse Funaro
//\date August 2005
//


#include <vil/algo/vil_gauss_filter.h>
#include <vil3d/vil3d_image_view.h>

template<class T, class U>
void dbil3d_gauss_filter_i(const T* src_im,
                          unsigned src_ni, unsigned src_nj, unsigned src_nk,
                          vcl_ptrdiff_t s_i_step, vcl_ptrdiff_t s_j_step,
                          vcl_ptrdiff_t s_k_step,
                          const vil_gauss_filter_5tap_params& params,
                          U* dest_im,
                          vcl_ptrdiff_t d_i_step, vcl_ptrdiff_t d_j_step, vcl_ptrdiff_t d_k_step);
template<class T,class U>
void dbil3d_gauss_filter(const vil3d_image_view<T>& src_im,
                        const float & sigma, 
                        vil3d_image_view<U>& dest_im,
                        vil3d_image_view<U>& work_im1,
                        vil3d_image_view<U>& work_im2);

template<class T,class U>
void dbil3d_gauss_filter(const vil3d_image_view<T>& src_im,
                        const float & sigma, 
                        vil3d_image_view<U>& dest_im);
#define DBIL3D_GAUSS_FILTER_INSTANTIATE(T,U) extern "please include dbil3d_gauss_filter.txx instead"

#endif // dbil3d_gauss_filter_h_
