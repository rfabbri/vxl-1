#ifndef dbil3d_detect_ridges_txx_
#define dbil3d_detect_ridges_txx_

#include "dbil3d_detect_ridges.h"
#include "dbil3d_hessian_decompose.h"
#include "dbil3d_gauss_filter.h"
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_matrix.h>
#include <vil3d/vil3d_switch_axes.h>
#include <vil3d/vil3d_trilin_interp.h>
#include <vil3d/vil3d_convert.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_grad_1x3.h>
#include <vil3d/io/vil3d_io_image_view.h>

template <class T> 
void dbil3d_detect_ridges( const vil3d_image_view<T>& Im,
                           float sigma,
                           float epsilon,
                           vil3d_image_view<int>& rho,
                           vil3d_image_view<float>& lambda1,
                           vil3d_image_view<float>& lambda2,
                           vil3d_image_view<float>& eigenv3)
{
  vil3d_image_view<float> I;
  vil3d_convert_cast(Im,I);

  vil3d_image_view<float> smoothed;
  dbil3d_gauss_filter(I,sigma,smoothed);
  vil3d_image_view<float> Im_x;
  vil3d_image_view<float> Im_y;
  vil3d_image_view<float> Im_z;
  vil3d_grad_1x3(smoothed,Im_x,Im_y,Im_z);

  dbil3d_detect_ridges(Im_x,Im_y,Im_z,epsilon,rho,lambda1,lambda2,eigenv3);
}

#undef  DBIL3D_DETECT_RIDGES_INSTANTIATE
#define DBIL3D_DETECT_RIDGES_INSTANTIATE(T) \
template void dbil3d_detect_ridges( const vil3d_image_view<T>& Im,\
                           float sigma,\
                           float epsilon,\
                           vil3d_image_view<int>& rho,\
                           vil3d_image_view<float>& lambda1,\
                           vil3d_image_view<float>& lambda2,\
                           vil3d_image_view<float>& eigenv3);
#endif 
