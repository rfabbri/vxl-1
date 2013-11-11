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
// File: LevelSetFunction_implementation.h                                   
//                                                                           
// Created: Fri Feb 17 16:36:53 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_LevelSetFunction_implementation.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Implements the inline functions declared in \ref LevelSetFunction.h.
//                                                                           
//===========================================================================

#ifndef _LEVELSETFUNCTION_IMPLEMENTATION_H
#define _LEVELSETFUNCTION_IMPLEMENTATION_H

#include <cmath>

namespace lsseg {

//===========================================================================
double LevelSetFunction::gradientNorm2D(int x, int y) const
//===========================================================================
{
    const int xp = --x >= 0 ? x : 0; ++x;
    const int xn = ++x < ni() ? x : ni() - 1; --x;
    const int yp = --y >= 0 ? y : 0; ++y;
    const int yn = ++y < nj() ? y : nj() - 1; --y;
    const double Icp = operator()(x, yp);
    const double Icn = operator()(x, yn);
    const double Ipc = operator()(xp, y);
    const double Inc = operator()(xn, y);

    const double x_diff = (xn - xp) == 2 ? 0.5 : 1; // inverse of distance
    const double y_diff = (yn - yp) == 2 ? 0.5 : 1; 

    const double dx = (Inc - Ipc) * x_diff;
    const double dy = (Icn - Icp) * y_diff;
    return sqrt((dx*dx) + (dy*dy));
}

//===========================================================================
double LevelSetFunction::gradientNorm3D(int x, int y, int z) const
//===========================================================================
{
    const int xp = --x >= 0 ? x : 0; ++x;
    const int xn = ++x < ni() ? x : ni() - 1; --x;
    const int yp = --y >= 0 ? y : 0; ++y;
    const int yn = ++y < nj() ? y : nj() - 1; --y;
    const int zp = --z >= 0 ? z : 0; ++z;
    const int zn = ++z < nplanes() ? z : nplanes() - 1; --z;
    const double Icpc = operator()(x, yp,  z);
    const double Icnc = operator()(x, yn,  z);
    const double Ipcc = operator()(xp, y,  z);
    const double Incc = operator()(xn, y,  z);
    const double Iccp = operator()(x,  y, zp);
    const double Iccn = operator()(x,  y, zn);
    
    const double x_diff = (xn - xp) == 2 ? 0.5 : 1; // inverse of distance
    const double y_diff = (yn - yp) == 2 ? 0.5 : 1; 
    const double z_diff = (zn - zp) == 2 ? 0.5 : 1;

    const double dx = (Incc - Ipcc) * x_diff;
    const double dy = (Icnc - Icpc) * y_diff;
    const double dz = (Iccn - Iccp) * z_diff;

    return sqrt((dx*dx) + (dy*dy) + (dz*dz));
}

//===========================================================================
double LevelSetFunction::curvature2D(int x, int y) const
//===========================================================================
{
    const double res = curvatureTimesGrad2D(x, y);
    return res / sqrt(cached_);
}

//===========================================================================
double LevelSetFunction::curvature3D(int x, int y, int z) const
//===========================================================================
{
    const double res = curvatureTimesGrad3D(x, y, z); // will also set 'cached_' to square of grad
    return res / sqrt(cached_); 
}


//===========================================================================
double LevelSetFunction::curvatureTimesGrad2D(int x, int y) const
//===========================================================================
{
    const double EPS=1.0e-5;
    const int xp = --x >= 0 ? x : 0; ++x;
    const int xn = ++x < ni() ? x : ni() - 1; --x;
    const int yp = --y >= 0 ? y : 0; ++y;
    const int yn = ++y < nj() ? y : nj() - 1; --y;
    const double Icp = operator()(x, yp);
    const double Icn = operator()(x, yn);
    const double Ipc = operator()(xp, y);
    const double Inc = operator()(xn, y);
    const double dx = (Inc - Ipc) * 0.5;
    const double dy = (Icn - Icp) * 0.5;
    const double norm2 = (dx*dx) + (dy*dy);
    cached_ = norm2 > EPS ? norm2 : EPS;

    if (norm2 < EPS) return 0; // cannot compute meaningful curvature with zero gradient

    const double Ipp = operator()(xp, yp);
    const double Inp = operator()(xn, yp);
    const double Icc = operator()(x,y);
    const double Ipn = operator()(xp, yn);
    const double Inn = operator()(xn, yn);
    const double dxx = Inc + Ipc - 2 * Icc;
    const double dyy = Icn + Icp - 2 * Icc;
    const double dxy = (dx * dy < 0) ? 
    0.5 * (2 * Icc + Ipp + Inn - Ipc - Inc - Icp - Icn) :
    0.5 * (Ipc + Inc + Icp + Icn - 2 * Icc - Ipn - Inp);
    return (dy * dy * dxx - 2 * dx * dy * dxy + dx * dx * dyy) / norm2;
}

//===========================================================================
double LevelSetFunction::curvatureTimesGrad3D(int x, int y, int z) const
//===========================================================================
{
    const double EPS=1.0e-5;
    const int xp = --x >= 0 ? x : 0; ++x;
    const int xn = ++x < ni() ? x : ni() - 1; --x;
    const int yp = --y >= 0 ? y : 0; ++y;
    const int yn = ++y < nj() ? y : nj() - 1; --y;
    const int zp = --z >= 0 ? z : 0; ++z;
    const int zn = ++z < nplanes() ? z : nplanes() - 1; --z;

    const double Icpc = operator()(x, yp,  z);
    const double Icnc = operator()(x, yn,  z);
    const double Ipcc = operator()(xp, y,  z);
    const double Incc = operator()(xn, y,  z);
    const double Iccp = operator()(x,  y, zp);
    const double Iccn = operator()(x,  y, zn);

    const double x_diff = (xn - xp) == 2 ? 0.5 : 1; // inverse of distance
    const double y_diff = (yn - yp) == 2 ? 0.5 : 1; 
    const double z_diff = (zn - zp) == 2 ? 0.5 : 1;

    const double dx = (Incc - Ipcc) * x_diff;
    const double dy = (Icnc - Icpc) * y_diff;
    const double dz = (Iccn - Iccp) * z_diff;
    const double norm2 = (dx*dx) + (dy*dy) + (dz*dz);
    cached_ = norm2 > EPS ? norm2 : EPS;

    if(norm2 < EPS) return 0; // cannot compute meaningful curvature with zero gradient

    const double Ippc = operator()(xp, yp,  z);
    const double Ipcp = operator()(xp,  y, zp);
    const double Ipcn = operator()(xp,  y, zn);
    const double Ipnc = operator()(xp, yn,  z);
    const double Icpp = operator()( x, yp, zp);
    const double Icpn = operator()( x, yp, zn);
    const double Iccc = operator()( x,  y,  z);
    const double Icnp = operator()( x, yn, zp);
    const double Icnn = operator()( x, yn, zn);
    const double Inpc = operator()(xn, yp,  z);
    const double Incp = operator()(xn,  y, zp);
    const double Incn = operator()(xn,  y, zn);
    const double Innc = operator()(xn, yn,  z);

    const double dxx = Incc + Ipcc - 2 * Iccc;
    const double dyy = Icnc + Icpc - 2 * Iccc;
    const double dzz = Iccn + Iccp - 2 * Iccc;
    const double dxy = (dx * dy < 0) ? 
    0.5 * (2 * Iccc + Ippc + Innc - Ipcc - Incc - Icpc - Icnc) :
    0.5 * (Ipcc + Incc + Icpc + Icnc - 2 * Iccc - Ipnc - Inpc);
    const double dxz =  (dx * dz < 0) ? 
    0.5 * (2 * Iccc + Ipcp + Incn - Ipcc - Incc - Iccp - Iccn) :
    0.5 * (Ipcc + Incc + Iccp + Iccn - 2 * Iccc - Ipcn - Incp);
    const double dyz = (dy * dz < 0) ?
    0.5 * (2 * Iccc + Icpp + Icnn - Icpc - Icnc - Iccp - Iccn) :
    0.5 * (Icpc + Icnc + Iccp + Iccn - 2 * Iccc - Icpn - Icnp);


    return (dx * dx * (dyy + dzz) + 
        dy * dy * (dxx + dzz) +
        dz * dz * (dxx + dyy) - 
        2 * (dx * dy * dxy + 
         dx * dz * dxz +
         dy * dz * dyz)
        ) / norm2;
}

//===========================================================================
void LevelSetFunction::curvature2D(vil_image_view<double>& target, Mask* mask) const
//===========================================================================
{
    /*
    //eli edit assert(!mask || spatial_compatible(*mask));
    MESSAGE_IF(nplanes() > 1, "Warning: applying 2D method on a grid with dim(z) > 1");
    const int X = ni();
    const int Y = nj();
    if (!size_compatible(target)) {
        target.resize(X, Y);
    }
    if (!mask) {
    for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
        target(x, y) = curvature2D(x, y);
        }
    }
    } else {
    char* mptr = mask->begin();
    for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
        if (*mptr++) {
            target(x, y) = curvature2D(x, y);
        }
        }
    }
    }
    */
}

//===========================================================================
void LevelSetFunction::curvature3D(vil_image_view<double>& target, Mask* mask) const
//===========================================================================
{
 /*   assert(!mask || spatial_compatible(*mask));
    const int X = ni();
    const int Y = nj();
    const int Z = nplanes();
    if (!size_compatible(target)) {
    target.resize(X, Y, Z);
    }
    if (!mask) {
    for (int z = 0; z < Z; ++z) {
        for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
            target(x, y, z) = curvature3D(x, y, z);
        }
        }
    }
    } else {
    char* mptr = mask->begin();
    for (int z = 0; z < Z; ++z) {
        for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
            if (*mptr++) {
            target(x, y, z) = curvature3D(x, y, z);
            }
        }
        }
    }
    }
    */
}

//===========================================================================
void LevelSetFunction::gradientNorm2D(vil_image_view<double>& target, Mask* mask) const
//===========================================================================
{
/*
    assert(!mask || spatial_compatible(*mask));
    MESSAGE_IF(nplanes() > 1, "Warning: applying 2D method on a grid with dim(z) > 1");
    const int X = ni();
    const int Y = nj();
    if (!size_compatible(target)) {
    target.resize(X, Y);
    }
    if (!mask) {
    for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
        target(x, y) = gradientNorm2D(x, y);
        }
    }
    } else {
    char* mptr = mask->begin();
    for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
        if (*mptr++) {
            target(x, y) = gradientNorm2D(x, y);
        }
        }
    }

    }
    */
}

//===========================================================================
void LevelSetFunction::gradientNorm3D(vil_image_view<double>& target, Mask* mask) const
//===========================================================================
{
    /*assert(!mask || spatial_compatible(*mask));
    const int X = ni();
    const int Y = nj();
    const int Z = nplanes();
    if (!size_compatible(target)) {
    target.resize(X, Y, Z);
    }
    if (!mask) {
    for (int z = 0; z < Z; ++z) {
        for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
            target(x, y, z) = gradientNorm3D(x, y, z);
        }
        }
    }
    } else {
    char* mptr = mask->begin();
    for (int z = 0; z < Z; ++z) {
        for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
            if (*mptr++) {
            target(x, y, z) = gradientNorm3D(x, y, z);
            }
        }
        }
    }
    }
    */
}


//===========================================================================
void LevelSetFunction::curvatureTimesGrad2D(vil_image_view<double>& target, Mask* mask) const
//===========================================================================
{
   /* assert(!mask || spatial_compatible(*mask));
    MESSAGE_IF(nplanes() > 1, "Warning: applying 2D method on a grid with dim(z) > 1");
    const int X = ni();
    const int Y = nj();
    if (!size_compatible(target)) {
    target.resize(X, Y);
    }

    if (!mask) {
    for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
        target(x, y) = curvatureTimesGrad2D(x, y);
        }
    }
    } else {
    char* mptr = mask->begin();
    for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
        if (*mptr++) {
            target(x, y) = curvatureTimesGrad2D(x, y);
        }
        }
    }
    }
    */
}

//===========================================================================
void LevelSetFunction::curvatureTimesGrad3D(vil_image_view<double>& target, Mask* mask) const
//===========================================================================
{
    /*assert(!mask || spatial_compatible(*mask));
    const int X = ni();
    const int Y = nj();
    const int Z = nplanes(); 
    if (!size_compatible(target)) {
    target.resize(X, Y, Z);
    }

    if (!mask) {
    for (int z = 0; z < Z; ++z) {
        for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
            target(x, y, z) = curvatureTimesGrad3D(x, y, z);
        }
        }
    }
    } else {
    char* mptr = mask->begin();
    for (int z = 0; z < Z; ++z) {
        for (int y = 0; y < Y; ++y) {
        for (int x = 0; x < X; ++x) {
            if (*mptr++) {
            target(x, y, z) = curvatureTimesGrad3D(x, y, z);
            }
        }
        }
    }
    }*/
}


}; // end namespace lsseg

#endif // _LEVELSETFUNCTION_IMPLEMENTATION_H



