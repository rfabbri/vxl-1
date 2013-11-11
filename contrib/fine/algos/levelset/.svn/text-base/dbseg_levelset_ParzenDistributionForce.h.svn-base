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
// File: ParzenDistributionForce.h                                          
//                                                                           
// Created: Wed Feb 22 21:10:10 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_ParzenDistributionForce.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Contains the definition of the \ref lsseg::ForceGenerator "ForceGenerator"
///        called \ref lsseg::ParzenDistributionForce "ParzenDistributionForce".
//                                                                           
//===========================================================================

#ifndef _PARZENDISTRIBUTIONFORCE_H
#define _PARZENDISTRIBUTIONFORCE_H

#include "dbseg_levelset_ForceGenerator.h" // base class
//#include "dbseg_levelset_Image.h"
#include <vil/vil_image_view.h>

#include "dbseg_levelset_Mask.h"
#include "dbseg_levelset_LevelSetFunction.h"
#include "dbseg_levelset_Histogram.h"
#include <vector>
#include <iostream>

namespace lsseg {

//===========================================================================
/// \brief This ForceGenerator creates a \ref anchor_NormalForceField "normal force field"
/// derived from the minimization of the 
/// \ref anchor_MumfordShahFunctional "Mumford-Shah functional", in a setting where
/// the pixel distribution of each region is modeled by a parzen estimate.
///
/// In the model from which this force is derived, the image regions in the 
/// \ref anchor_MumfordShahFunctional "Mumford-Shah functional" are modeled by unknown
/// probability distributions which are \em estimtated from the current segmentation.
/// This is done by computing the pixel distribution for each current region and channel,
/// and smoothing the obtained histograms by convolution with a Gauss kernel.  The distributions
/// thus obtained capture more information about the regions than does a parametrized
/// distribution like the one used by the \ref NormalDistributionForce generator (a normal
/// distribution described by its average and mean variation).  On the other hand, 
/// using a parzen estimate for modeling the regions risk overfitting the data if the 
/// histograms are not sufficiently smoothed.  For that reason, it is best to apply
/// the ParzenDistributionForce to the region development when departing from an initial
/// segmentation that is already not too bad.
/// For more details, please refer to page 113-118 of Thomas Brox' thesis:
/// \ref anchor_Brox05 "[Brox05]".
class ParzenDistributionForce : public ForceGenerator
//===========================================================================
{
public:
    /// \brief Constructor making a ParzenDistributionForce-generator which will have 
    /// to be initialized with the \ref init() function  before being usable.
    /// \param multireg_mode The ParzenDistributionForce-generator is written to also work
    ///                      in a multiregion setting (ie. when segmenting an image into
    ///                      more than two parts using a <I>region competition approach</I>).
    ///                      If you construct this object with the intention of using it in
    ///                      a multiregion setting, set \c multireg_mode to \c true.  Otherwise
    ///                      (if you intend to use the classical setting with two regions), leave
    ///                      it at \c false.
    ///                      For more about the multiregion setting, refer to the 
    ///                      \ref section_MultiRegion "appropriate section" of the page on
    ///                      \ref page_HowSegmentationWorks "How Segmentation in LSSEG Works".
    ParzenDistributionForce(bool multireg_mode = false) : 
    multi_region_mode_(multireg_mode), 
    num_force_bins_(256),
    img_(0), 
    mask_(0)
    {} // must be initialized before use

    /// \brief Constructor
    ///
    /// \param img pointer to the image to be segmented
    /// \param mask (pointer to) an optional \ref Mask that specifies which part(s) of the
    ///             image are active.  (No forces will be computed for the inactive
    ///             part(s) of an image).  If this pointer is left at zero, the whole
    ///             of \c img is considered to be active.
    /// \param multireg_mode The ParzenDistributionForce-generator is written to also work
    ///                      in a multiregion setting (ie. when segmenting an image into
    ///                      more than two parts using a <I>region competition approach</I>).
    ///                      If you construct this object with the intention of using it in
    ///                      a multiregion setting, set \c multireg_mode to \c true.  Otherwise
    ///                      (if you intend to use the classical setting with two regions), leave
    ///                      it at \c false.
    ///                      For more about the multiregion setting, refer to the 
    ///                      \ref section_MultiRegion "appropriate section" of the page on
    ///                      \ref page_HowSegmentationWorks "How Segmentation in LSSEG Works".
    explicit ParzenDistributionForce(const vil_image_view<double>* img, 
                      Mask* mask = 0, 
                      bool multireg_mode = false) ;

    virtual ~ParzenDistributionForce() {} 

    virtual void init(const vil_image_view<double>* img, const Mask* mask = 0);

    virtual void update(const LevelSetFunction& phi);
    virtual double force2D(int x, int y) const;
    virtual double force3D(int x, int y, int z) const ;
    virtual double force(size_t ix) const;
    virtual void force(LevelSetFunction&, const Mask* mask = 0) const;

    /// \brief Write one of the currently estimated distributions to a stream
    ///
    /// \param channel specify the image channel of the distribution
    /// \param os the stream to which the distribution will be written
    /// \param inside if \c true, the \em distribution of the \em closed region
    ///               will be written, if \c false, it is the distribution \em outside
    ///               the closed region that will be written.
    void saveChannelDistribution(int channel, std::ostream& os, bool inside) const;

    /// \brief Read a precomputed distribution for a spesific region/channel from stream,
    /// and 'fix' it, i.e., the distribution will \em not be changed in the future, even
    /// if the \ref update() function is called.
    ///
    /// \param channel specify the image channel of the distribution
    /// \param is the stream from which to read the precomputed distribution
    /// \param inside if \c true, the \em distribution of the \em closed
    ///               region will be read, if \c false, it is distribution for the 
    ///               \em outside of the closed region that will be read.
    void fixChannelDistributionTo(int channel, std::istream& is, bool inside);

    /// \brief Fix the distribution for a particular region/channel to a distribution
    /// where all pixel values have the same probability.  The distribution will then 
    /// \em not be changed in the future, even if the \ref update() function is called.
    /// 
    /// \param channel specify the image channel of the distribution
    /// \param inside if \c true, this call will apply to the \em inside of the
    ///               closed region.  If \c false, it is the \em outside of the closed
    ///               region that will be fixed.
    void fixDistributionToUniform(int channel, bool inside);

    /// \brief Unfix a previously fixed distribution, so that it becomes recomputed whenever
    /// the \ref update() function is called.
    ///
    /// \param channel specify the channel of the distribution to unfix
    /// \param inside specify whether it is the distribution \em inside or \em outside
    ///               the closed region that will be unfixed.
    void unfixChannelDistribution(int channel, bool inside);

    /// \brief get a const-pointer to the underlying Image<T> to segment
    const vil_image_view<double>* const baseImage() const {return img_;}

private:

    /// \brief flag specifying whether this object is intended to be used in a multi-region
    /// setting (see \ref section_MultiRegion "here" for an explanation), or if it is to be
    /// used in the classical two-region problem.
    const bool multi_region_mode_;

    /// \brief number of entries ("bins") in the precalculated force vectors
    /// (\ref precalc_force_).  
    ///
    /// The force acting on a pixel as a function of its intensity for each image channel
    /// depends on the estimated distributions for the different regions.  These distributions
    /// may change each time the ParzenDistributionForce object is \ref update() "update()"d.
    /// However, for a given set of distributions, these forces can be precomputed, saving
    /// time when computing the complete force field for an image.  The various possible
    /// force values are stored in a discrete set of "bins", so that one can quickly look up
    /// the force for a given pixel by looking at its intensity and determining which bin it
    /// belongs to.  \ref num_force_bins_ specifies the resolution of this discrete set of bins.
    const int num_force_bins_; // number of bins for the precalculated force, must not
                               // be confused with the invidivual number of bins for the
                               // histograms.

    /// \brief pointer to the Image to be segmented
    const vil_image_view<double>* img_;

    /// \brief pointer to a \ref Mask specifying which parts of the image are considered to be 
    /// active
    const Mask* mask_;

    /// \brief specify which distributions are 'fixed' (i.e. do not change even though the
    /// region changes and \ref update() is called).    
    std::vector<std::pair<bool, bool> > is_fixed_;

    /// \brief vector with one entry for each image channel of \ref img_.  Each entry
    /// contains the smoothed Histogram (considered as an estimated probability distribution),
    /// for the pixels inside and outside the closed region for the corresponding image channel.
    std::vector<std::pair<Histogram, Histogram> > hist_; // histogram inside / outside

    /// \brief vector with one entry for each image channel of \ref img_.  Each entry
    /// specifies the max. and min. pixel value found in \ref img_ for the corresponding 
    /// channel.
    std::vector<std::pair<double, double> > channel_ranges_;

    /// \brief contains one entry per image channel.  Specifies the inverse of the width of
    /// each bin in the precalculated force vector \ref precalc_force_.  
    ///
    /// The width of each bin of a given channel is defined as the total pixel value range
    /// of that channel (as found in \ref channel_ranges_), divided by the number of 
    /// bins (as found in \ref num_force_bins_).  For computational optimizing reasons, it is
    /// the \em inverses of the widths that are stored in this vector.
    std::vector<double> force_bin_factor_;

    /// \brief contains one vector per image channel.  Each vector is a lookup-table where one
    /// can find the resulting force acting on a pixel based on the intensity of that pixel
    /// for each image channel.  This force is derived from the distributions in \ref hist_,
    /// but are precalculated and stored here for optimization reasons.  The number of entries
    /// ("bins") in each vector has been given by \ref num_force_bins_.  Since the distributions
    /// in \ref hist_ are apt to change each time \ref update() is called, this vector will 
    /// change accordingly.
    std::vector<std::vector<double> > precalc_force_;


    inline void report() const;

    /// \brief determine the correct 'bin' in the \ref precalc_force_ lookup-array for a given
    /// pixel intensity in a given image channel.
    /// 
    /// \param val the pixel intensity
    /// \param channel the image channel
    inline int getForceBin(double val, int channel) const; 

    /// \brief fill out the information in the \ref precalc_force_ table for a particular
    /// channel, based on estimated distributions (presented as smoothed histograms).
    /// 
    /// \param channel the channel for which we want to compute the corresponding table 
    ///                in \ref precalc_force_.
    /// \param ihist \ref Histogram representing the estimated pixel distribution \em inside
    ///              the closed region for this particular image channel.
    /// \param ohist \ref Histogram representing the estimated pixel distribution \em outside
    ///              the closed region for this particular image channel.
    /// \param forcevec vector containing the precomputed forces.  When this method is called
    ///                 internally, the corresponding entry in precalc_force_ is always given
    ///                 here.
    void precompute_force(int channel,
              const Histogram& ihist,
              const Histogram& ohist,
              std::vector<double>& forcevec);
    
    /// \brief get the value for the "average" or the "center" of a particular channel and bin
    /// of the \ref precalc_force_ lookup table.
    /// 
    /// \param channel specify the image channel
    /// \param ix specify the bin by its index
    double getCenterBinValue(int channel,int ix);

    /// \brief compute the estimated region distributions for a given image channel and a 
    /// given segmentation.
    ///
    /// \param channel       specifies the image channel for which the region distributions
    ///                      will be estimated.
    /// \param phi           a level-set function specifying the current segmentation of the
    ///                      image \ref img_ into two regions.
    /// \retval inside_hist  upon return, this variable will contain the estimated pixel 
    ///                      distribution of the \em inside of the closed segmentation region
    /// \retval outside_hist upon return, this variable will contain the estimated pixel 
    ///                      distribution of the \em outside of the closed segmentation region.
    inline void get_histogram(int channel, 
                  const LevelSetFunction& phi,
                  Histogram& inside_hist,
                  Histogram& outside_hist) const;
    
    /// \brief construct and return an empty histogram based on the pixel value range 
    /// for a particular channel in the image.
    Histogram makeDefaultHistogram(int channel) const ;
};

}; // end namespace lsseg

#endif // _PARZENDISTRIBUTIONFORCE_H



