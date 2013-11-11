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
// File: simple_tools.h                                                      
//                                                                           
// Created: Tue Oct 25 13:56:00 2005                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_simple_tools.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief Contains misc. utilities for working with \ref lsseg::Image "Images".
//                                                                           
//===========================================================================

// recent changes:
// 2006/03/30:    added function read_image_sequence_multichn() (kgre)

#ifndef _SIMPLE_TOOLS_H
#define _SIMPLE_TOOLS_H

//#include "dbseg_levelset_Image.h"
#include <vil/vil_image_view.h>

#include "dbseg_levelset_LevelSetFunction.h"
#include "dbseg_levelset_Mask.h"
#include "dbseg_levelset_ParzenDistributionForce.h"

namespace lsseg {

enum SEG_REGION {SEG_NEGATIVE = 0, SEG_POSITIVE};

//===========================================================================
// image transform functions
//===========================================================================

/// \brief make an Image<double> a negative of itself.
/// 
/// \param[in]     min the min. image pixel value
/// \param[in]     max the max. image pixel value
/// \param[in,out] img the Image<double> which will be turned into its own negative
/// \note This function is hastily implemented, and should be limited to use on single-channel images,
/// or all channels should be within the range specified by \c min and \c max
void negate(vil_image_view<double>& img, double min = 0, double max = 255);

/// \brief rescale the pixel range of an Image<double> so that its pixels spans a different range 
/// than before rescaling
/// 
/// \param[in]       cur_min the current min. of the image's pixel range
/// \param[in]       cur_max the current max. of the image's pixel range
/// \param[in]       to_min  the min. of the image's pixel range \em after rescaling
/// \param[in]       to_max  the max. of the image's pixel range \em after rescaling
/// \param[out]      img     the image whose pixels are to be rescaled
/// \note it could be objected that the \c cur_min and \c cur_max parameters could possibly be directly
/// determined from the image.
/// However, this is not always desirable.  Firstly, it is not sure that the image spans the complete
/// possible range (for instance, many pictures have a formal pixel value range from 0 to 255, but where
/// no pixel in the image attains these extreme values).  Secondly, computing the range directly from the
/// image is costly.
void rescale(vil_image_view<double>& img, double cur_min, double cur_max, double to_min = 0, double to_max = 255);

/// \brief rescale the pixel ranges of each of an image's channels to a user-specified interval
/// 
/// <b> NB </b> Upon function completion, each channel of the image will have a pixel range that spans 
/// the \em full interval specified by the user.
/// \param[in]      to_min the min. value of each channel's pixel range \em after rescaling
/// \param[in]      to_max the max. value of each channel's pixel range \em after rescaling
/// \param[in, out] img    the image whose pixel ranges are to be rescaled
void rescale_channels(vil_image_view<double>& img, double to_min = 0, double to_max = 255);

/// \brief clip the pixel range of a image to a certain interval
///
/// Pixel values below/above the user-specified interval will be set to the interval's min/max
/// value respectively.
/// \param[in,out] img image whose pixels will be clipped to the specified range
/// \param[in] min min. bound of the user-specified range
/// \param[in] max max. bound of the user-specified range
void clip(vil_image_view<double>& img, double min, double max);

/// \brief convert a three-channel Image (RGB of \c double, \c int, etc.) to greyscale.
///
/// \param[in, out] img the image to convert to greyscale.  Should have exactly three channels.
///                     Upon function completion, it will have only one channel, which will be a 
///                     weighed average of those three.
template<typename T>
void to_grayscale(vil_image_view<T>& img);

/// \brief transpose the \c x and \c y dimensions of an Image<double>
/// 
/// The \c z-dimension will remain unchanged.  The transpose operation is of course
/// carried out on \em all image channels.
/// \param[in, out] img the image to be transposed
void transpose(vil_image_view<double>& img);

/// \brief Generate an Image<double> that is the transpose of another Image<double>
/// 
/// The transpose is taking place in the \c x and \c y dimensions, with the 
/// \c z-dimension remaining unchanged.  
/// \param[in] img the original image
/// \param[out] target upon function completion, \c target will contain the transposed version of \c img.
void transpose(const vil_image_view<double>& img, vil_image_view<double>& target);

//===========================================================================
// syntetic image generation functions - generate test images, etc.
//===========================================================================

/// \brief Initializes a LevelSetFunction with horizontal bands created by a sine function.
///
/// This is a simple way of creating a smooth LevelSetFunction whose two regions consist
/// of a number of horizontal bands (the positive vs. the negative regions of the 
/// sine function).  This can be used as an initialization of a LevelSetFunction used
/// for segmentation purposes, since it starts out with a "segmentation" where the two
/// regions are somewhat equally distributed over the image.
/// \param[in, out] img the LevelSetFunction we want to fill with horizontal sinusoidal bands
/// \param[in] num_bands number of distinct bands in the image
/// \param[in] phase use this variable to change the phase of the sine function and
///                  thus specify exactly the vertical position of the bands.  The phase is
///                  specified in multiples of \f$2 \pi \f$, so that <tt>phase = 0.5</tt> gives
///                  a shift of \f$ \frac{\pi}{2} \f$ and <tt> phase = 1 </tt> gives the
///                  same image as <tt> phase = 0 </tt>
void horizontal_sinusoidal_bands(LevelSetFunction& img, int num_bands, double phase=0);


/// \brief Initializes a LevelSetFunction to become 1 everywhere, except for an interior
///        rectangular domain where the LevelSetFunction has the value -1.
///
/// This is a simple way of creating a LevelSetFunction where the closed region is a rectangle
/// positioned by the user somewhere inside the image.  As the resulting function is not smooth,
/// it \em may be recommendable to \ref LevelSetFunction::reinitialize3D() "reinitialize" it 
/// before using it for segmentation purposes.
/// \param[in, out] img the LevelSetFunction we want to initialize
/// \param[in] xmin_ratio position of the "leftmost" edge of the inner rectangular domain, as a fraction of 
///                       total domain width of the LevelSetFunction; i.e. 0 means the leftmost 
///                       edge of the domain.
/// \param[in] xmax_ratio position of the "rightmost" edge of the inner rectangular domain, as a fraction of
///                       total domain width of the LevelSetFunction; i.e. 1 means the rightmost
///                       edge of the domain.
/// \param[in] ymin_ratio As \c xmin_ratio, but concern the <tt>y</tt>-direction.
/// \param[in] ymax_ratio As \c ymax_ratio, but concern the <tt>y</tt>-direction.
/// \param[in] zmin_ratio As \c zmin_ratio, but concern the <tt>z</tt>-direction.
/// \param[in] zmax_ratio As \c zmax_ratio, but concern the <tt>z</tt>-direction.
/// \Note It is required that <tt>xmin_ratio </tt> \< <tt>xmax_ratio </tt>, and similar requirements for
///       <tt>ymin_ratio</tt> and <tt>zmin_ratio</tt>.
void rectangle(LevelSetFunction& img, 
           double xmin_ratio, 
           double xmax_ratio,
           double ymin_ratio,
           double ymax_ratio,
           double zmin_ratio = 0,
           double zmax_ratio = 1);

/// \brief Initializes a LevelSetFunction to become the signed distance function from a point \f$P\f$ in its 
///        domain \em minus some fixed value, thus its zero-set becomes a circle around \f$P\f$.
///
/// \param[in, out] img the LevelSetFunction to initialize
/// \param[in] relrad the relative radius of the sphere (circle in 2D) defined by <tt>img</tt>'s zero-set
///                   after initialization.  1 correspond to the length of the \em longest dimension of
///                   \c img, so usually you will want to give this a value between 0 and 1.
/// \param[in] xrelpos the relative position of the x-coordinate of the center of the point \f$P\f$ as defined
///                    above.  A value of 0 corresponds to the LevelSetFunction's domain's leftmost edge, 
///                    and 1 to its rightmost.
/// \param[in] yrelpos the relative position of the y-coordinate of the center of the point \f$P\f$ as defined
///                    above.  Analoguous to \c xrelpos but concerning the y-dimension.
/// \param[in] zrelpos the relative position of the z-coordinate of the center of the point \f$P\f$ as defined
///                    above.  Analoguous to \c xrelpos but concerning the z-dimension.
void sphere(LevelSetFunction& img,
        double relrad,  //between 0 and 1
        double xrelpos, // between 0 and 1
        double yrelpos, // between 0 and 1
        double zrelpos = 0);
        
// // @@ document this or remove it....   Is it superfluous given the sphere() function above???
// void distance_from_point(LevelSetFunction& img,
//              double zero_level, // which distance is considered zero (as ratio of longest
//                                 // dimension
//              double point_x_relpos, // from 0 to 1
//              double point_y_relpos, // from 0 to 1
//              double point_z_relpos = 0); // from 0 to 

/// \brief Initializes a LevelSetFunction to become a function with two values, 1 and -1, corresponding
///        to the domains in which the force defined by the \c pf argument is positive or negative.
/// 
/// This way of initalizing a LevelSetFunction before a segmentation may be useful in some cases, where
/// the segmentation is strongly dependent on a fixed pixel distribution that can be specified 
/// beforehand (fixed in the ParzenDistributionForce).
/// \param[in, out] img the LevelSetFunction to initialize
/// \param pf the ForceGenerator that will directly decide which region each of the LevelSetFuction's 
///           pixels belong to.  \note The domain of the underlying image refered to by the 
///           ParzenDistributionForce is expected to be exactly the same as that of \c img.
/// \param m An optional \ref Mask defining which part of \c img that will be filled in, and which
///          part will remain unchanged.  If \c m is a zero-pointer, then no \ref Mask will be used and the
///          complete domain of \c img will be initialized.
void set_from_parzen(LevelSetFunction& img, 
              const ParzenDistributionForce pf,
              const Mask* m = 0);

/// \brief Initializes a set of \ref LevelSetFunction "LevelSetFunctions" defined on a common domain 
/// so that their closed regions together constitute a voronoi subdivision of the domain.  The points
/// defining the voronoi subdivision is given by the user.
///
/// Defined this way, the union of the closed regions will exactly cover the total domain, and the 
/// intersection of the closed regions will be empty.  This can be a useful initialization for multiregion-
/// segmentations where you have some idea about the position of each object.
/// \param[in, out] regs pointer to an array of \ref LevelSetFunction "LevelSetFunctions" that will be 
///                      initialized so that each of them represent one of the voronoi regions in the subdivision
///                      of the domain.  <b>NB:</b>All the \ref LevelSetFunction "LevelSetFunctions" in this 
///                      array must have exactly the same shape, and there must be as many of them as the 
///                      desired number of regions.
/// \param[in] center_coords pointer to an array of 2D or 3D coordinates, which represent the points used as 
///                          the basis for the voronoi subdivision of the domain.  The number of points must 
///                          be equal to the number of regions.  The coordinates are stored in
///                          \f$\{x_1y_1x_2y_2...\}\f$ format (2D) or \f$\{x_1y_1z_1x_2y_2z_2...\}\f$ 
///                          format (3D).
/// \param[in] num_regions the number of voronoi regions into which the domain should be subdivided.
/// \param[in] three_d specify whether the domain is 2D or 3D.
void init_voronoi_regions(LevelSetFunction* regs,
              const double* center_coords, // each between 0 and 1
              int num_regions,
              bool three_d = false); // true if 3D

/// \brief Initializes a set of \ref LevelSetFunction "LevelSetFunctions" defined on a common domain
/// so that their closed regions together constitute a voronoi subdivision of the domain.  The points
/// defining the voronoi subdivision are randomly generated.
///
/// Defined this way, the union of the closed regions will exactly cover the whole domain, and the 
/// intersection of the closed regions will be empty.  The number of disjoint fragments of each region
/// can be specified by the user (each such fragment constitutes a voronoi region in the total domain 
/// subdivision).
/// \param[in, out] regs pointer to an array of \ref LevelSetFunction "LevelSetFunctions" that will
///                      be initialized so that each of them represent a certain number of disjoint voronoi
///                      regions.  Taken together, the interior regions defined by \c regs will cover the whole
///                      domain. <b>NB:</b> All the \ref LevelSetFunction "LevelSetFunctions" in this
///                      array must have exactly the same shape, and there must be as many of them as the
///                      desired number of regions, as expressed with \c num_regs
/// \param[in] num_regs  The number of regions into which the domain should be subdivided.
/// \param[in] num_fragments The number of disjointed fragments (separate voronoi regions) that each region
///                          could be made up of.  Should be at least 1.
void random_scattered_voronoi(LevelSetFunction* regs,
                  int num_regs,
                  int num_fragments); // per region

/// \brief Initializes a set of \ref LevelSetFunction "LevelSetFunctions" defined on a common domain so that
/// their closed regions together constitute a subdivision on the domain, and where each such region consists
/// of a set of horizontal bands of a certain width along the y-direction.
///
/// The union of the closed regions will exactly cover the whole domain, and the intersections of the closed
/// regions will be empty.
/// \param[in, out] regs pointer to an array of \ref LevelSetFunction "LevelSetFunctions" that will be initialized
///                      so that their closed domains consists of horizontal stripes, and form a disjoint partition 
///                      of the domain. <b>NB:</b> All the \ref LevelSetFunction "LevelSetFunctions" in this 
///                      array must have exactly the same shape (the shape of the domain), and there must be 
///                      as many of them as the desired number of regions, as expressed with \c num_regs.
/// \param[in] num_regs  The number of regions into which the domain should be subdivided.
/// \param[in] pixel_bandwidth The number of pixels in the <tt> y</tt>-direction across each horizontal stripe 
///                            that makes up a region.
void multiregion_bands(LevelSetFunction* regs,
               int num_regs,
               int pixel_bandwidth);

//===========================================================================
// other functions
//===========================================================================

/// \brief Generate a \ref Mask over a domain, where the active region of the \ref Mask is specified
///        as the region of a certain width (in pixels) around the \ref anchor_ZeroSet "zero-set"
///        of a LevelSetFunction defined on the same domain.
///
/// \param[in] phi the LevelSetFunction defined over the domain in question.  Its \ref anchor_ZeroSet "zero-set"
///            will be used in the definition of the active region of the \ref Mask to create
/// \param[out] target the \ref Mask that will be defined.  It will be \ref LevelSetFunction::resize() "resized"
///             so that its domain coincides with that of \c phi, and its active region will be set according to the 
///             description above.
/// \param[in] width Specify the width of the active region.  The width is specified in <em> number of pixels</em>
///            around the \ref anchor_ZeroSet "zero-set" of the LevelSetFunction given by \c phi.
/// \param[in] geom_mask An optional pointer to a mask specifying the exact geometry of the domain to consider.
///                      If this pointer is 0, the domain to consider is the whole domain covered by \c phi.
void make_border_mask(const LevelSetFunction& phi, 
              Mask& target, 
              int width = 2,
              const Mask* geom_mask = 0);

/// \brief Generate a \ref Mask over a domain, where the active region of the \ref Mask is specified
///        as the region defined by the negative (or positive) region of a LevelSetFunction over the
///        same domain.
///
/// \param[in] phi the LevelSetFunction specifying which part of the domain that shall constitute the 
///                \ref Mask "Mask's" active region.
/// \param[out] target the \ref Mask that will be generated
/// \param[in] reg set this to SEG_NEGATIVE if you want the active region of the \ref Mask to be the
///                part of the domain where \c phi is negative.  Set it to SEG_POSITIVE if you want
///                the active region of the \ref Mask to be the part of the domain where \c phi is
///                positive.
void mask_from_segmentation(const LevelSetFunction& phi, Mask& target, SEG_REGION reg);

/// \brief Read a set of 2D-image files and write the result into a 3D Image<double>, where the read
/// images will be stacked along the z-coordinate direction.
///
/// \param[in] image_list a stream containing the list of names of the image files to read.  The names should
///                       be in ASCII, separated by whitespace. <b>NB:</b> All the image must have the same
///                       shape (resolution) and number of channels.  The fileformats are specified by the 
///                       filename suffixes (.png, .gif, .jpeg, etc..)
/// \param[out] result The images will be written to this Image<double>, stacked along the <tt>z</tt>-direction.
/// \param[in] convert_to_grayscale set this to \c true if you want to convert the read images into grayscale
///                                 before writing them to \c result.  If you do this, \c result will only
///                                 contain <i>one</i> channel upon return.
void read_image_sequence(std::istream& image_list, 
             vil_image_view<double>& result, 
             bool convert_to_grayscale = false);


// void read_image_sequence_multichn(std::istream& image_list, 
//              Image<double>& result,
//              bool convert_to_grayscale = false);

/// \brief Resample an image, using nearest-pixel or linear interpolation
/// 
/// The function is a template on the image type, \c ImgType, which should be
/// an Image<> of some kind (LevelSetFunction, Mask, etc..). \note Don't ask
/// me why the function is using a \c template argument here and not a reference to a base class... 
/// At the time of writing of this documentation, I don't remember any more.
/// \param[in] src The image to resample
/// \param[out] target Before calling this function, set \c target to the desired shape
///                    of the resampled image (in order to communicate it to this function).
///                    On function completion, it will contain the resampled image.
/// \param[in] linear set this to \c true for linear pixel interpolation, otherwise,
///            the resampling will make use of the "nearest pixel" in the image before
///            resampling when determining the value of a pixel in the resampled image.
template<typename ImgType>
void resample_into(const ImgType& src, ImgType& target, bool linear = true);

// if not linear, then piecewise constant

/// \brief Create a sequence of progressively smaller, resampled copies of a reference 
/// image.
///
/// The function is a template on the image type, \c ImgType, which should be an 
/// Image<> of some kind (LevelSetFunction, Mask, etc..). \note Don't ask 
/// me why the function is using a \c template argument here and not a reference to a base class...
/// At the time of writing of this documentation, I don't remember any more.
/// \param[in] input the image we will use as reference image for generating the sequence.
/// \param[out] result When calling the function, this should be an empty vector.  Upon function
///                    completion, it will contain a number of progressively smaller resampled
///                    versions of the image \c input.
/// \param[in] min_num_pixels This is a criterion for determining how many progressively smaller
///                           images will be generated.  When the resampled image would have a coordinate 
///                           direction with a resolution less than \c min_num_pixel, the function stops, and
///                           this image will not be generated. <b> NB:</b> The <tt>z</tt>-coordinate 
///                           direction will only be taken into consideration here if \c downscale_z = \c true.
/// \param[in] downscale_z If '\c true', downscaling will also take place in the <tt>z</tt>-direction, otherwise
///                        the image will only be downscaled in the <tt>x</tt>- and <tt>y</tt>-directions.
///                        Do remember that if the image is only 2D (ie. with a <tt>z</tt>-dimension of length 1),
///                        setting this parameter to \c true would result in the functions immediate return (no
///                        resampled images generated at all), due to the stop criterion explained in the explanation
///                        of the parameter \c min_num_pixels
/// \param[in] downscale_factor Specify the downscale factor.  If \c downscale_factor is equal to 2, the resolution
///                             along the <tt>x</tt>- and <tt>y</tt>- (and possibly <tt>z</tt>-) directions will be
///                             halved from one image in the sequence to the next (divided by 2).  
///                             If \c downscale_factor = \c 1.5, the resolution along each direction  will decrease
///                             with 50% from one image in the sequence to the next (divided by 1.5), etc.
/// \param[in] to_grayscale Specify whether to convert the images to grayscale before adding them to the \c result
///                         vector.
/// \param[in] linear set this to \c true for linear pixel interpolation, otherwise,
///            the resampling will make use of the "nearest pixel" in the image before
///            resampling when determining the value of a pixel in the resampled image.
template<typename ImgType>
void downsample_series(const ImgType& input, 
               std::vector<ImgType >& result,
               int min_num_pixels,
               bool downscale_z = false,
               double downscale_factor = 2,
               bool to_grayscale = false,
               bool linear = true);

/// \brief Count the number of nonzero pixels of an \ref Image\<int\>.
/// 
/// \param[in] img the image for which you want to count the number of nonzero pixels.
/// \retval returns the number of nonzero pixels in \c img.
unsigned long int nonzeroes(const vil_image_view<int>& img);

/// \brief Calculate the ratio of the number pixels with a nonzero value to the total number of pixels
///        in an \ref Image\<int\>.
///
/// \param[in] img the image for which we want to compute this ratio.
/// \retval returns the ratio of nonzero pixels to the total number of image pixels in \c img.
double nonzero_ratio(const vil_image_view<int>& img);

/// \brief Count the number of pixels with a nonnegative value in an \ref Image\<int\>.
///
/// \param[in] img the image for which we want to compute the number of nonnegative pixels.
/// \retval returns the number of nonnegative pixels in \c img.
unsigned long int positives(const vil_image_view<double>& img); // positive or zero

/// \brief Calculate the ratio of the number of pixels with a nonnegative value to the total number of pixels
///        in an \ref Image\<double\>. 
///
/// \param[in] img the image for which we want to compute this ratio
/// \retval returns the ratio of nonnegative pixels to the total number of image pixels in \c img.
double positive_ratio(const vil_image_view<double>& img); // positive or zero ratio

/// \brief Count the number of pixels with a negative value in an \ref Image\<int\>.
///
/// \param[in] img the image for which we want to compute the number of negative pixels.
/// \retval returns the number of negative pixels in \c img.
unsigned long int negatives(const vil_image_view<double>& img); // negative

/// \brief Calculate the ratio of the number of pixels with a negative value to the total number of pixels
///        in an \ref Image\<double\>. 
///
/// \param[in] img the image for which we want to compute this ratio
/// \retval returns the ratio of negative pixels to the total number of image pixels in \c img.
double negative_ratio(const vil_image_view<double>& img); // negative ratio

///  \cond
inline double compute_nu_Brox(unsigned int domain_size, double nu_factor) 
{
    // as proposed by Brox
    return 1.0e-3 * nu_factor * pow(domain_size, 0.7);
}
/// \endcond


}; // end namespace lsseg

#include "dbseg_levelset_simple_tools_templates.h"

#endif // _SIMPLE_TOOLS_H



