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
// File: image_utils.h                                                       
//                                                                           
// Created: Wed Feb 22 14:59:21 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_image_utils.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Misc. functions useful when working with \ref lsseg::Image "images".

//===========================================================================

#ifndef _IMAGE_UTILS_H
#define _IMAGE_UTILS_H

#include "dbseg_levelset_errormacros.h"
namespace lsseg {

//===========================================================================
/// Function that takes each image channel of a given number of images, and combines 
/// them all into a new image.
/// \param[in] channel_array array of pointers to the images whose channels will participate
///                          in the new image.  It is of course required that all the images
///                          refered to are \ref Image::spatial_compatible "spatially compatible".
/// \param[in] num_input_images specify the size of the array of pointers in \c channel_array.
/// \param[out] result the resulting image, created from combining all of the channels in the
///             input images.
template<typename T>
void combine_channel_images(const vil_image_view<T>** channel_array, // pointer to an array of pointers
                int num_input_images,
                vil_image_view<T>& result)
//===========================================================================
{
    assert(num_input_images > 0);
    for (int i = 1; i < num_input_images; ++i) {
    /*eli edit   ALWAYS_ERROR_IF(!((channel_array[i])->spatial_compatible(*channel_array[i-1])),
            "Channel size mismatch."); */
    
    }
    const int X = channel_array[0]->ni();
    const int Y = channel_array[0]->nj();
    const int Z = channel_array[0]->nplanes();
    int total_num_channels = 0;
    for (int i = 0; i < num_input_images; ++i) {
    total_num_channels += channel_array[i]->nplanes();
    }
    result.set_size(X, Y, Z);
    
    // filling result
    T* tp = result.begin();
    for (int i = 0; i < num_input_images; ++i) {
    copy(channel_array[i]->begin(), channel_array[i]->end(), tp);
    tp += channel_array[i]->size();
    }
}

}; // end namespace lsseg;

#endif // _IMAGE_UTILS_H



