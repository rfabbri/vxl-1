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
// File: Mask.h                                                              
//                                                                           
// Created: Tue Feb 21 12:07:24 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_Mask.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Contains the definition of the \ref Mask "Mask" object, which is 
/// in fact a <c>typedef</c>-ed \ref lsseg::Image "Image".
//                                                                           
//===========================================================================

#ifndef _MASK_H
#define _MASK_H

//#include "dbseg_levelset_Image.h"
#include <vil/vil_image_view.h>


namespace lsseg {

/// \brief The Mask is an Image used for defining active and inactive pixels of
///        another, identically shaped image.
///
/// As such, the pixel values of a Mask as interpreted as boolean, i.e. as either
/// \c true or \c false.  (In the current implementation, these boolean values are
/// stored as \c char; the reason for this being that this was most efficient
/// computationally speaking on the platform where this library was developed).
/// The Mask is typically single-channelled, and used in conjunction with another 
/// \ref Image of the same \c x, \c y and \c z spatial resolution, so that there can 
/// be a one-to-one correspondence with the pixels in each channel of the \ref Image
/// and those of the Mask.  Those image pixels who are associated with pixels in the
/// Mask having \c false as their value, are considered inactive ("masked out").
typedef vil_image_view<char> Mask;

}; // end namespace lsseg

#endif // _MASK_H



