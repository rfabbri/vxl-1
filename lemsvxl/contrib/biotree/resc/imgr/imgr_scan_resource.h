#ifndef imgr_scan_resource_h_
#define imgr_scan_resource_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief image resource for a multiview  scan 
// \author J. L. Mundy
// \date Feb. 27, 2005
// The resource holds a set of open image files so that files can 
// be read on demand to create a bounded 3-d image view of actual pixel data
// It is reasonable to open the files and check them for consistency so that
// the existence of the resource guarantees the availability of the image data.
//
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_pixel_format.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_image_resource.h>
#include <xscan/xscan_scan.h>
#include <imgr/imgr_image_view_3d_base_sptr.h>

class imgr_scan_resource : public vbl_ref_count
{
 protected:
  //: format
  enum vil_pixel_format format_;

  //: All the 2d image resources that make up this volume
  vcl_vector<vil_image_resource_sptr> resources_;

#if 0
  //: temporarily a dummy scan, eventually a real scan (sptr)
  imgr_dummy_scan scan_;
#endif
  xscan_scan scan_;

  //: the dimensions of the resource
  unsigned ni_;
  unsigned nj_;
  unsigned nk_;
  unsigned nplanes_;

  //: default constructor doesn't make sense
  imgr_scan_resource();
  public:

  //: only reasonable constructor. The resources have been loaded by scan_io
  imgr_scan_resource(xscan_scan const& scan, 
                     vcl_vector<vil_image_resource_sptr> const& resources);

  virtual ~imgr_scan_resource(){};
  //:view access, for now we return only a bounded view
  // the full dataset will not fit into memory, so bound to the 3-d region of
  // interest
  imgr_image_view_3d_base_sptr
    get_bounded_view(vgl_box_3d<double> const& bounds);

  void crop_bound_2d(vgl_box_2d<double> const& box_2d, 
                                       unsigned &i0, unsigned &j0, 
                                       unsigned &imax, unsigned &jmax,
                                       unsigned int ni, unsigned int nj);

  //: Dimensions:  nplanes x ni x nj x nk.
  // This concept is treated as a synonym to components.
  virtual unsigned nplanes() const {return nplanes_;}
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of pixels in each row.
  virtual unsigned ni() const {return ni_;}
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of pixels in each column.
  virtual unsigned nj() const {return nj_;}
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of images per scan
  virtual unsigned nk() const {return nk_;}

  //: Pixel Format 
  enum vil_pixel_format pixel_format() const {return format_;}

};
#include <imgr/imgr_scan_resource_sptr.h>
#endif // imgr_scan_resource_h_
