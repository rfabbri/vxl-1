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
// File: Region.h                                                            
//                                                                           
// Created: Tue Feb 21 11:58:23 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_Region.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Contains the definition of the \ref lsseg::Region "Region" class,
///        which represents a geometrical region of an \ref lsseg::Image "Image".
//                                                                           
//===========================================================================

#ifndef _REGION_H
#define _REGION_H

#include "dbseg_levelset_LevelSetFunction.h"
#include "dbseg_levelset_ForceGenerator.h"

namespace lsseg {

/// \brief This struct represents the information completely describing one particular
///        segmented region of an image.  It can be used alone in a two-region segmentation
///        setting (the other region being its complement), or several \ref Region "Regions"
///        can be used in a \ref section_MultiRegion "multiregion segmentation setting".

struct Region
{
    /// \brief the weight of the smoothness term used when the shape of the region
    /// changes under the PDE-based evolution process. (Euler-Lagrange equations derived
    /// from the minimization of the
    /// \ref anchor_MumfordShahFunctional "Mumford-Shah functional").
    double mu;

    /// \brief The \ref anchor_LevelSetFunction "level-set function" describing the \em shape
    /// of the region.  (Specified as all image pixels for which the corresponding value in 
    /// \c phi is negative).
    LevelSetFunction phi;
    
    /// \brief pointer to the ForceGenerator used when defining the force driving the 
    /// region's evolution when applying the PDE-process.
    ForceGenerator* fgen;
};

}; // end namespace lsseg

#endif // _REGION_H



