#ifndef dbil3d_hessian_decompose_txx_
#define dbil3d_hessian_decompose_txx_
#include "dbil3d_hessian_decompose.h"
#include "dbil3d_gauss_filter.h"
#include <vil3d/vil3d_convert.h>
#include <vil3d/algo/vil3d_grad_1x3.h>

template <class T> 
void dbil3d_hessian_decompose( const vil3d_image_view<T>& Im,
                           const float& sigma,
                           vil3d_image_view<float>& e1,
                           vil3d_image_view<float>& e2,
                           vil3d_image_view<float>& e3,
                           vil3d_image_view<float>& l1,
                           vil3d_image_view<float>& l2,
                           vil3d_image_view<float>& l3)
{
vil3d_image_view<float> I;
vil3d_convert_cast(Im,I);

vil3d_image_view<float> smoothed;
dbil3d_gauss_filter(I,sigma,smoothed);

vil3d_image_view<float> Im_x;
vil3d_image_view<float> Im_y;
vil3d_image_view<float> Im_z;
vil3d_grad_1x3(smoothed,Im_x,Im_y,Im_z);

dbil3d_hessian_decompose(Im_x,Im_y,Im_z,e1,e2,e3,l1,l2,l3);
}



#undef  DBIL3D_HESSIAN_DECOMPOSE_INSTANTIATE
#define DBIL3D_HESSIAN_DECOMPOSE_INSTANTIATE(T) \
template void dbil3d_hessian_decompose( const vil3d_image_view<T>& Im,\
                           const float& sigma,\
                           vil3d_image_view<float>& e1,\
                           vil3d_image_view<float>& e2,\
                           vil3d_image_view<float>& e3,\
                           vil3d_image_view<float>& l1,\
                           vil3d_image_view<float>& l2,\
                           vil3d_image_view<float>& l3);
#endif 
