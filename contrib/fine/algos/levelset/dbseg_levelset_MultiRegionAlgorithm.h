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
// File: MultiRegionAlgorithm.h                                              
//                                                                           
// Created: Fri Feb 24 14:04:23 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_MultiRegionAlgorithm.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Contains the \ref lsseg::develop_multiregion_2D "develop_multiregion_2D
/// and \ref lsseg::develop_multiregion_3D "develop_multiregion_3D" functions, which 
/// are central in the segmentation process when requiring more than two regions.
//                                                                           
//===========================================================================

#ifndef _MULTIREGIONALGORITHM_H
#define _MULTIREGIONALGORITHM_H

#include "dbseg_levelset_Region.h"
#include "dbseg_levelset_Mask.h"

namespace lsseg {

//===========================================================================
/// This function is the bread-and-butter of the level-set based image segmentation
/// progess.  It runs a \ref section_MultiRegion "multi-region segmentation" on an
/// \ref lsseg::Image "Image" according to rules specified by a specific set of
/// \ref lsseg::ForceGenerator "ForceGenerators", and with a specified set of boundary
/// smoothness criteria (scalar values).  The segmentation is carried out by the development
/// of a time-dependent partial differential equation.  A certain, user-specified, number of
/// iterations will be carried out - there is no stop-criterion or other convergence checks 
/// (such checks would be quite computationally expensive).  The \ref lsseg::Image "Image" 
/// with its initial segmentation, as well as the \ref lsseg::ForceGenerator "ForceGenerators" 
/// and the smoothness criteria are given to the function through a set of Region objects, each
/// representing a particular segmented region of the image.  The resulting segmentation will
/// be returned through the same Region objects.  For an overview of the segmentation procedure, 
/// read our \ref page_HowSegmentationWorks "text on how segmentation works", which also explains
/// the \ref section_MultiRegion "general details on multi-region segmentation".
/// This function is optimized for <em>two-dimensional</em> \ref Image "Images".
/// \param[in,out] regs 'regs' points to an array of \ref Region "Regions", each representing one
///                     of the regions that will be developing in the image.  The information
///                     contained in the \ref Region "Region's" structure gives the driving forces
///                     behind the segmentation for that Region, its initial segmentation and smoothness
///                     criterion.
///                     After calling this function, each Region will have its LevelSetFunction changed
///                     to represent its resulting segmentation.
/// \param[in] num_regs Number of \ref Region "Regions" in the array pointed to by 'regs' (ie. number
///                     of "competing regions" in the segmentation process).
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
/// function.  It does not appear to be explicitly given anywhere.  The 'regs' argument only contains
/// a LevelSetFunction describing the shape of the segmented region, a ForceGenerator and a smoothness
/// value.  The answer is that the image is implicitly specified through the
///  \ref ForceGenerator "ForceGenerators" pointed to by each Region in 'regs'.  Most 
/// \ref ForceGenerator "ForceGenerators" define their \ref anchor_NormalForceField "force field" based
/// on an underlying Image specified when the ForceGenerator was set up.  In fact, the Image is not 
/// needed explicitly anywhere in this function; its only effect is seen through the 
/// \ref anchor_NormalForceField "force fields" that each \ref Region "Region"'s ForceGenerator generate.
/// \note Some \ref ForceGenerator "ForceGenerators" have different modes for normal, two-region 
/// segmentation and for multi-region segmentation.  Make sure they are constructed with the multi-region
/// mode if they are to be used in the context of this function.
double develop_multiregion_2D(Region* regs, 
                  int num_regs,
                  int num_iter,
                  int reinit_modulo, // 0 means no reinitialization
                  const Mask* geom_mask);
//===========================================================================

//===========================================================================
/// This function is the bread-and-butter of the level-set based image segmentation
/// progess.  It runs a \ref section_MultiRegion "multi-region segmentation" on an
/// \ref lsseg::Image "Image" according to rules specified by a specific set of
/// \ref lsseg::ForceGenerator "ForceGenerators", and with a specified set of boundary
/// smoothness criteria (scalar values).  The segmentation is carried out by the development
/// of a time-dependent partial differential equation.  A certain, user-specified, number of
/// iterations will be carried out - there is no stop-criterion or other convergence checks 
/// (such checks would be quite computationally expensive).  The \ref lsseg::Image "Image" 
/// with its initial segmentation, as well as the \ref lsseg::ForceGenerator "ForceGenerators" 
/// and the smoothness criteria are given to the function through a set of Region objects, each
/// representing a particular segmented region of the image.  The resulting segmentation will
/// be returned through the same Region objects.  For an overview of the segmentation procedure, 
/// read our \ref page_HowSegmentationWorks "text on how segmentation works", which also explains
/// the \ref section_MultiRegion "general details on multi-region segmentation".
/// This function is optimized for <em>three-dimensional</em> \ref Image "Images".
/// \param[in,out] regs 'regs' points to an array of \ref Region "Regions", each representing one
///                     of the regions that will be developing in the image.  The information
///                     contained in the \ref Region "Region's" structure gives the driving forces
///                     behind the segmentation for that Region, its initial segmentation and smoothness
///                     criterion.
///                     After calling this function, each Region will have its LevelSetFunction changed
///                     to represent its resulting segmentation.
/// \param[in] num_regs Number of \ref Region "Regions" in the array pointed to by 'regs' (ie. number
///                     of "competing regions" in the segmentation process).
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
/// function.  It does not appear to be explicitly given anywhere.  The 'regs' argument only contains
/// a LevelSetFunction describing the shape of the segmented region, a ForceGenerator and a smoothness
/// value.  The answer is that the image is implicitly specified through the
///  \ref ForceGenerator "ForceGenerators" pointed to by each Region in 'regs'.  Most 
/// \ref ForceGenerator "ForceGenerators" define their \ref anchor_NormalForceField "force field" based
/// on an underlying Image specified when the ForceGenerator was set up.  In fact, the Image is not 
/// needed explicitly anywhere in this function; its only effect is seen through the 
/// \ref anchor_NormalForceField "force fields" that each \ref Region "Region"'s ForceGenerator generate.
/// \note Some \ref ForceGenerator "ForceGenerators" have different modes for normal, two-region 
/// segmentation and for multi-region segmentation.  Make sure they are constructed with the multi-region
/// mode if they are to be used in the context of this function.
double develop_multiregion_3D(Region* regs, 
                  int num_regs,
                  int num_iter,
                  int reinit_modulo, // 0 means no reinitialization
                  const Mask* geom_mask);
//===========================================================================

}; // end namespace lsseg 

#endif // _MULTIREGIONALGORITHM_H




