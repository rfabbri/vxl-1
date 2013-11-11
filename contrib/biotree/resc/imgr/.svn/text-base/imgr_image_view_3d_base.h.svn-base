// This is biotree/resc/imgr/imgr_image_view_3d_base.h
#ifndef imgr_image_view_3d_base_h_
#define imgr_image_view_3d_base_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A base class reference-counting view of a set of 2-d images
// \author J.L. Mundy (after Ian Scott's vil3d design)
//
// The base class is necessary so that we can pass views around without
// regard for the underlying template type. The base class is also ref-counted
// to support smart pointers.  A 3-d view should typically be created 
// on the stack by the scan resource, i.e. using "new".
//
// \verbatim
//  Modifications <none>
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_pixel_format.h>

class imgr_image_view_3d_base : public vbl_ref_count
{
 protected:
  //: Number of columns in each image
  unsigned ni_;
  //: Number of rows in each image
  unsigned nj_;
  //: Number of 2-d image views
  unsigned nk_;
  //: Number of planes (components in a 2-d image view)
  unsigned nplanes_;
  //: Pixel format
   enum vil_pixel_format format_;

 public:
  imgr_image_view_3d_base(unsigned ni, unsigned nj, unsigned nk,
                          unsigned nplanes, enum vil_pixel_format format):
  ni_(ni), nj_(nj), nk_(nk), nplanes_(nplanes), 
  format_(format) {}

  //: Default is an empty one-plane image
  //  Don't set nplanes_ to zero as it confuses set_size(nx,ny,nz) later
  imgr_image_view_3d_base(): ni_(0), nj_(0), nk_(0), nplanes_(1), 
  format_(VIL_PIXEL_FORMAT_UNKNOWN) {}


  // The destructor must be virtual so that the image storage is destroyed
  virtual ~imgr_image_view_3d_base() {}

  //: Width
  unsigned ni()  const {return ni_;}
  //: Height
  unsigned nj()  const {return nj_;}
  //: Number of images (virtual to allow checking of nk_ against actual number)
  virtual unsigned nk()  const {return nk_;}

  //: Number of planes(components) in a 2-d image, e.g. RGB
  unsigned nplanes() const {return nplanes_;}

  //: The number of pixels.
  unsigned long size() const { return ni_ * nj_ * nk_ * nplanes_; }

  //: Return a description of the concrete data pixel type.
  // For example if the value is VIL_PIXEL_FORMAT_BYTE,
  // you can safely cast, or assign the base class reference to
  // a imgr_image_view_3d<vxl_byte>.
  enum vil_pixel_format pixel_format() const {return format_;}
};
#include <resc/imgr/imgr_image_view_3d_base_sptr.h>
#endif // imgr_image_view_3d_base_h_
