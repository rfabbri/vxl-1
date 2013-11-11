// This is lemsvxlsrc/basics/dbil3d/algo/dbil3d_finite_second_differences.h
#ifndef dbil3d_finite_second_differences_h_
#define dbil3d_finite_second_differences_h_
//:
// \file
// \brief Given forward/backward finite differences, calculates second derivatives
// \author  jdfunaro
// \verbatim
//  Modifications <none>
// \endverbatim


#include <vil3d/vil3d_image_view.h>
template <class T>
void dbil3d_finite_second_differences(const vil3d_image_view<T>& src_im,
                    const vil3d_image_view<T>& dxp,
                    const vil3d_image_view<T>& dxm,
                    const vil3d_image_view<T>& dyp,
                    const vil3d_image_view<T>& dym,
                    const vil3d_image_view<T>& dzp,
                    const vil3d_image_view<T>& dzm,
                    vil3d_image_view<T>& dxx,
                    vil3d_image_view<T>& dyy,
                    vil3d_image_view<T>& dzz,
                    vil3d_image_view<T>& dxy,
                    vil3d_image_view<T>& dxz,
                    vil3d_image_view<T>& dyz);
#endif 
