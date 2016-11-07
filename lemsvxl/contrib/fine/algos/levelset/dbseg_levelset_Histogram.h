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
// File: Histogram.h                                                         
//                                                                           
// Created: Fri Mar 31 16:41:35 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_Histogram.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief This file contains the definition of the \ref lsseg::Histogram "Histogram" 
/// class, used to represent histograms.
//                                                                           
//===========================================================================

#ifndef _HISTOGRAM_H
#define _HISTOGRAM_H

#include <vector>
#include <iostream>

namespace lsseg {

//===========================================================================
/// \brief Class representing a histogram.  Its main use is in the
///  \ref ParzenDistributionForce class.
///
/// A histogram class, representing the distribution of a continuous range of 
/// values into a discrete set of bins.
class Histogram
//===========================================================================
{
public:
    /// \brief Constructor, making an undefined histogram.
    /// 
    /// The constructed Histogram is useless, but can be initialized using
    /// the \ref init() function.
    Histogram() {} 

    /// \brief Constructor which explicitly defines the range and number of 
    ///        histogram bins.  
    ///
    /// Upon successful construction, the bins will all be empty.
    /// \param min lower bound of the continuous value range that should be
    ///            split up into discrete bins
    /// \param max upped bound of the continuous value range that should be
    ///            split up into discrete bins
    /// \param num_bins number of discrete bins of this histogram.
    Histogram(double min, double max, int num_bins) 
    {
    init(min, max, num_bins); 
    }
    
    /// \brief Initialize an already-existing Histogram with a new range and
    /// number of bins.
    ///
    /// \param min lower bound of the continuous value range that should be
    ///            split up into discrete bins
    /// \param max upped bound of the continuous value range that should be
    ///            split up into discrete bins
    /// \param num_bins number of discrete bins of this histogram.
    void init (double min, double max, int num_bins) 
    {
    range_min_ = min;
    range_max_ = max;
    dist_ = std::vector<double>(num_bins, 0);
    bin_factor_ = double(num_bins) / (max - min);
    }

    /// \brief get the total number of bins in the Histogram
    int numBins() const { return dist_.size(); }

    /// \brief get the lower bound of the Histogram's value range
    double rangeMin() const { return range_min_; }

    /// \brief get the upper bound of the Histogram's value range
    double rangeMax() const { return range_max_; }
    std::vector<double>& binValues() { return dist_; }
    
    /// \brief blur the histogram
    ///
    /// The contents of the histogram's bins will be "smoothed" with a factor 
    /// determined by the argument \c sigma. (\c sigma is a  positive value - 
    /// and higher values will result in more smoothing).
    void blur(double sigma);

    /// \brief Get the index of the appropriate bin for a given value
    ///
    /// \param val the value for which we want the correct bin index.
    ///            It is supposed that \c val is within the continuous value
    ///            range, which is the closed interval defined by 
    ///            \ref rangeMin() and \ref rangeMax().
    int getBin(double val) const ;
    
    /// \brief Get the "histogram value" for a given value in the Histogram's 
    ///        range.
    /// 
    /// The "histogram value" is a value we have defined to be the product of 
    /// the value of the bin in which the argment \c val falls, multiplied
    /// by the width of the bin.
    double valueFor(double val) const ;

    /// \brief write the complete Histogram object to a stream
    void write(std::ostream& os) const;

    /// \brief read the complete Histogram object from a stream
    void read(std::istream& is);

private:

    /// defines the lower bound on the continous value range
    double range_min_;
    /// definies the upper bound on the continuous value range
    double range_max_;
    
    /// expresses the "width" of each bin
    double bin_factor_;

    /// STL vector holding the value of each individual bin's contents
    std::vector<double> dist_;
};

}; // end namespace lsseg

#endif // _HISTOGRAM_H



