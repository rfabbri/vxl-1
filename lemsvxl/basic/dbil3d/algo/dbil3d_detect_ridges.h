// This is lemsvxlsrc/basics/dbil3d/algo/dbil3d_detect_ridges.h
#ifndef dbil3d_detect_ridges_h_
#define dbil3d_detect_ridges_h_
//:
// \file
// \brief  Ridge Detection
// Computes a 3d image rho such that rho(i,j,k) == 1 at ridge points
// and rho(i,j,k) == -1 at trough points.  Additionally, provides the 2 largest
// magnitude eigenvalues at each point
//
// Typical parameters:  sigma = 1.5, epsilon = 0.7
// \author  jdfunaro, modified from matlab code by Amir Tamrakar, from Kalitzin etal PAMI 2001 
// \verbatim
//  Modifications <none>
//  2006-07-20 H. Can Aras : added the eigenvector corresponding to the smallest eigenvalue as
//                           an additional return parameter
// \endverbatim

#include <vil3d/vil3d_image_view.h>

//: Compute ridge/trough map and 2 largest eigenvalues given a 3d image
template <class T> 
void dbil3d_detect_ridges( const vil3d_image_view<T>& Im,
                           float sigma,
                           float epsilon,
                           vil3d_image_view<int>& rho,
                           vil3d_image_view<float>& lambda1,
                           vil3d_image_view<float>& lambda2,
                           vil3d_image_view<float>& eigenv3);

//: Compute ridge/trough map and 2 largest eigenvalues given x,y, and z gradients of a 3d image
void dbil3d_detect_ridges( const vil3d_image_view<float>& gradient_x,
                           const vil3d_image_view<float>& gradient_y,
                           const vil3d_image_view<float>& gradient_z,
                           const float& epsilon,
                           vil3d_image_view<int>& rho,
                           vil3d_image_view<float>& lambda1,
                           vil3d_image_view<float>& lambda2,
                           vil3d_image_view<float>& eigenv3);

//: Compute ridge/trough map and 2 largest eigenvalues given gradient & second derivatives
//of a 3d image
void dbil3d_detect_ridges( const vil3d_image_view<float>& gradient_x,
                const vil3d_image_view<float>& gradient_y,
                const vil3d_image_view<float>& gradient_z, 
                const vil3d_image_view<float>& Im_xx,
                const vil3d_image_view<float>& Im_yy,
                const vil3d_image_view<float>& Im_zz,
                const vil3d_image_view<float>& Im_xy,
                const vil3d_image_view<float>& Im_xz,
                const vil3d_image_view<float>& Im_yz, 
                const float& epsilon,
                vil3d_image_view<int>& rho,
                vil3d_image_view<float>& lambda1,
                vil3d_image_view<float>& lambda2,
                vil3d_image_view<float>& eigenv3);

#endif 
