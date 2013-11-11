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
// File: BorderCondition.h                                                   
//                                                                           
// Created: Fri Jan  6 16:22:48 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_BorderCondition.h,v 1.1 2009-06-09 22:29:13 fine Exp $
//                                                                           
// Description: 
/// \file 
/// \brief Contains the BorderCondition enumeration, used when developing a 
/// \ref lsseg::LevelSetFunction "LevelSetFunction" over time as a part of a 
/// segmentation process.
//                                                                           
//===========================================================================

#ifndef _BORDERCONDITION_H
#define _BORDERCONDITION_H

namespace lsseg {

/// \brief An enumeration of possible border conditions when solving a PDE.
///
/// <ul>
/// <li>DIRICHLET means that the value of the function is fixed to some known
/// value at the boundary, but that its gradient there is unknown</li>
/// <li>NEUMANN means that the gradient  of the function is fixed to some known
/// value at the boundary, while its actual value is unknonw</li>
/// </ul>
enum BorderCondition {DIRICHLET = 0, 
              NEUMANN = 1} ;

}; // end namespace lsseg

#endif // _BORDERCONDITION_H



