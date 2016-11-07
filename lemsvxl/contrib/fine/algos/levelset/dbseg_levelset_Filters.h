//===========================================================================
// The Level-Set Segmentation Library (LSSEG)
//
//
// Copyright (C) 2000-2005 SINTEF ICT, Applied Mathematics, Norway.
//
// This program is free software; you can redistribute it and/or          
// modify it under the terms of the GNU General Public License            
// as published by the Free Software Foundation version 2 of the License. 
//
// This program is distributed in the hope that it will be useful,        
// but WITHOUT ANY WARRANTY; without even the implied warranty of         
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          
// GNU General Public License for more details.                           
//
// You should have received a copy of the GNU General Public License      
// along with this program; if not, write to the Free Software            
// Foundation, Inc.,                                                      
// 59 Temple Place - Suite 330,                                           
// Boston, MA  02111-1307, USA.                                           
//
// Contact information: e-mail: tor.dokken@sintef.no                      
// SINTEF ICT, Department of Applied Mathematics,                         
// P.O. Box 124 Blindern,                                                 
// 0314 Oslo, Norway.                                                     
// 
//
// Other licenses are also available for this software, notably licenses
// for:
// - Building commercial software.                                        
// - Building software whose source code you wish to keep private.        
//
//===========================================================================
//===========================================================================
//                                                                           
// File: Filters.h                                                           
//                                                                           
// Created: Wed Feb 22 13:33:39 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_Filters.h,v 1.1 2009-06-09 22:29:13 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief This file contains definitions of functions useful when working
/// with \ref anchor_ASmooth "anisotropic smoothing" and filtering of images.
//                                                                           
//===========================================================================

#ifndef _FILTERS_H
#define _FILTERS_H

//#include "dbseg_levelset_Image.h"
#include <vil/vil_image_view.h>


namespace lsseg {

//===========================================================================
/// \brief Compute the \ref anchor_StructureTensor "structure tensor" of a 2D-image, and writes
/// the result to a 3-channeled 2D-image of the same resolution. 
///
/// \param[in] img the image we want to compute the \ref anchor_StructureTensor "structure tensor" for.  
///                It should have two dimensions (\c x and \c y) and an arbitrary number of channels.
/// \param[out] G this image will contain the three components of the
///               \ref anchor_StructureTensor "structure tensor", each stored in a separate channel.  
///               The first channel will be the square of the estimated partial derivative in \c x, 
///               the second channel will be the estimated partial derivative in \c x multiplied by the 
///               estimated partial derivative in \c y, and the third channel will be the square 
///               of the estimated partial derivative in \c y.
/// \param[in] square_root if this is set to 'true', then the <i>square root</i> of the tensor
///                        will be calculated in each pixel, rather than the tensor itself.
void compute_structure_tensor_2D(const vil_image_view<double>& img, 
                 vil_image_view<double>& G, 
                 bool square_root = false);
//===========================================================================

//===========================================================================
/// \brief Compute the \ref anchor_StructureTensor "structure tensor" of a 3D-image, and writes
/// the result to a 6-channeled 2D-image of the same resolution. 
///
/// \param[in] img the image we want to compute the \ref anchor_StructureTensor "structure tensor" for.  
///                It should have three dimensions (\c x, \c y and \c z) and an arbitrary number of channels.
/// \param[out] G this image will contain the six components of the \ref anchor_StructureTensor "structure tensor", 
///               each stored in a separate channel.  The first channel will be the square of the estimated 
///               partial derivative in \c x, the second channel will be the estimated partial derivative in
///               \c x multiplied by the estimated partial derivative in \c y.  The third channel will be the square
///               of the estimated partial derivative in \c y. The fourth cahnnel will be the estimated
///               partial derivative in \c x multiplied by the estimated partial derivative in \c z.  The
///               fifth channel will be the estimated partial derivative in y multiplied by the estimated
///               partial derivative in \c z.  The sixth channel will be the square of the estimated 
///               partial derivative in \c z.
/// \param[in] square_root if this is set to 'true', then the <i>square root</i> of the tensor
///                        will be calculated in each pixel, rather than the tensor itself.
void compute_structure_tensor_3D(const vil_image_view<double>& img,
                 vil_image_view<double>& G,
                 bool square_root = false);
//===========================================================================

//===========================================================================
/// Compute the smoothing geometry tensor field T from a (blured) structure tensor field
/// G, i.e.,
/// \f$ T = c_1 uu^T + c_2 v v^T\f$, where \f$u\f$ and \f$v\f$ are the eigenvectors of the 
/// \ref anchor_StructureTensor "structure tensor" in each point.  \f$u\f$ correspond to the 
/// direction with the smallest eigenvalue and \f$v\f$ to the one with the largest.
/// The multiplicative factors
/// \f$c_1\f$ and \f$c_2\f$ are computed from the eigenvalues of the structure tensor,
/// \f$\lambda_u\f$ and \f$\lambda_v\f$ in the following way:
/// \f[c_1 = \frac{1}{(1 + \lambda_u + \lambda_v)^{p_1}}\;,\;\;\;\;c_2 = \frac{1}{(1 + \lambda_u + \lambda_v)^{p_2}} \f]
/// For explanation of use, read section 2.1.5 of \ref anchor_Tschumperle02 "D. Tschumperle's thesis", 
/// as well as 2.1 of \ref anchor_Tschumperle06 "[Tschumperle06]".
/// \param G  the input structure tensor field
/// \param p1 Power of the inverse of the multiplicative factor for the direction with the smallest eigenvalue.
///           Should generally be smaller than p2.
/// \param p2 Power of the inverse of the multiplicative factor for the direction with the largest eigenvalue.
///           Should generally be larger than p1.
/// \param T  The resulting structure tensor field
/// \param take_square_root If 'true', the generated structure tensor field will be sqrt(T) 
///                         rather than T.
void compute_smoothing_geometry_2D(const vil_image_view<double>& G,
                   double p1,
                   double p2,
                   vil_image_view<double>& T,
                   bool take_square_root = false);
//===========================================================================

//===========================================================================
/// Compute the smoothing geometry tensor field T from a (blured) structure tensor field
/// G, i.e.,
/// \f$ T = c_1 uu^T + c_2 v v^T + c_3 ww^T\f$, where \f$u\f$, \f$v\f$ and \f$w\f$ are the three
/// eigenvectors of the \ref anchor_StructureTensor "structure tensor" in each point.  \f$u\f$ correspond here
/// to the eigenvector with the smallest eigenvalue and \f$w\f$ to the one with the largest.
/// The multiplicative factors \f$c_1\f$, \f$c_2\f$ and \f$c_3\f$ are computed from the eigenvalues 
/// of the structure tensor, \f$\lambda_u\f$, \f$\lambda_v\f$ and \f$\lambda_w\f$ in the following way:
/// \f[c_1 = \frac{1}{(1 + \lambda_u + \lambda_v + \lambda_w)^{p_1}}\f]
/// \f[c_2 = \frac{1}{(1 + \lambda_u + \lambda_v + \lambda_w)^{p_2}}\f]
/// \f[c_3 = \frac{1}{(1 + \lambda_u + \lambda_v + \lambda_w)^{p_3}}\f]
/// For explanation of use, read section 2.1.5 of \ref anchor_Tschumperle02 "D. Tschumperle's thesis", 
/// as well as 2.1 of \ref anchor_Tschumperle06 "[Tschumperle06]".
/// \param G  the input structure tensor field
/// \param p1 Power of the inverse of the multiplicative factor for the direction with the smallest eigenvalue
///           for G.  Should generally be smaller than p2.
/// \param p2 Power of the inverse of the multiplicative factor for the direction with the intermediate eigenvalue
///           for G.  Should generally be larger than p1.
/// \param p3 Power of the inverse of the multiplicative factor for the direction with the largest eigenvalue for G.
///           Should generally be larger than p1 and p2.
/// \param T  The resulting structure tensor field (6 components)
/// \param take_square_root If 'true', the generated structure tensor field will be sqrt(T) 
///                         rather than T.
void compute_smoothing_geometry_3D(const vil_image_view<double>& G,
                   double p1,
                   double p2,
                   double p3,
                   vil_image_view<double>& T,
                   bool take_square_root = false);
//===========================================================================

//===========================================================================
/// compute the 'scale factor' of an image.  The scale factor gives some information
/// about the global scale of the structure that each pixel belongs to.
/// The scale factor and how it is computed is explained by \ref anchor_Brox05 "Thomas Brox".
/// Basically, it is determined by how much each pixel changes during an anisotropic
/// smoothing process defined by a time-dependent partial differential equation.
///
/// \param img the image for which we want to compute the scale factor.
/// \param scale_accum an image containing the accumulated scale of each pixel in \c img 
///                    over the total 'simulation time'.
/// \param time_accum an image containing, for each pixel, the accumulated 'simulation time'
///                   during which its accumulated scale value has changed.
/// \param dt simulation time step used when computing the scale factor.
/// \param T total simulation time used.
void compute_scale_factor_2D(const vil_image_view<double>& img,
                 vil_image_view<double>& scale_accum,
                 vil_image_view<double>& time_accum,
                 double dt,
                 double T);
//===========================================================================

//===========================================================================
/// compute the 'scale factor' of an image.  The scale factor gives some information
/// about the global scale of the structure that each pixel belongs to.
/// The scale factor and how it is computed is explained by \ref anchor_Brox05 "Thomas Brox".
/// Basically, it is determined by how much each pixel changes during an anisotropic
/// smoothing process defined by a time-dependent partial differential equation.
///
/// \param[in] img the image for which we want to compute the scale factor.
/// \param[out] scale_accum an image containing the accumulated scale of each pixel in \c img 
///                    over the total 'simulation time'.
/// \param[out] time_accum an image containing, for each pixel, the accumulated 
///                        'simulation time' during which its accumulated scale value has
///                        changed.
/// \param[in] dt simulation time step used when computing the scale factor.
/// \param[in] T total simulation time used.
void compute_scale_factor_3D(const vil_image_view<double>& img,
                 vil_image_view<double>& scale_accum,
                 vil_image_view<double>& time_accum,
                 double dt,
                 double T);
//===========================================================================

//===========================================================================
/// Makes a smoothed version of a given image.  The smoothing is nonlinear, meaning
/// that the edges of the images will be smoothed differently than regular areas.
/// The process is based on the time-dependent partial differntial equation
/// \f$u_t = div(\frac{1}{||\nabla u||^p}\nabla u)\f$, where \f$u\f$ represent the 
/// image pixel values and \f$p\f$ is a fixed scalar value.  
/// An explanation can be found in section 2.1 of \ref anchor_Brox05 "Thomas Brox' thesis",
/// particularly under 2.1.2.
/// \note The smoothing process is carried out by evolving the differential equation
/// above with \em one, user-defined timestep.  An operator-splitted implicit scheme 
/// is used, meaning that the process is stable even for large timesteps, and that the 
/// image x, y (and z) dimensions are treated independently.
/// \param[in] img_old the original image that we want to smooth.
/// \param[out] img_new the resulting image after smoothing
/// \param[in] dt the time step to be used in the smoothing process
/// \param[in] sigma strength of (nonlinear) gaussian pre-smoothing of the image before
///                  applying the nonlinear smoothing process.  (Recommended: a small, nonzero
///                  value).
/// \param[in] p this is the user-given \f$p\f$ of the equation above.  It can be seen as a
///              "nonlinearity" parameter.  A value of 0 yields pure gaussian smoothing.
///              A value of 1 yields the TVD (total-variation-diminishing) smoothing.
///              Values beyond 1 yields backward diffusion, meaning that edges will actually
///              be enhanced <em>(edge-enhancing flow)</em>.
void nonlinear_gauss_filter_2D(const vil_image_view<double>& img_old,
                   vil_image_view<double>& img_new,
                   double dt,
                   double sigma,
                   double p);
//===========================================================================

//===========================================================================
/// Makes a smoothed version of a given image.  The smoothing is nonlinear, meaning
/// that the edges of the images will be smoothed differently than regular areas.
/// The process is based on the time-dependent partial differntial equation
/// \f$u_t = div(\frac{1}{||\nabla u||^p}\nabla u)\f$, where \f$u\f$ represent the 
/// image pixel values and \f$p\f$ is a fixed scalar value.  
/// An explanation can be found in section 2.1 of \ref anchor_Brox05 "Thomas Brox' thesis",
/// particularly under 2.1.2.
/// \note The smoothing process is carried out by evolving the differential equation
/// above with \em one, user-defined timestep.  An operator-splitted implicit scheme 
/// is used, meaning that the process is stable even for large timesteps, and that the 
/// image x, y (and z) dimensions are treated independently.
/// \param[in] img_old the original image that we want to smooth.
/// \param[out] img_new the resulting image after smoothing
/// \param[in] dt the time step to be used in the smoothing process
/// \param[in] sigma strength of (nonlinear) gaussian pre-smoothing of the image before
///                  applying the nonlinear smoothing process.  (Recommended: a small, nonzero
///                  value).
/// \param[in] p this is the user-given \f$p\f$ of the equation above.  It can be seen as a
///              "nonlinearity" parameter.  A value of 0 yields pure gaussian smoothing.
///              A value of 1 yields the TVD (total-variation-diminishing) smoothing.
///              Values beyond 1 yields backward diffusion, meaning that edges will actually
///              be enhanced <em>(edge-enhancing flow)</em>.
void nonlinear_gauss_filter_3D(const vil_image_view<double>& img_old,
                   vil_image_view<double>& img_new,
                   double dt,
                   double sigma,
                   double p);
//===========================================================================

//===========================================================================
/// A wrapper function for \ref nonlinear_gauss_filter_2D and \ref nonlinear_gauss_filter_3D.
/// It checks whether the input image is 2D or 3D, and calls the respective function 
/// accordingly.
/// \note The reason there exists separate implementations for the 2D and 3D case is just 
/// computational efficiency.
/// \param[in] img_old the original image that we want to smooth.
/// \param[out] img_new the resulting image after smoothing
/// \param[in] dt the time step to be used in the smoothing process
/// \param[in] sigma strength of (nonlinear) gaussian pre-smoothing of the image before
///                  applying the nonlinear smoothing process.  (Recommended: a small, nonzero
///                  value).
/// \param[in] p this is the user-given \f$p\f$ of the equation above.  It can be seen as a
///              "nonlinearity" parameter.  A value of 0 yields pure gaussian smoothing.
///              A value of 1 yields the TVD (total-variation-diminishing) smoothing.
///              Values beyond 1 yields backward diffusion, meaning that edges will actually
///              be enhanced <em>(edge-enhancing flow)</em>.
void anisotropic_smoothing(const vil_image_view<double>& img_old, 
               vil_image_view<double>& img_new, 
               double dt, 
               double sigma = 0, // default is no initial blurring
               double p = 1); // default is TV flow
//===========================================================================


}; // end namespace lsseg

#endif // _FILTERS_H



