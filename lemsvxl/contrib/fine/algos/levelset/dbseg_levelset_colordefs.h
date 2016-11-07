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
// File: colordefs.h                                                         
//                                                                           
// Created: Fri Mar  3 10:34:23 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_colordefs.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Header containing the definition of some frequently used colors,
/// represented as 3-tuples of \c int, in the <em>RGB format</em>.
//                                                                           
//===========================================================================

#ifndef _COLORDEFS_H
#define _COLORDEFS_H

namespace lsseg {
    
    /// \brief 3-tuple of \c int representing the color <b>red</b> in RGB format.
    const int RED[] = {255, 0, 0};
    /// \brief 3-tuple of \c int representing the color <b>blue</b> in RGB format.
    const int BLUE[] = {0, 255,0};
    /// \brief 3-tuple of \c int representing the color <b>green</b> in RGB format.
    const int GREEN[] = {0, 0, 255};
    /// \brief 3-tuple of \c int representing the color <b>yellow</b> in RGB format.
    const int YELLOW[] = {255,0, 255};
    /// \brief 3-tuple of \c int representing the color <b>cyan</b> in RGB format.
    const int CYAN[] = {0, 255, 255};
    /// \brief 3-tuple of \c int representing the color <b>magenta</b> in RGB format.
    const int MAGENTA[] = {255, 255, 0};
    /// \brief 3-tuple of \c int representing the color <b>white</b> in RGB format.
    const int WHITE[] = {255,255, 255};
    /// \brief 3-tuple of \c int representing the color <b>black</b> in RGB format.
    const int BLACK[] = {0, 0, 0};
    /// \brief 3-tuple of \c int representing the color <b>grey</b> in RGB format.
    const int GREY[] = {200, 200, 200};
    /// \brief 3-tuple of \c int representing the color <b>brown</b> in RGB format.
    const int BROWN[] = {200, 100, 40};

}; // end namespace lsseg

#endif // _COLORDEFS_H



