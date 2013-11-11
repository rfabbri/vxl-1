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
// File: cimg_dependent.h                                                    
//                                                                           
// Created: Tue Oct 25 13:22:40 2005                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_cimg_dependent.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file 
/// \brief This header encapsulates all functions, classes, etc. whose implementations
/// are dependent on the \c cimg library (downloadable
/// <a href="http://cimg.sourceforge.net/">here</a>). 
///
///
/// The idea of separating
/// out all code dependent on \c cimg in this header (and the file containing its
/// implementation: \ref cimg_dependent.C) was to keep a sharp line between code
/// in this project (\c lsseg) that stands alone, and code that needs the external 
/// library \c cimg to run.
/// This way, if ever it is decided to make \c lsseg a fully independent library, 
/// it is sufficient to reimplement the functions declared in this header (and
/// whose current implementation is found in \ref cimg_dependent.C).
///
//===========================================================================

#ifndef _CIMG_DEPENDENT_H
#define _CIMG_DEPENDENT_H

//#include "dbseg_levelset_Image.h"
#include <vil/vil_image_view.h>

namespace lsseg {

/// \cond OMIT_FROM_DOXYGEN
struct Pimpl; // class implementation
/// \endcond 

/// \brief load an Image<double> from file
///
/// \param name                 - name of the file containing the image.  The file extension, 
///                               being a part of the name, specifies what format the image is 
///                               stored in (png, jpg, etc.)
/// \param convert_to_greyscale - if set to \c true, the loaded image will be converted to 
///                               greyscale, and thus be guaranteed to contain exactly \em one
///                               image channel.  If \c false, the number of image channels
///                               will be those found in the file.
/// \retval target              - Upon function completion, \c target will contain the image 
///                               loaded from file
void load_image(const char* name, vil_image_view<double>& target, bool convert_to_greyscale = false);

/// \brief load an Image<int> from file
///
/// \param name                 - name of the file containing the image.  The file extension, 
///                               being a part of the name, specifies what format the image is 
///                               stored in (png, jpg, etc.)
/// \param convert_to_greyscale - if set to \c true, the loaded image will be converted to 
///                               greyscale, and thus be guaranteed to contain exactly \em one
///                               image channel.  If \c false, the number of image channels
///                               will be those found in the file.
/// \retval target              - Upon function completion, \c target will contain the image 
///                               loaded from file
void load_image(const char* name, vil_image_view<int>& target, bool convert_to_greyscale = false);

/// \brief save an Image<double> to file
/// 
/// \param name the filename where the image should be stored.  The storage format is specified
///             by the filename extension (jpg, png, gif, etc..)
/// \param target the image to be written to file.
void save_image(const char* name, const vil_image_view<double>& target);

/// \brief Display an image in a graphical window and pause the program until the window
/// is closed
///
/// \param img the image to be displayed
/// \param z in case of a 3D image, \c z specifies which z-coordinate slice to display on
///          screen (the image shown will always be in the (x, y)-plane).
void display_image(const vil_image_view<double>& img, int z = 0);


// display and continue program (at the moment, causes memory leak)
/// \brief Display an Image in a graphical window an continue execution of program.
///
/// \param img the image to display
///
/// \note the current implementation might cause a memory leak!
void permanent_display(const vil_image_view<double>& img);

/// \brief Gaussian blur an Image<double> with a certain strength.
///
/// \param img the image to be blurred
/// \param rho the \f$ \sigma \f$ parameter of the Gaussian kernel.  Should be positive.
///            The higher the value of \c rho, the more the picture will be blurred.
void blur_image(vil_image_view<double>& img, double rho);

/// \brief Gaussian blur an 1D-array of \c double values with a certain strength.
///
/// \param data pointer to the memory array to be blurred.
/// \param data_size size of the memory array to be blurred
/// \param rho the \f$ \sigma \f$ parameter of the Gaussian kernel.  Should be positive.
///            The higher the value of \c rho, the more the picture will be blurred.
void blur_1D(double* data, unsigned int data_size, double rho);

/// \brief fill an Image<double> with Gaussian noise.
///
/// \param img the image to be filled with Gaussian noise
/// \param sigma the \f$ \sigma \f$ parameter of the Gaussian kernel.  Should be nonnegative.
void gaussian_noise(vil_image_view<double>& img, double sigma = 0);

/// \brief Display the pixel distribution of the image as a graph (histogram).
///
/// \param img the image for which we want to display the pixel distribution.
/// \note in case of a 3D image, only the histogram corresponding to the (x, y)-plane
/// for which \c z = 0 will be displayed.
//void display_distribution(const Image<double>& img); @@@

//===========================================================================
/// \brief Graphical display of an \ref Image, with basic user interaction
///        functionality.
///
/// This class is useful for debugging and demonstration purposes.  Upon construction
/// it immediately shows the concerned Image on the user display, where it remains
/// as long as the constructed object exists.  The image can be successively updated, 
/// allowing for slideshows or basic animations.  The user can also be allowed basic
/// interaction with the image, like resizing it or inspecting pixel values.
/// Several UpdatableImages can also be \em connected, meaning that resizing one of them
/// will automatically resize all the others, and inspecting pixel values in one will
/// also return the corresponding pixel values in the connected UpdatableImages.
/// \note This class is only intended for debugging/simple demonstrations.  Its implementation
/// is not very rigorous and it is not intended to be used in any serious setting.
class UpdatableImage
//===========================================================================
{
public:
    /// \brief Constructor taking an Image<double> and an optional text string as arguments
    /// 
    /// Upon construction, the image \c img will immediately be displayed on the user's screen
    /// (and stay there).  If \c name points to a text string, the contents of that string 
    /// will become the display window's name.
    UpdatableImage(const vil_image_view<double>& img, const char* name = 0);

    /// \brief Destructor.  This will also remove the Image<double> from screen for good.
    ~UpdatableImage();

    /// \brief Update the displayed image
    ///
    /// \param img the displayed image will be replaced by the contents of this image.
    /// \param reshape if \c true, the display window will be reshaped to the shape of the
    ///                argument \c img.  Otherwise, it will retain its old shape although
    ///                its image contents has changed (resulting in a displayed image with
    ///                a possibly altered aspect ratio).
    void update(const vil_image_view<double>& img, bool reshape = false);

    /// \brief invoking this method will halt the program while the user is allowed to interact
    /// with the displayed image.
    ///
    /// The user is able to do the following after this method is called:
    /// \li Inspecting the RGB value of a pixel of the UpdatableImage (and the corresponding pixels
    /// in all the UpdatableImage's connected UpdatableImages) by clicking on it with the left
    /// mouse button.  Result will be written to standard
    /// output.
    /// \li Resize the display window (and all its connected UpdatableImages display windows).
    /// \li Using the middle mouse button for something.  No functionality for this is yet implemented,
    /// but it should be easy for the developer to fill in something useful.
    /// \li return control to the program by pushing space bar or the right mouse button.
    void interact(); // user interaction until user leaves control

    /// \brief connect this UpdatableImage with another one.
    /// 
    /// Connected UpdatableImages will react in unison when the user resizes one of them or when she
    /// inspects pixel values.
    void connect(UpdatableImage& rhs);
private:
    /// pointer-to-implementation paradigm used here in order to avoid having to include the very
    /// long header file CImg.h into this header file.
    Pimpl* p_;
};

}; // end namespace lsseg

#endif // _CIMG_DEPENDENT_H



