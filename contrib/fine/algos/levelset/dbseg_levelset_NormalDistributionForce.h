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
// File: NormalDistributionForce.h                                           
//                                                                           
// Created: Tue Feb 21 12:02:43 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_NormalDistributionForce.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Contains the definition of the \ref lsseg::ForceGenerator "ForceGenerator"
///        called \ref lsseg::NormalDistributionForce "NormalDistributionForce".
//                                                                           
//===========================================================================

#ifndef _NORMALDISTRIBUTIONFORCE_H
#define _NORMALDISTRIBUTIONFORCE_H

#include "dbseg_levelset_Mask.h"
#include "dbseg_levelset_LevelSetFunction.h"
#include "dbseg_levelset_ForceGenerator.h" // only documentation

namespace lsseg {

//===========================================================================
/// \brief This ForceGenerator creates a \ref anchor_NormalForceField "normal force field"
/// derived from the minimization of the
/// \ref anchor_MumfordShahFunctional "Mumford-Shah functional", in a setting where
/// each region is modeled statistically using a normal distribution.
///
/// This model can be seen as an elaboration of the model presented by Chan and Vese
/// in \ref anchor_Chan99 "[Chan99]", with an additional parameter (standard deviation) 
/// describing the pixel distribution inside each region.  Thus, contrary to the 
/// Chan-Vese approach, it can discern between regions having the same average pixel
/// values, as long as the standard deviation of the pixels are different in the two
/// regions.  How the force defined by this ForceGenerator is derived from the 
/// \ref anchor_MumfordShahFunctional "Mumford-Shah functional" is explained e.g. 
/// on page 113 and 114 in the thesis by Brox: \ref anchor_Brox05 "[Brox05]".
class NormalDistributionForce : public ForceGenerator
//===========================================================================
{
public:
    /// \brief Constructor making a NormalDistributionForce-generator which will have 
    /// to be initialized with the \ref init() function  before being usable.
    ///
    /// \param multireg_mode The NormalDistributionForce-generator is written to also work
    ///                      in a multiregion setting (ie. when segmenting an image into
    ///                      more than two parts using a <I>region competition approach</I>
    ///                      If you construct this object with the intention of using it in
    ///                      a multiregion setting, set \c multireg_mode to \c true.  Otherwise
    ///                      (if you intend to use the classical setting with two regions), leave
    ///                      it at \c false.
    ///                      For more about the multiregion setting, refer to the 
    ///                      \ref section_MultiRegion "appropriate section" of the page on
    ///                      \ref page_HowSegmentationWorks "How Segmentation in LSSEG Works".
    NormalDistributionForce(bool multireg_mode = false) 
    : img_(0), mask_(0), multi_region_mode_(multireg_mode) {}  // must be initialized first

    /// \brief Constructor
    ///
    /// \param img pointer to the image to be segmented
    /// \param mask (pointer to) an optional \ref Mask that specifies which part(s) of the
    ///             image are active.  (No forces will be computed for the inactive
    ///             part(s) of an image).  If this pointer is left at zero, the whole
    ///             of \c img is considered to be active.
    /// \param multireg_mode The NormalDistributionForce-generator is written to work
    ///                      in a multiregion setting (ie. when segmenting an image into
    ///                      more than two parts using a <I>region competition approach</I>).
    ///                      If you construct this object with the intention of using it in
    ///                      a multiregion setting, set \c multireg_mode to \c true.  Otherwise
    ///                      (if you intend to use the classical setting with two regions), leave
    ///                      it at \c false.
    ///                      For more about the multiregion setting, refer to the 
    ///                      \ref section_MultiRegion "appropriate section" of the page on
    ///                      \ref page_HowSegmentationWorks "How Segmentation in LSSEG Works".
    NormalDistributionForce(const vil_image_view<double>* img, 
                Mask* mask = 0, 
                bool multireg_mode = false);

    virtual ~NormalDistributionForce() {}

    virtual void init(const vil_image_view<double>* img, const Mask* mask = 0);
    virtual void update(const LevelSetFunction& phi);
    virtual double force2D(int x, int y) const;
    virtual double force3D(int x, int y, int z) const ;
    virtual double force(size_t ix) const;
    virtual void force(LevelSetFunction&, const Mask* mask = 0) const;
    
private:

    /// \brief pointer to the Image to be segmented
    const vil_image_view<double>* img_;

    /// \brief pointer to a \ref Mask specifying which parts of the image are considered to be 
    /// active
    const Mask* mask_;

    /// \brief flag specifying whether this object is intended to be used in a multi-region
    /// setting (see \ref section_MultiRegion "here" for an explanation), or if it is to be
    /// used in the classical two-region problem.
    const bool multi_region_mode_;  // set to true if intended for use in a multiregion setting

    /// \brief average pixel value \em inside the closed region; one entry per image channel.
    std::vector<double> mu_in_; 

    /// \brief average pixel value \em outside the closed region; one entry per image channel.
    std::vector<double> mu_out_; 

    /// \brief The \em inverse of the <I>mean deviation</I> \em inside the closed region; one
    /// entry per image channel.
    std::vector<double> inv_sigma_in_; 
                                       
    /// \brief The \em inverse of the <I>mean deviation</I> \em outside the closed region;
    /// one entry per image channel.
    std::vector<double> inv_sigma_out_; 
                                        
    /// \brief precalculate the logarithm component (defined by the regions' mean pixel 
    /// deviations) of the force.  This expression is slightly different according to whether 
    /// we work in a two-region or multiple-region setting.
    std::vector<double> precalc_log_;  // precalculated the quantity log(s1/s2) = log(s2_inv/s1_inv)


    inline void report() const;

    /// \brief function called by the update() function.  Computes the pixel averages for each 
    /// channel and each region, and writes the result to the \ref mu_out_ and \ref mu_in_ arrays.
    inline void compute_averages(const LevelSetFunction&);

    /// \brief function called by the update() function.  Computes the pixel mean deviation
    /// for each channel and each region, and writes the result to the \ref inv_sigma_in_ and 
    /// \ref inv_sigma_out_ arrays.  It also precomputes the values in \ref precalc_log_ array.
    inline void compute_deviations(const LevelSetFunction&);
};



}; // end namespace lsseg

#endif // _NORMALDISTRIBUTIONFORCE_H


