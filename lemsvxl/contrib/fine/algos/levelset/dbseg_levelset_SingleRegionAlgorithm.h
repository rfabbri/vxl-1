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
// File: SingleRegionAlgorithm.h                                             
//                                                                           
// Created: Tue Feb 21 12:53:42 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_SingleRegionAlgorithm.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Contains the \ref lsseg::develop_single_region_2D "develop_single_region_2D
/// and \ref lsseg::develop_single_region_3D "develop_single_region_3D" functions, which 
/// are central in the segmentation process.
//                                                                           
//===========================================================================

#ifndef _SINGLEREGIONALGORITHM_H
#define _SINGLEREGIONALGORITHM_H

#include "dbseg_levelset_Region.h"
#include "dbseg_levelset_Mask.h"

namespace lsseg {

//===========================================================================
/// This function is the bread-and-butter of the level-set based image segmentation
/// progess.  It runs a segmentation on an \ref lsseg::Image "Image" according to 
/// rules specified by a specific \ref lsseg::ForceGenerator "ForceGenerator", and with
/// a specified boundary smoothness criterion (scalar value).  The segmentation is
/// carried out by the development of a time-dependent partial differential equation.
/// A certain, user-specified, number of iterations will be carried out - there is no
/// stop-criterion or other convergence checks (such checks would be quite computationally
/// expensive).  The \ref lsseg::Image "Image" with its initial segmentation, as well 
/// as the \ref lsseg::ForceGenerator "ForceGenerator" and the smoothness criterion in
/// question are all given to the function through a Region object, and the resulting
/// segmentation will be returned as part of the same Region object.  For an overview
/// of the segmentation procedure, read our
/// \ref page_HowSegmentationWorks "text on how segmentation works".
/// This function is optimized for <em>two-dimensional</em> \ref Image "Images".
/// \param[in,out] reg Through this argument, the image, the initial segmentation,
///                    the driving force behind the segmentation and the smoothness criterion
///                    are specified.  After calling this function, the LevelSetFunction
///                    defining the segmentation will have changed to the resulting one.
/// \param[in] num_iter number of iterations (timesteps) of applying the PDE-based process
///                     for developing the segmentation.  Please note that the length of each
///                     timestep can vary and is not given here.  The timestep is chosen by
///                     the algorithm to be the largest one that is guaranteed to be stable.
/// \param[in] reinit_modulo Ever so often, the LevelSetFunction defining the image segmentation
///                          should be \ref LevelSetFunction::reinitialize2D "reinitialized".  
///                          This stabilizes the segmentation process.  Reinitialization is quite 
///                          expensive, however, and it is not in general necessary to run it after
///                          each iteration of applying the PDE.  This parameter lets the user
///                          decide how many iterations of applying the PDE should be carried out
///                          between each reinitialization.  A value of 0 to this argument means
///                          that no reinitalization should ever be carried out.
/// \param[in] geom_mask if nonzero, the \ref Mask pointed to by this argument specifies which parts of 
///                      the original Image that should participate in the segmentation process, the
///                      rest of the image will not be taken into account.  If this pointer is zero,
///                      then the whole image participates in the segmentation process.
/// \note "Where is the image to be segmented?" one might ask when looking at the arguments of this
/// function.  It does not appear to be explicitly given anywhere.  The 'reg' argument only contains
/// a LevelSetFunction describing the shape of the segmented region, a ForceGenerator and a smoothness
/// value.  The answer is that the image is implicitly specified through the ForceGenerator pointed to
/// by 'reg'.  Most \ref ForceGenerator "ForceGenerators" define their
/// \ref anchor_NormalForceField "force field" based on an underlying Image specified when the
/// ForceGenerator was set up.  In fact, the Image is not needed explicitly anywhere in this function; 
/// its only effect is seen through the \ref anchor_NormalForceField "force field" that the
/// ForceGenerator generates.
double develop_single_region_2D(Region& reg, 
                int num_iter, 
                int reinit_modulo = 0, // 0 means no reinitialization
                const Mask* geom_mask = 0);
//===========================================================================

//===========================================================================
/// This function is the bread-and-butter of the level-set based image segmentation
/// progess.  It runs a segmentation on an \ref lsseg::Image "Image" according to 
/// rules specified by a specific \ref lsseg::ForceGenerator "ForceGenerator", and with
/// a specified boundary smoothness criterion (scalar value).  The segmentation is
/// carried out by the development of a time-dependent partial differential equation.
/// A certain, user-specified, number of iterations will be carried out - there is no
/// stop-criterion or other convergence checks (such checks would be quite computationally
/// expensive).  The \ref lsseg::Image "Image" with its initial segmentation, as well 
/// as the \ref lsseg::ForceGenerator "ForceGenerator" and the smoothness criterion in
/// question are all given to the function through a Region object, and the resulting
/// segmentation will be returned as part of the same Region object.  For an overview
/// of the segmentation procedure, read our
/// \ref page_HowSegmentationWorks "text on how segmentation works".
/// This function is optimized for <em>three-dimensional</em> \ref Image "Images".
/// \param[in,out] reg Through this argument, the image, the initial segmentation,
///                    the driving force behind the segmentation and the smoothness criterion
///                    are specified.  After calling this function, the LevelSetFunction
///                    defining the segmentation will have changed to the resulting one.
/// \param[in] num_iter number of iterations (timesteps) of applying the PDE-based process
///                     for developing the segmentation.  Please note that the length of each
///                     timestep can vary and is not given here.  The timestep is chosen by
///                     the algorithm to be the largest one that is guaranteed to be stable.
/// \param[in] reinit_modulo Ever so often, the LevelSetFunction defining the image segmentation
///                          should be \ref LevelSetFunction::reinitialize2D "reinitialized".  
///                          This stabilizes the segmentation process.  Reinitialization is quite 
///                          expensive, however, and it is not in general necessary to run it after
///                          each iteration of applying the PDE.  This parameter lets the user
///                          decide how many iterations of applying the PDE should be carried out
///                          between each reinitialization.  A value of 0 to this argument means
///                          that no reinitalization should ever be carried out.
/// \param[in] geom_mask if nonzero, the \ref Mask pointed to by this argument specifies which parts of 
///                      the original Image that should participate in the segmentation process, the
///                      rest of the image will not be taken into account.  If this pointer is zero,
///                      then the whole image participates in the segmentation process.
/// \note "Where is the image to be segmented?" one might ask when looking at the arguments of this
/// function.  It does not appear to be explicitly given anywhere.  The 'reg' argument only contains
/// a LevelSetFunction describing the shape of the segmented region, a ForceGenerator and a smoothness
/// value.  The answer is that the image is implicitly specified through the ForceGenerator pointed to
/// by 'reg'.  Most \ref ForceGenerator "ForceGenerators" define their
/// \ref anchor_NormalForceField "force field" based on an underlying Image specified when the
/// ForceGenerator was set up.  In fact, the Image is not needed explicitly anywhere in this function; 
/// its only effect is seen through the \ref anchor_NormalForceField "force field" that the
/// ForceGenerator generates.
double develop_single_region_3D(Region& reg, 
                int num_iter, 
                int reinit_modulo = 0, // 0 means no reinitialization
                const Mask* geom_mask = 0);
//===========================================================================

}; // namespace lsseg

#endif // _SINGLEREGIONALGORITHM_H



