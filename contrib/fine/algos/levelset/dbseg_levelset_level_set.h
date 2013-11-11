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
// File: level_set.h                                                         
//                                                                           
// Created: Tue Oct 25 14:02:35 2005                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_level_set.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Contains functionality for visualizing
/// \ref anchor_LevelSetFunction "level-set functions" and for computing the
/// \ref anchor_NormalDirectionFlow "normal direction flow" of 
/// \ref anchor_LevelSetFunction "level-set functions".
//===========================================================================

#ifndef _LEVEL_SET_H
#define _LEVEL_SET_H

#include <limits>

//#include "dbseg_levelset_Image.h"
#include <vil/vil_image_view.h>

#include "dbseg_levelset_BorderCondition.h"
#include "dbseg_levelset_LevelSetFunction.h"
#include "dbseg_levelset_Mask.h"

namespace lsseg {

//===========================================================================
/// Compute the change that must be made to a LevelSetFunction when it undergoes
/// one timestep of a \ref anchor_NormalDirectionFlow "normal direction flow",
/// under an externally imposed \ref anchor_NormalForceField "force field".
/// The length of the timestep is not specified, but is taken to be the maximum
/// stable one.  As we are in fact solving a Hamilton-Jacobi equation, the 
/// maximum allowable timestep can be computed by \f$ \Delta t\f$ satisfying the
/// CFL condition: \f$ \Delta t \times max\{\frac{|H_1|}{\Delta x} + \frac{|H_2|}{\Delta y} \} < 1\f$.
/// The book \ref anchor_Osher03 "Level set methods and dynamic implicit surfaces" has a good
/// primer on Hamilton-Jacobi equations and their numerical discretisation, and the reader
/// is encouraged to consult it for details.
/// \param[in] phi                   the level-set function that should undergo the normal-direction flow
/// \param[in,out] advect            on input, this represent the \ref anchor_NormalForceField "normal force field",
///                                  which should have the same dimensions as 'phi' (one value per pixel).  Upon
///                                  return of the function, it will contain the "modification function" that should 
///                                  be multiplied by the timestep and added to 'phi'.
/// \param[in] bcond                 specify the boundary condition to use at the edges of the domain (or the active 
///                                  parts of the domain, in case we use a \ref Mask "mask").
/// \param[out] H1                   returns the max partial derivative of the Hamiltonian with respect to \c x.  This is used
///                                  in order to compute the maximum allowable timestep, according to the CFL condition
///                                  presented above.
/// \param[out] H2                   returns the max partial derivative of the Hamiltonian with respect to \c y.  This is used
///                                  in order to compute the maximum allowable timestep, according to the CFL condition
///                                  presented above.
/// \param[in] changes_allowed_mask  If nonzero, a \ref Mask "mask" defining which parts of the LevelSetFunction 'phi' that
///                                  are subject to change for this timestep.  Values corresponding to inactive parts of this
///                                  mask will not get any update value in 'advect' upon return of the function.
///                                  If zero, the all of (the defined part of) 'phi' is subject to change in this timestep.
/// \param[in] defined_region_mask   If nonzero, a \ref Mask "mask" defining which parts of (the domain of) the 
///                                  LevelSetFunction 'phi' are really defined.  Only these parts will be used when computing
///                                  update values. The boundary of the defined parts will be treated using the boundary
///                                  condition specified in 'bcond'.  If zero, then the whole domain of 'phi' is considered
///                                  as defined.
///
/// \note There is a subtle difference between the \c changes_allowed_mask and the \c defined_region_mask that deserves to
///       be pointed out.  The \c defined_region_mask really specifies which pixels of the LevelSetFunction that contain
///       any meaningful value.  I.e. when estimating derivatives, etc., the algorithm must take care never to use any 
///       pixel that lies outside this domain.  Moreover, it is of no use to compute updates for pixels outside this domain,
///       as these pixels do not make sense anyway.
///       On the other hand, the \c changes_allowed_mask expresses which parts of (the defined region of) the 
///       LevelSetFunction that are subject to change during this timestep.  I.e. updates will only be computed for pixels
///       lying inside this region.  However, pixels outside this region can be used for the algorithm's computational needs
///       (estimation of derivatives, etc.).
void normal_direction_flow_2D(const LevelSetFunction& phi, 
                  LevelSetFunction& advect, // image containing advection term will
                                            // be changed to contain the update
                  BorderCondition bcond,
                  double& H1,
                  double& H2,
                  const Mask* const changes_allowed_mask = 0,
                  const Mask* const defined_region_mask = 0);
//===========================================================================

//===========================================================================
/// Compute the change that must be made to a LevelSetFunction when it undergoes
/// one timestep of a \ref anchor_NormalDirectionFlow "normal direction flow",
/// under an externally imposed \ref anchor_NormalForceField "force field".
/// The length of the timestep is not specified, but is taken to be the maximum
/// stable one.  As we are in fact solving a Hamilton-Jacobi equation, the 
/// maximum allowable timestep can be computed by \f$ \Delta t\f$ satisfying the
/// CFL condition: \f$ \Delta t \times max\{\frac{|H_1|}{\Delta x} + \frac{|H_2|}{\Delta y} + \frac{|H_3|}{\Delta z} \} < 1\f$.
/// The book \ref anchor_Osher03 "Level set methods and dynamic implicit surfaces" has a good
/// primer on Hamilton-Jacobi equations and their numerical discretisation, and the reader
/// is encouraged to consult it for details.
/// \param[in] phi                   the level-set function that should undergo the normal-direction flow
/// \param[in,out] advect            on input, this represent the \ref anchor_NormalForceField "normal force field",
///                                  which should have the same dimensions as 'phi' (one value per pixel).  Upon
///                                  return of the function, it will contain the "modification function" that should 
///                                  be multiplied by the timestep and added to 'phi'.
/// \param[in] bcond                 specify the boundary condition to use at the edges of the domain (or the active 
///                                  parts of the domain, in case we use a \ref Mask "mask").
/// \param[out] H1                   returns the max partial derivative of the Hamiltonian with respect to \c x.  This is used
///                                  in order to compute the maximum allowable timestep, according to the CFL condition
///                                  presented above.
/// \param[out] H2                   returns the max partial derivative of the Hamiltonian with respect to \c y.  This is used
///                                  in order to compute the maximum allowable timestep, according to the CFL condition
///                                  presented above.
/// \param[out] H3                   returns the max partial derivative of the Hamiltonian with respect to \c z.  This is used
///                                  in order to compute the maximum allowable timestep, according to the CFL condition
///                                  presented above.
/// \param[in] changes_allowed_mask  If nonzero, a \ref Mask "mask" defining which parts of the LevelSetFunction 'phi' that
///                                  are subject to change for this timestep.  Values corresponding to inactive parts of this
///                                  mask will not get any update value in 'advect' upon return of the function.
///                                  If zero, the all of (the defined part of) 'phi' is subject to change in this timestep.
/// \param[in] defined_region_mask   If nonzero, a \ref Mask "mask" defining which parts of (the domain of) the 
///                                  LevelSetFunction 'phi' are really defined.  Only these parts will be used when computing
///                                  update values. The boundary of the defined parts will be treated using the boundary
///                                  condition specified in 'bcond'.  If zero, then the whole domain of 'phi' is considered
///                                  as defined.
///
/// \note There is a subtle difference between the \c changes_allowed_mask and the \c defined_region_mask that deserves to
///       be pointed out.  The \c defined_region_mask really specifies which pixels of the LevelSetFunction that contain
///       any meaningful value.  I.e. when estimating derivatives, etc., the algorithm must take care never to use any 
///       pixel that lies outside this domain.  Moreover, it is of no use to compute updates for pixels outside this domain,
///       as these pixels do not make sense anyway.
///       On the other hand, the \c changes_allowed_mask expresses which parts of (the defined region of) the 
///       LevelSetFunction that are subject to change during this timestep.  I.e. updates will only be computed for pixels
///       lying inside this region.  However, pixels outside this region can be used for the algorithm's computational needs
///       (estimation of derivatives, etc.).
void normal_direction_flow_3D(const LevelSetFunction& phi, 
                  LevelSetFunction& advect, // image containing advection term will
                                         // be changed to contain the update
                  BorderCondition bcond,
                  double& H1,
                  double& H2,
                  double& H3,
                  const Mask* const changes_allowed_mask = 0,
                  const Mask* const defined_region_mask = 0);
//===========================================================================

//===========================================================================
/// Creates a black-and-white image that visualizes a LevelSetFunction, using one intensity
/// for the inside region, another for the outside region, and a third intensity for the
/// pixels on the boundary between the two regions.  
/// \param[in] phi the level-set function to visualize
/// \param[out] target the produced image
/// \param[in] threshold the threshold between what is considered 'inside' and 'outside' of the
///                      level-set function (usually zero).
/// \param[in] outside_intensity image intensity of the region considered 'outside' of 'phi'
/// \param[in] curve_intensity image intensity for pixels lying on the boundary between the 'inside' and
///                            'outside' regions.
/// \param[in] interior_intensity image intensity of the region considered 'inside' of 'phi'.
/// \param[in] mask if nonzero, this \ref Mask "mask" specifies what is considered the \em defined part of the 
///                 domain 'phi'. If zero, then the whole domain of 'phi' is considered defined.
/// \param[in] mask_intensity image intensity of the undefined region of 'phi', as specified by 'mask'.
int visualize_level_set(const LevelSetFunction& phi, 
            vil_image_view<double>& target, 
            double threshold,
            double outside_intensity = 0,
            double curve_intensity = 255,
            double interior_intensity = 50,
            const Mask* const mask = 0,
            double mask_intensity = 255);
//===========================================================================

//===========================================================================
/// Creates a color image that visualizes a set of \ref LevelSetFunction "LevelSetFuncions",
/// using a different \ref colordefs.h "color" for the inside of each LevelSetFunction, and another
/// color for regions outside of all \ref LevelSetFunction "LevelSetFunctions".
/// \param[in] phis pointer to array of \ref LevelSetFunction "LevelSetFunctions" to visualize.
/// \param[in] num_phi number of elements in the array pointed to by 'phis'. (I.e., number of 
///                    \ref LevelSetFunction "LevelSetFunctions" to visualize).
/// \param[out] target the produced image
/// \param[in] rgb_color pointer to an array of \ref colordefs.h "colors", where each color is defined 
///                      as a 3-array of RGB values.  There should be (num_phi + 1) colors in this array;
///                      the last one being the color for regions that fall outside of all given 
///                      level-set functions.
void visualize_multisets(const LevelSetFunction** const phis,
             int num_phi,
             vil_image_view<double>& target,
             const int** const rgb_color);
//===========================================================================


}; // end namespace lsseg

#endif // _LEVEL_SET_H



