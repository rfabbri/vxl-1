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
// File: ForceGenerator.h                                                    
//                                                                           
// Created: Tue Feb 21 11:52:30 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_ForceGenerator.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief This header file contains the abstract base class for all
/// \ref lsseg::ForceGenerator "force generators"
//===========================================================================

#ifndef _FORCEGENERATOR_H
#define _FORCEGENERATOR_H

#include "dbseg_levelset_LevelSetFunction.h"
#include "dbseg_levelset_Mask.h"

namespace lsseg {

//===========================================================================
/// \brief This is the abstract base class for the generators that define the
/// normal force driving the evolution of a \ref Region's LevelSetFunction.
///
/// The role of a ForceGenerator is to generate a field of forces acting \em normally
/// on the constant-value curves (including zero-set curves) of a level-set function.
/// The basis for generating this \ref anchor_NormalForceField "normal force field" is the 
/// image to be segmented, as well as the current state of segmentation for the image.  
/// The normal force as defined by the ForceGenerator is expressed as a scalar value
/// at each pixel of the image, expressing the magnitude of the normal force acting
/// at the level-set at that location.  These values are then used to evolve the 
/// curve  given timestep.\n
///
/// Read the \ref page_HowSegmentationWorks "How Segmentation in LSSEG Works" for a
/// more comprehensive explanation of the role ForceGenerators play in
/// in this library's segmentation functionally.
class ForceGenerator
//===========================================================================
{
public:
    /// \brief virtual destructor
    virtual ~ForceGenerator() {}

    /// \brief Initialize the ForceGenerator.
    ///
    /// \param img (pointer to) the image that the ForceGenerator should use to 
    ///            derive the force field (i.e. the Image to be segmented).
    /// \param mask (pointer to) an optional \ref Mask that specifies which part(s) of the
    ///             image are active.  (No forces will be computed for the inactive
    ///             part(s) of an image).  If this pointer is left at zero, the whole
    ///             of \c img is considered to be active.
    virtual void init(const vil_image_view<double>* img, const Mask* mask = 0) = 0;

    /// \brief Update the force field based on a given segmentation of an image.
    /// 
    /// \param phi this LevelSetFunction should be of the same shape as the Image
    ///            that the ForceGenerator derives the force field from, and it 
    ///            specifies a segmentation of the Image.  Taking this segmentation
    ///            of the Image into consideration, the ForceGenerator computes
    ///            the new force field.
    virtual void update(const LevelSetFunction& phi) = 0;

    /// \brief Get the normal force value of a (x, y) location in the current normal force field.
    ///
    /// \note \c for 3D images, \c z is here set to be 0.  The method is not overloaded
    /// due to performance reasons.  For true 3D fields, use the force3D() function instead.
    virtual double force2D(int x, int y) const = 0;

    /// \brief Get the normal force value of a (x, y, z) location in the current normal force field.
    /// 
    /// \note The method is not overloaded due to performance reasons.  If you want to 
    /// get the normal force at a point in a 2D field, consider using the \ref force2D() function
    /// instead, as this might be less expensive.
    virtual double force3D(int x, int y, int z) const = 0;

    /// \brief Get the normal force value of a particular pixel, specified by its internal storage
    /// index.
    ///
    /// The internal storage index of any pixel can be found by applying the \ref Image::indexOf()
    /// member function.
    virtual double force(size_t ix) const = 0;
    
    /// \brief Get the complete normal force field as a LevelSetFunction.
    ///
    /// \retval phi the LevelSetFunction which will, upon completion of the function, contain
    ///             the force field currently contained in the ForceGenerator
    /// \param mask (pointer to) an optional \ref Mask that specifies which part(s) of the
    ///             force field are active.  (The inactive parts will not be filled into the
    ///              LevelSetFunction \f$ \phi \f$).  If this pointer is left at zero, the whole
    ///             of the force field is considered to be active.
    virtual void force(LevelSetFunction& phi, const Mask* mask) const = 0;
};

}; // end namespace lsseg

#endif // _FORCEGENERATOR_H


