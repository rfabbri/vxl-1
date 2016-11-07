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
// File: Image.h                                                             
//                                                                           
// Created: Tue Oct 25 13:57:00 2005                                         
//                                                                           
// Author: Odd A. Andersen <Odd.Andersen@sintef.no>
//                                                                           
// Revision: $Id: dbseg_levelset_Image.h,v 1.1 2009-06-09 22:29:14 fine Exp $
//                                                                           
// Description:
/// \file
/// \brief This file contains the definition of the \ref lsseg::Image "Image" template
/// class, used to represent images and multidimensional functions (level-sets, etc)
/// in \ref lsseg "this software package".
//                                                                           
//===========================================================================

#ifndef _IMAGE_H
#define _IMAGE_H

#include <algorithm>
#include <assert.h>
#include <vector>
#include <istream>
#include <ostream>
#include <numeric>
#include "dbseg_levelset_lambda.hpp"

using namespace boost;
using namespace boost::lambda;

namespace lsseg {

//===========================================================================
/// \brief An 1, 2 or 3-dimensional image with an arbitrary number of channels.

/// This class represents an 1, 2 or 3-dimensional image with an arbitrary number
/// of channels.  It is parametrized on its type, and can thus be used to represent
/// both images with floating point and integer (as well as boolean, i.e. 'mask')
/// values.
template<typename T>
class Image
//===========================================================================
{
public:
    /// \brief Get the value type of the Image<T> class
    typedef T value_type;

    /// \brief emtpy constructor - makes an image of size 0
    Image() 
    {std::fill(dim_, dim_ + 4, 0);}

    /// \brief constructor
    ///
    /// \param x resolution (in # pixels) in x-direction (minimum 1)
    /// \param y resolution (in # pixels) in y-direction (minimum 1)
    /// \param z resolution (in # pixels) in z-direction (minimum 1)
    /// \param chan number of channels in the image (minimum 1)
    /// \param fill_val which value to fill the pixels with (default is 0)
    Image(int x, int y, int z = 1, int chan = 1, T fill_val = 0) 
    : data_(x * y * z * chan, fill_val)
    { 
    dim_[0] = x;
    dim_[1] = y;
    dim_[2] = z;
    dim_[3] = chan;
    }
    
    /// destructor
    virtual ~Image() {}

    /// \brief Constructor converting from different type.
    ///
    /// Using implicit conversion to convert from the argument Image's 
    /// value type \c U to (*this) Image's value type \c T.
    template<typename U>
    Image(const Image<U>& rhs) {
    dim_[0] = rhs.dimx();
    dim_[1] = rhs.dimy();
    dim_[2] = rhs.dimz();
    dim_[3] = rhs.numChannels();
    data_.resize(size());
    assert(size() == rhs.size());
    copy(rhs.begin(), rhs.end(), &data_[0]);
    }

    /// copy constructor
    Image(const Image<T>& rhs) : data_(rhs.data_) 
    { std::copy(rhs.dim_, rhs.dim_ + 4, dim_);}

    /// \brief Quasi-copy constructor, where copying of image contents is optional. 
    ///
    /// Can be used when you want to create an Image of the same shape as a previously 
    /// existing image, but not necessarily with the same type or contents.
    /// \param rhs  the Image to be "copied"
    /// \param copy if \c false, only the image \em shape will be copied; if \c true, 
    ///             the image \em contents will be copied as well.
    template<typename U>
    Image(const Image<U>& rhs, bool copy) {
    dim_[0] = rhs.dimx();
    dim_[1] = rhs.dimy();
    dim_[2] = rhs.dimz();
    dim_[3] = rhs.numChannels();
    data_.resize(size());
    if (copy) {
        T* target = begin();
        for (const U* it = rhs.begin(); it != rhs.end(); ++it, ++target) {
        *target = T(*it);
        }
    }
    }
    
    /// assignment operator
    virtual Image<T>& operator=(const Image<T>& rhs) {
    Image<T> tmp(rhs);
    this->swap(tmp);
    return *this;
    }

    /// Fill all pixels of image with a particular value \c val.
    Image<T>& operator=(const T& val) {
    std::fill(data_.begin(), data_.end(), val);
    return *this;
    }

    /// get total size (in number of pixels) of an image
    unsigned long int size() const { return dim_[0] * dim_[1] * dim_[2] * dim_[3];}

    /// get size of each channel (in number of pixels) of an image
    unsigned long int channelSize() const { return size() / numChannels();}

    /// \brief get size of \em one x-y slice of \em one channel of an image 
    /// (i.e. # pixels in x-dimension multiplied by # pixels in y-dimension)
    unsigned long int graySize2D() const { return dimx() * dimy();}

    /// get number of pixels in x-direction
    int dimx() const { return dim_[0];}

    /// get number of pixels in y-direction
    int dimy() const { return dim_[1];}
    
    /// get number of pixels in z-direction
    int dimz() const { return dim_[2];}

    /// get number of separate image channels
    int numChannels() const { return dim_[3];}

    /// \brief resize image to the same shape as another image \c rhs.  
    /// Previous image content will be destroyed.
    template<typename U>
    void resize(const Image<U>& rhs) {
    resize(rhs.dimx(), rhs.dimy(), rhs.dimz(), rhs.numChannels());
    }
    
    /// \brief resize image.  Previous image content will be destroyed.
    ///
    /// \param x new resolution in x-direction
    /// \param y new resolution in y-direction
    /// \param z new resolution in z-direction
    /// \param channels new number of image channels
    virtual void resize(int x, int y, int z = 1, int channels = 1) 
    {
    dim_[0] = x;
    dim_[1] = y;
    dim_[2] = z;
    dim_[3] = channels;
    data_.resize(size());
    }
    
    /// swapping two images
    virtual void swap(Image<T>& rhs) {
    std::swap_ranges(dim_, dim_ + 4, rhs.dim_);
    data_.swap(rhs.data_);
    }

    /// \brief add another image to this one, pixel-by-pixel
    ///
    /// The Image added must be parametrized on the same type and 
    /// be \ref size_compatible.
    Image<T>& operator +=(const Image<T>& rhs) {
    assert(size_compatible(rhs));
    const T* from = rhs.begin();
    T* to;
    for (to = begin(); to != end(); ++to, ++from) {
        *to += *from;
    }
    return *this;
    }

    /// \brief subtract another image from this one, pixel-by-pixel
    ///
    /// The Image added must be parametrized on the same type and
    /// be \ref size_compatible.
    Image<T>& operator -=(const Image<T>& rhs) {
    assert(size_compatible(rhs));
    const T* from = rhs.begin();
    T* to;
    for (to = begin(); to != end(); ++to, ++from) {
        *to -= *from;
    }
    return *this;
    }

    /// \brief add a scalar value to \em all pixels in the image
    ///
    /// \param a a scalar value which will be added to \em all pixels
    ///          in the image, regardless of channel.
    Image<T>& operator +=(T a) {
    T* it;
    for (it = begin(); it != end(); ++it) {
        *it += a;
    }
    return *this;
    }

    /// \brief subtract a scalar value from \em all pixels in the image
    ///
    /// \param a a scalar value which will be subtracted from \em all
    ///          pixels in the image, regardless of channel.
    Image<T>& operator -=(T a) {
    return operator+=(-a);
    }

    /// \brief multiply \em all pixels in the image with a given scalar value.
    ///
    /// \param a the scalar value that all pixels in the image will be 
    ///          multiplied with, regardless of channel.
    Image<T>& operator *=(T a) {
    T* it;
    for (it = begin(); it != end(); ++it) {
        *it *= a;
    }
    return *this;
    }

    /// \brief divide all pixels in the image with a given scalar value.
    ///
    /// \param a the scalar value that all pixels in the image will be divided
    ///          with, regardless of channel.
    Image<T>& operator /=(T a) {
    const T denom_inv = T(1) / a;
    return (*this) *= denom_inv;
    }

    /// \brief get the maximum pixel value of the image (over all channels)
    T max() const { return *std::max_element(data_.begin(), data_.end()); }

    /// \brief get the minimum pixel value of the image (over all channels)
    T min() const { return *std::min_element(data_.begin(), data_.end()); }

    /// \brief Check if another image is "spatial compatible" with this image.
    /// 
    /// Two images are spatially compatible if they have the same resolutions in
    /// \c x, \c y and \c z.  They do \em not need to have the same number of 
    /// channels.
    template<typename U>
    bool spatial_compatible(const Image<U>& rhs) const {
    if (dimx() != rhs.dimx()) return false;
    if (dimy() != rhs.dimy()) return false;
    if (dimz() != rhs.dimz()) return false;
    return true;
    }


    /// \brief Check if another Image is "size compatible" with this image.
    /// 
    /// Two images are size compatible if they have the same resolutions in 
    /// \c x, \c y and \c z, \em and the same number of channels.
    template<typename U>
    bool size_compatible(const Image<U>& rhs) const {
    if (!spatial_compatible(rhs)) return false;
    if (numChannels() != rhs.numChannels()) return false;
    return true;
    }

    /// \brief Fill the whole image (all pixels in all channels) with a particular
    /// value \c val
    void fill(T val) { std::fill(data_.begin(), data_.end(), val); }

    /// \brief Set all pixels in the image to their absolute value.
    void setAbsolute() {
    for (T* p = begin(); p != end(); ++p) {
        if (*p < 0) *p = -(*p);
    }
    }

    /// \brief get the average value of all pixels in the image.
    T getAverage() const
    { return std::accumulate(data_.begin(), data_.end(), T(0)) / T(size()); }

    /// \brief get an iterator to the beginning of the image pixel storage array
    T* begin() { return &data_[0];}

    /// \brief get an iterator to the one-past-end of the image pixel storage array
    T* end() { return &data_[0] + size();}

    /// \brief get a \c const iterator to the beginning of the image pixel storage array
    const T* begin() const { return &data_[0];}

    /// \brief get a \c const iterator to the one-past-end of the image pixel storage array
    const T* end() const { return &data_[0] + size();}

    /// \brief get an iterator to the beginning of where a particular channel is stored
    /// in the image pixel storage array.
    ///
    /// \param channel the channel whose beginning of storage we want an iterator to
    T* channelBegin(int channel) { return &data_[0] + channel * channelSize();}

    /// \brief get an iterator to the one-past-end of where a particular channel is stored
    /// in the image pixel storage array.
    /// \param channel the channel whose one-past-end of storage we want an iterator to
    T* channelEnd(int channel) { return channelBegin(channel) + channelSize();}

    /// \brief get a \c const iterator to the beginning of where a particular channel is stored
    /// in the image pixel storage array.
    /// \param channel the channel whose beginning of storage we want an iterator to
    const T* channelBegin(int channel) const { return &data_[0] + channel * channelSize();}

    /// \brief get a \c const iterator to the one-past-end of where a particular channel is stored
    /// in the image pixel storage array.
    /// \param channel the channel whose one-past-end of storage we want an iterator to
    const T* channelEnd(int channel) const  { return channelBegin(channel) + channelSize();}

    /// \brief superpose an image \c img onto this image.
    ///
    /// The difference between the superpose() function and the operator+= is that the superpose()
    /// function does not require the two images to be \ref size_compatible, only that they have
    /// the same number of pixels in (at least) the x and y direction, and that the number of 
    /// channels and z-resolution  in \c *this image is either equal to, or exact multiples of
    /// the number of channels and z-resolution in the argument image \c img.  The image \c img
    /// will then be superposed to \c *this image such that it is repeated along the z-resolution
    /// and number of channels.
    /// \param img the image that will be superposed onto this image.  It should have the 
    ///            same resolution in the \c x and \c y directions as \c *this image, and that
    ///            the z-resolution of \c *this image is an exact multiple of the z-resolution of
    ///            \c img.  The same goes for the number of channels.
    void superpose(const Image<T>& img) {
    assert(dimx() == img.dimx() && dimy() == img.dimy()); // at least this must be correct
    assert(img.size() <= size());
    T* to = begin();
    while (to != end()) {
        std::transform(img.begin(), img.end(), to, to, _1 + _2);
        to += img.size();
    }
    }

    /// \brief Intersect another image with \c *this image.
    /// 
    /// The argument image \c img must be \ref size_compatible with \c *this image.
    /// The intersection operator \c && will then be applied to each pixel in this image
    /// with its equivalent in the argument image \c img.
    /// \param img the image that will intersect with \c *this image.  It should be 
    ///            \ref size_compatible with this image.
    void intersect(const Image<T>& img) {
    assert(size_compatible(img));
    std::transform(img.begin(), img.end(), begin(), begin(), _1 && _2);
    }

    /// \brief dump info about image resolution and number of channels to the
    /// stream \c os.  Useful for debug purposes.
    void dumpInfo(std::ostream& os) const {
    os << "-------------\n";
    os << "X: " << data_[0] << '\n';
    os << "Y: " << data_[1] << '\n';
    os << "Z: " << data_[2] << '\n';
    os << "Channels: " << data_[3] << std::endl;
    }

    /// \brief get the internal storage index of a pixel at position (\c x, \c y).
    ///
    /// The \c z position will be 0 and the channel will be 0.  Overloading is not
    /// used here for performance reasons.
    size_t indexOf(int x, int y) const {
    assert(x >= 0 && y >= 0);
    assert(x <= dimx() && y <= dimy());
    return x + dim_[0] * y;
    }

    /// \brief get the internal storage index of a pixel at position (\c x, \c y, \c z).
    ///
    /// The channel number will be 0.  Overloading is not used here for performance reasons.
    size_t indexOf(int x, int y, int z) const {
    assert(x >= 0 && y >= 0 && z >= 0);
    assert(x <= dimx() && y <= dimy() && z <= dimz());
    return x + dim_[0] * (y + dim_[1] * z);
    }

    /// \brief get the internal storage index of a pixel at position (\c x, \c y, \c z), and
    /// for the channel \c c.
    size_t indexOf(int x, int y, int z, int c) const {
    assert(x >= 0 && y >= 0 && z >= 0 && c >= 0);
    assert(x <= dimx() && y <= dimy() && z <= dimz() && c <= numChannels());
    return x + dim_[0] * (y + dim_[1] * (z + dim_[2] * c));
    }

    /// \brief read/write access to the pixel value at position (\c x, \c y)
    ///
    /// The \c z position will be 0 and the channel will be 0.  Overloading is not
    /// used here for performance reasons.
    T& operator()(int x, int y) { return data_[indexOf(x, y)];}

    /// \brief read/write access to the pixel value at position (\c x, \c y, \c z).
    /// 
    /// The channel number will be 0.  Overloading is not used here for performance reasons.
    T& operator()(int x, int y, int z) { return data_[indexOf(x, y, z)];}

    /// \brief read/write access to the pixel value at position (\c x, \c y, \c z), and for 
    /// the channel \c c.
    T& operator()(int x, int y, int z, int c) { return data_[indexOf(x, y, z, c)];}

    /// \brief read-only access to the pixel value at position (\c x, \c y)
    /// 
    /// The \c z position will be 0 and the channel will be 0.  Overloading is not
    /// used here for performance reasons.
    const T& operator()(int x, int y) const { return data_[indexOf(x, y)];}

    /// \brief read-only access to the pixel value at position (\c x, \c y, \c z).
    ///
    /// The channel number will be 0.  Overloading is not used here for performance reasons.
    const T& operator()(int x, int y, int z) const { return data_[indexOf(x, y, z)];}

    /// \brief read-only access to the pixel value at position (\c x, \c y, \c z), and for
    /// the channel \c c.
    const T& operator()(int x, int y, int z, int c) const { return data_[indexOf(x, y, z, c)];}

    /// \brief read/write access to the pixel value at internal storage position \c ix.
    /// 
    /// The internal storage position can be found using the \ref indexOf() function.
    T& operator[](unsigned int ix) {
    return data_[ix];
    }

    /// \brief read-only access to the pixel value at internal storage position \c ix.
    /// 
    /// The internal storage position can be found using the \ref indexOf() function.
    const T& operator[](unsigned int ix) const {
    return data_[ix];
    }

    /// \brief permute the image's \c x, \c y, \c z and channel indices.
    ///
    /// \param perm pointer to an integer array of 4 elements, specifying the permutation to
    ///             be carried out.  The integer array should contain a permutation of the
    ///             four numbers \c 0, \c 1, \c 2 and \c 3.  If we assign these numbers
    ///             to the image indices \c x, \c y, \c z and \c channel respectively, this
    ///             array specifies how the indices will be rearranged.  The value at <tt> perm[i]</tt>
    ///             specifies which of the indices in the image \em before mutation will be 
    ///             repositioned to index \c i \em after permutation.
    virtual void permute(const int* const perm)
    {
    Image<T> tmp_img(dim_[perm[0]], dim_[perm[1]], dim_[perm[2]], dim_[perm[3]]);
    int ix[4];

    const T* dp = begin();
    for (ix[3] = 0; ix[3] != numChannels(); ++ix[3]) {
        for (ix[2] = 0; ix[2] != dimz(); ++ix[2]) {
        for (ix[1] = 0; ix[1] != dimy(); ++ix[1]) {
            for (ix[0] = 0; ix[0] != dimx(); ++ix[0]) {
            const int Xperm = ix[perm[0]];
            const int Yperm = ix[perm[1]];
            const int Zperm = ix[perm[2]];
            const int Cperm = ix[perm[3]];
            tmp_img.data_[tmp_img.indexOf(Xperm, Yperm, Zperm, Cperm)] = *dp++;
            }
        }
        }
    }
    swap(tmp_img);
    
//     Go::GoBorrowedMVGrid<4, T> tmp_grid = getGrid();
//     tmp_grid.permuteElements(perm);
//     for (int i = 0; i < 4; ++i) {
//         dim_[i] = tmp_grid.rowlength(i);
//     }
    }

    /// \brief write the Image to a stream, either in ASCII or in binary format.
    ///
    /// \param os     the stream onto which the Image information will be written
    /// \param binary set this to \c false to write in ASCII, \c true to write in binary format.
    void write(std::ostream& os, bool binary=true)  const
    {
    if (binary) {
        // writing to file in binary format

        // platform-dependent stuff
        assert(sizeof(double) == 8);
        assert(sizeof(float) == 4);
        assert(sizeof(int) == 4);

        const char* dp = reinterpret_cast<const char*>(dim_);
        int dim_storage_size = 4 * sizeof(int);
        os.write(dp, dim_storage_size);

        int total_data_size = size() * sizeof(T);
        dp = reinterpret_cast<const char*>(&data_[0]);
        os.write(dp, total_data_size);

    } else {
        // writing to file in ASCII format
        for (int d = 0; d != 4; ++d) {
        os << dim_[d] << '\n';
        }
        for (size_t i = 0; i != data_.size(); ++i) {
        os << data_[i] << ' ';
        }
        os << std::endl;
    }
    }


    /// \brief read an Image from a stream, either in binary or ASCII format.
    ///
    /// \param is     the stream from which the Image will be read
    /// \param binary set this argument to \c true to read from a binary stream, 
    ///               \c false to read from an ASCII text stream.
    virtual void read(std::istream& is, bool binary=true) 
    {
    Image<T> tmp_img;

    if (binary) {
        // read in binary format

        // platform-dependent stuff
        assert(sizeof(double) == 8);
        assert(sizeof(float) == 4);
        assert(sizeof(int) == 4);

        int dim[4];
        int dim_storage_size = 4 * sizeof(int);
        char* dp = reinterpret_cast<char *>(dim);
        is.read(dp, dim_storage_size);
        tmp_img.resize(dim[0], dim[1], dim[2], dim[3]);

        int total_data_size = tmp_img.size() * sizeof(T);
        dp = reinterpret_cast<char*>(tmp_img.begin());
        is.read(dp, total_data_size);
        
    } else {
        // read in ascii format
        int dim[4];
        for (int i = 0; i != 4; ++i) {
        is >> dim[i];
        }
        tmp_img.resize(dim[0], dim[1], dim[2], dim[3]);
        T* dpointer = tmp_img.begin();
        for (size_t i = 0; i != tmp_img.size(); ++i) {
        is >> *dpointer;
        ++dpointer;
        }
    }
    swap(tmp_img);
    }

    /// \brief mask out parts of the Image by setting the concerned pixel values to 0.
    ///
    /// \param mask The \ref Mask is a (typically one-channeled) image whose pixel values are 
    ///             interpreted as boolean values.  Read more about masks \ref Mask "here".
    ///             the \c mask argument should be \ref spatial_compatible() with this
    ///             image, and each pixel in this image whose corresponding pixel in 
    ///             \c mask is \c false, will be set to zero ("masked out") by applying this 
    ///             function.  (If this image is multichanneled and the \c mask single-channeled, 
    ///             the mask will be applied to each channel separately).\n
    ///             \b Note: Although \ref Mask s represent boolean values, their pixel
    ///             value type is \c char.  The reason is that this in our case has proven
    ///             to be the best choice performance-wise.
    void applyMask(const Image<char>& mask) {
    assert(spatial_compatible(mask));
    for (int ch = 0; ch  < numChannels(); ++ch) {
        const char* mp = mask.begin();
        T* ch_end = channelEnd(ch);
        for (T* pt = channelBegin(ch); pt != ch_end; ++pt, ++mp) {
        *pt = (*mp) ? *pt : 0;
        }
    }
    }

    /// \brief make an Image that is a copy of one of the channels of \c *this image.
    ///
    /// \param target this is the image that will become a copy of a channel of \c *this image.
    /// \param ch     the channel number in question.
    void makeChannelImage(Image<T>& target, int ch) 
    {
    target.resize(dimx(), dimy(), dimz(), 1);
    std::copy(begin() + ch * channelSize(), 
          begin() + (ch + 1) * channelSize(),
          target.begin());
    }

protected:
    /// stores the size of the \c x, \c y and \c z dimensions, as well as the number of channels.
    int dim_[4];

    /// STL vector storing the image pixels.
    std::vector<T> data_;
};

}; // end namespace lsseg

#endif // _IMAGE_H



