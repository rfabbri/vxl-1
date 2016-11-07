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
// File: LevelSetFunction.h                                                  
//                                                                           
// Created: Fri Feb 17 13:29:58 2006                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_LevelSetFunction.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief File containing the definition of the
/// \ref lsseg::LevelSetFunction "level-set function representation" used in 
/// \ref lsseg "this library".
//                                                                           
//===========================================================================

#ifndef _LEVELSETFUNCTION_H
#define _LEVELSETFUNCTION_H

#include "dbseg_levelset_errormacros.h"
//#include "dbseg_levelset_Image.h"
#include <vil/vil_image_view.h>

#include "dbseg_levelset_Mask.h"

namespace lsseg {

//===========================================================================
/// \brief A class representing a \ref anchor_LevelSetFunction "level-set function"
///  in 1, 2 or 3D.
/// 
/// This class represents a \ref anchor_LevelSetFunction "level-set function" 
/// in 1, 2 or 3D.  It inherits from \ref Image, but implements some extra functionality 
/// used by level-set functions.  It also imposes the \em constraint that the
/// number of channels is limited to 1, and that the data type should be 
/// \c double.
class LevelSetFunction : public vil_image_view<double>
//===========================================================================
{
public:
    /// \brief constructor making a level-set function of zero extent.
    ///
    /// The created object is not directly useful for anything, but can be 
    /// assigned or \ref read() into.
    LevelSetFunction() : vil_image_view<double>(0, 0, 1) {}
    LevelSetFunction(int x, int y, int z = 1, double val = 0) 
    : vil_image_view<double>(x, y, z) {}
    
    virtual  ~LevelSetFunction() {}

    /// copy constructor
    LevelSetFunction(const LevelSetFunction& rhs) : vil_image_view<double>(rhs) {};
    
    /// \brief Quasi-copy constructor, where copying of image contents is optional. 
    ///
    /// Can be used when you want to create a LevelSetFunction of the same shape as a 
    /// previously existing one, but not necessarily with the same contents.
    /// \param rhs  the LevelSetFunction to be "copied"
    /// \param copy if \c false, only the \em shape will be copied; if \c true, 
    ///             the LevelSetFunction \em contents will be copied as well.
    LevelSetFunction(const LevelSetFunction& rhs, bool copy) 
    : vil_image_view<double>(rhs) {}

    /// assignment operator
    /*virtual vil_image_view<double>& operator=(// [{[eli edit const]}] vil_image_view<double>& rhs) {
    //eli edit single_channel_assert(rhs.numChannels());
    return eli edit vil_image_view<double>::operator=(rhs);
    }*/

    /// fill all pixels of LevelSetFunction with a particular value \c val.
    /*virtual LevelSetFunction& operator=(const double& val)
    {
    std::fill(data_.begin(), data_.end(), val);
    return *this;
    }*/

    /// \brief resize LevelSetFunction to the same shape as another one \c rhs.  
    /// Previous content will be destroyed.
    void resize(const LevelSetFunction& rhs)
    {
        set_size(rhs.ni(), rhs.nj(), rhs.nplanes());
    }

    /// \brief resize LevelSetFunction.  Previous content will be destroyed.
    ///
    /// \param x new resolution in x-direction
    /// \param y new resolution in y-direction
    /// \param z new resolution in z-direction
    /// \param channels new number of image channels.  Should be 1; anything
    ///                 else is a "breach of contract", and will throw an 
    ///                 exception.
    virtual void resize(int x, int y, int z= 1, int channels = 1) {
    //eli edid single_channel_assert(channels);
    vil_image_view<double>::set_size(x, y, z);
    }
    
    /// \brief swap contents of LevelSetFunction with those of an Image<double>.
    ///
    /// This is a legal operation as long as the number of channels of \c rhs is
    /// exactly 1.  Anything else will throw an exception.  
    virtual void swap(vil_image_view<double>& rhs) {
    //eli edit single_channel_assert(rhs.numChannels());
    //Image<double>::swap(rhs);

        /*vil_image_view<double> temp = rhs;
        rhs = this->c;
        this = temp;*/
    }
    
    /// \brief permute the LevelSetFunction's \c x, \c y, \c z and channel indices.
    ///
    /// \param perm pointer to an integer array of 4 elements, specifying the permutation to
    ///             be carried out.  The integer array should contain a permutation of the
    ///             four numbers \c 0, \c 1, \c 2 and \c 3.  If we assign these numbers
    ///             to the LevelSetFunction's indices \c x, \c y, \c z and \c channel respectively, this
    ///             array specifies how the indices will be rearranged.  The value at <tt> perm[i]</tt>
    ///             specifies which of the indices in the LevelSetFunction \em before mutation will be 
    ///             repositioned to index \c i \em after permutation.
    /// \note for \ref LevelSetFunction s, the channel indice is not supposed to be able to 
    ///       participate in permutations.  For that reason, it is obligatory that <tt> perm[3]</tt> = 3;
    ///       anything else will throw an execption.  The interface is however inherited from Image<double>, 
    ///       which does not have this restriction.
    virtual void permute(const int* const perm) {
    
        /*ALWAYS_ERROR_IF(perm[3] != 3, 
            "cannot permute the 'channel' of a LevelSetFunction");
    Image<double>::permute(perm);*/
    }    

    /// \brief read a LevelSetFunction from a stream, either in binary or ASCII format.
    ///
    /// \param is     the stream from which the Image will be read
    /// \param binary set this argument to \c true to read from a binary stream, 
    ///               \c false to read from an ASCII text stream.
    
    
    /*virtual void read(std::istream& is, bool binary=true) {
    LevelSetFunction tmp;
    tmp.Image<double>::read(is, binary);
    single_channel_assert(tmp.numChannels());
    swap(tmp);
    }*/

    //======================================================================
    //  Functions purely proper to the LevelSetFunction (not to be found in
    //  the Image<double> object).
    //======================================================================

    /// \brief get the norm of the LevelSetFunction's 2D gradient vector at position
    /// (\c x, \c y) in the (x,y)-plane, with \c z set to 0.
    /// 
    /// Overloading not used here for performance reasons.
    inline double gradientNorm2D(int x, int y) const;

    /// \brief get the norm of the LevelSetFunction's 3D gradient vector at position
    /// (\c x, \c y, \c z).
    inline double gradientNorm3D(int x, int y, int z) const;

    /// \brief get the curvature value in the (x, y)-plane of the LevelSetFunction
    /// at position (\c x, \c y), with \c z set to 0.
    inline double curvature2D(int x, int y) const;
    
    /// \brief get the curvature value (3D) of the LevelSetFunction at position
    /// (\c x, \c y, \c z).
    inline double curvature3D(int x, int y, int z) const;

    /// \brief Generate an Image of (a region of) the LevelSetFunction's curvature values in 
    ///        the (x, y) plane.
    ///
    /// \retval target this is the Image we want to generate.  It will be 
    ///                reshaped to the shape of the LevelSetFunction, and its pixels
    ///                will be set to the LevelSetFunction's curvature values in the (x, y) plane
    ///                for the corresponding positions.
    /// \param mask pointer to a \ref Mask that specifies which region of the LevelSetFunction
    ///             that is considered "active".  Only in this region will the curvature be 
    ///             computed.  If the mask pointer is 0, the whole domain of the LevelSetFunction
    ///             will be used.  See \ref Mask "here" for more information about masks.
    inline void curvature2D(vil_image_view<double>& target, Mask* mask = 0) const;

    /// \brief Generate an Image of (a region of) the LevlSetFunction's curvature values.
    ///
    /// \retval target this is the Image we want to generate.  It will be reshaped to the
    ///                shape of the LevelSetFunction, and its pixels will be set to the
    ///                LevelSetFunction's curvature values for the corresponding positions.
    /// \param mask pointer to a \ref Mask that specifies which region of the LevelSetFunction
    ///             that is considered "active".  Only in this region will the curvature be
    ///             computed.  If the mask pointer is 0, the whole domain of the LevelSetFunction
    ///             will be used.  See \ref Mask "here" for more information about masks.
    inline void curvature3D(vil_image_view<double>& target, Mask* mask = 0) const;

    /// \brief Generate an Image of the product of the LevelSetFunction's curvature in the
    /// (x, y)-plane and the norm of its 2D gradient vector.
    ///
    /// \retval target this is the Image we want to generate.  It will be reshaped to the 
    ///                shape of the LevelSetFunction, and each of its pixel values will 
    ///                be set to the corresponding curvature value in the (x, y)-plane of
    ///                the LevelSetFunction, multiplied by the norm of the LevelSetFunction's 
    ///                gradient vector at the same position.
    /// \param mask pointer to a \ref Mask that specifies which region of the LevelSetFunction
    ///             that is considered "active".  Only in this region will the curvature and 
    ///             gradient be computed.  If the mask pointer is 0, the whole domain of 
    ///             the LevelSetFunction will be used.  See \ref Mask "here" for more 
    ///             information about masks.
    inline void curvatureTimesGrad2D(vil_image_view<double>& target, Mask* mask = 0) const;

    /// \brief get product of the LevelSetFunction's curvature in the (x, y)-plane
    /// and the norm of its 2D gradient vector, at position (\c x, \c y), with
    /// \c z set to 0.
    inline double curvatureTimesGrad2D(int x, int y) const;

    /// \brief get the product of the LevelSetFunction's curvature and the norm
    /// of its gradient vector at position (\c x, \c y, \c z).
    inline double curvatureTimesGrad3D(int x, int y, int z) const;

    /// \brief Generate an Image of the product of the LevelSetFunction's curvature and norm
    /// of its gradient vector.
    ///
    /// \retval target this is the Image we want to generate.  It will be reshaped to the 
    ///         shape of the LevelSetFunction, and each of its pixel values will be set to the
    ///         corresponding curvature value of the LevelSetFunction, multiplied by the norm of 
    ///         the LevelSetFunction's gradient vector at the same position.
    /// \param mask pointer to a \ref Mask that specifies which region of the LevelSetFunciton
    ///             that is considered "active".  Only in this region will the curvature and 
    ///             gradient be computed.   If the mask pointer is 0, the whole domain of the 
    ///             LevelSetFunction will be used.  See \ref Mask "here" for more information 
    ///             about masks.
    inline void curvatureTimesGrad3D(vil_image_view<double>& target, Mask* mask = 0) const;

    /// \brief Generate an Image of the norm of the LevelSetFunction's gradient in the
    /// (x, y)-plane.
    ///
    /// \retval target this is the Image we want to generate.  It will be reshaped to the
    ///                shape of the LevelSetFunction, and each of its pixel values will be set
    ///                to the norm of the gradient at the corresponding position of the 
    ///                LevelSetFunction, as projected onto the (x, y)-plane.
    /// \param mask pointer to a \ref Mask that specifies which region of the LevelSetFunction
    ///             that is considered "active".  Only in this region will the gradient norm
    ///             be computed.   If the mask pointer is 0, the whole domain of the LevelSetFunction
    ///             will be used.  See \ref Mask "here" for more information about masks.
    inline void gradientNorm2D(vil_image_view<double>& target, Mask* mask = 0) const;

    /// \brief Generate an Image of the norm of the LevelSetFunction's gradient
    ///
    /// \retval target this is the Image we want to generate.  It will be reshaped to the shape
    ///                of the LevelSetFunction, and each of its pixel values will be set to the
    ///                norm of the gradient at the corresponding position of the LevelSetFunction.
    /// \param mask pointer to a \ref Mask that specifies which region of the LevelSetFunction
    ///             that is considered "active".  Only in this region will the gradient norm 
    ///             be computed.  If the mask pointer is 0, the whole domain of the LevelSetFunction
    ///             will be used.  See \ref Mask "here" for more information about masks.
    inline void gradientNorm3D(vil_image_view<double>& target, Mask* mask = 0) const;

    /// \brief Reinitialize a (2D) LevelSetFunction to become the \ref anchor_SignedDistanceFunction
    /// "signed distance function" from its \ref anchor_ZeroSet "zero-set".
    /// 
    /// \param mask pointer to a \ref Mask that specifies which region of the LevelSetFunction
    ///             that is considered "active" and should be reinitialized (the complementary
    ///             region will remain unchanged).  If the mask pointer is 0, the whole domain
    ///             will be reinitialized.
    /// \note This method is intended for 2D LevelSetFunctions (i.e. where the size along z is 
    ///       restricted to 1).  It can be run on 3D LevelSetFunctions too, but then each (x,y)-plane
    ///       will be reinitialized in 2D, independently of the others.  If you want a real
    ///       3D initialization of your LevelSetFunction, you should call the method 
    ///       \ref reinitialize3D() instead.
    void reinitialize2D(const Mask* mask = 0);

    /// \brief Reinitialize a (3D) LevelSetFunction to become the \ref anchor_SignedDistanceFunction
    /// "signed distance function" from its \ref anchor_ZeroSet "zero-set".
    ///
    /// \param mask pointer to a \ref Mask that specifies which region of the LevelSetFunction
    ///             that is considered "active" and should be reinitialized (the complementary
    ///             region will remain unchanged).  If the mask pointer is 0, the whole domain
    ///             will be reinitialized.
    /// \note This method is intended for 3D LevelSetFunctions (i.e. where the size along z is
    ///       superior to 1).  It can be run on 2D LevelSetFunctions too, but if you know your 
    ///       levelSetFunction is 2D, it is better to call the \ref reinitialize2D() method
    ///       instead, for performance reasons.
    void reinitialize3D(const Mask* mask = 0);
    
private:
    /// \brief function used to assure that the LevelSetFunction design contract is
    /// fulfilled.
    ///
    /// The function is used when checking that arguments to member functions
    /// do not risk changing the number of channels to more than one.  This
    /// in order to fulfill the LevelSetFunction design contract.
    void single_channel_assert(int num_chan) {
    ALWAYS_ERROR_IF(num_chan != 1, "Single channel assert failed.");
    }

    /// only used for internal communication of values
    mutable double cached_;
};

}; // end namespace lsseg

#include "dbseg_levelset_LevelSetFunction_implementation.h"

#endif // _LEVELSETFUNCTION_H



