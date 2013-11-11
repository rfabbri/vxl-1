// This is lemsvxlsrc/basics/dbil3d/algo/dbil3d_hessian_decompose.h
#ifndef dbil3d_hessian_decompose_h_
#define dbil3d_hessian_decompose_h_
//:
// \file
// \brief  Hessian Decomposition
// \author  jdfunaro
// \verbatim
//  Modifications <none>
// \endverbatim

#include <vil3d/vil3d_image_view.h>

//: Compute Hessian decomposition at each point in a 3d image
template <class T> 
void dbil3d_hessian_decompose( const vil3d_image_view<T>& Im,
                               const float & sigma,
                               vil3d_image_view<float>& e1,
                               vil3d_image_view<float>& e2,
                               vil3d_image_view<float>& e3,
                               vil3d_image_view<float>& l1,
                               vil3d_image_view<float>& l2,
                               vil3d_image_view<float>& l3);

//: Compute Hessian decomposition at each point in a 3d image,
//  given gradients in x, y, and z directions
void dbil3d_hessian_decompose( const vil3d_image_view<float>& gradient_x,
                               const vil3d_image_view<float>& gradient_y,
                               const vil3d_image_view<float>& gradient_z,
                               vil3d_image_view<float>& e1,
                               vil3d_image_view<float>& e2,
                               vil3d_image_view<float>& e3,
                               vil3d_image_view<float>& l1,
                               vil3d_image_view<float>& l2,
                               vil3d_image_view<float>& l3);


//: Compute Hessian decomposition at each point in a 3d image,
void dbil3d_hessian_decompose( const vil3d_image_view<float>& Im_xx,
                               const vil3d_image_view<float>& Im_yy,
                               const vil3d_image_view<float>& Im_zz,
                               const vil3d_image_view<float>& Im_xy,
                               const vil3d_image_view<float>& Im_xz,
                               const vil3d_image_view<float>& Im_yz,
                               vil3d_image_view<float>& e1,
                               vil3d_image_view<float>& e2,
                               vil3d_image_view<float>& e3,
                               vil3d_image_view<float>& l1,
                               vil3d_image_view<float>& l2,
                               vil3d_image_view<float>& l3);

#endif 
