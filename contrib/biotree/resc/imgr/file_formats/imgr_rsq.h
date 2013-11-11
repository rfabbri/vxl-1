#ifndef imgr_rsq_h_
#define imgr_rsq_h_

//: 
// \file  rsq_file_format.h
// \brief  file format for scanco rsq data, 
// NOTE: that is Brown copyrighted material which contains Scanso 
// proprietary information. Please don't disseminate any information 
// from this file.
 
// \author    Kongbin Kang
// \date        2005-04-12
// 

// the camera coordinate system used in this class is as following:
//                                /
//                               /z
//                              /
//                             /
//                            /
//                           |---------------------x
//                           |
//                           |
//                           |
//                           |
//                           |
//                           |
//                           |
//                           |
//                           y
//                          
//  x-axis is the row direction of sensor plane, y-axis is the column direction of the senosr plane
//  z-axis is perpendicular to the sensor plane
//
//  The world coordinate system used in this class is:
//                            z
//
//                            |
//                            |        y
//                            |      /
//                            |     /
//                            |    / 
//                            |   /
//                            |  /
//                            | /
//                            |/
//                            /-------------- x
//   x-axis is about the row direction of the sensor plane, y-axis is pointing to the sensor plane
//   and z-axis is the rotation axis.


#include <vcl_vector.h>
#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_stream.h>
#include <vil/vil_pixel_format.h>
#include <imgr/imgr_scan_images_resource.h>
#include <imgr/imgr_image_view_3d_base_sptr.h>
#include <dbil/dbil_bounded_image_view.h>
#include "imgr_rsq_header.h"

class imgr_rsq : public imgr_scan_images_resource 
{
  public:
    virtual char const *tag() const;

    //: constructor
    imgr_rsq(vil_stream *vs = 0);

    //: construct from a file name
    imgr_rsq(const char* fname);

    virtual xscan_scan get_scan() const;

    virtual void set_scan(xscan_scan scan);

    virtual vcl_vector<vil_image_resource_sptr> get_images() const;
    vcl_vector<vil_image_resource_sptr> get_images(unsigned int interval) const;
    vcl_vector<vil_image_resource_sptr> interval_images(vil_stream* vs,unsigned int interval) const;

    //: get first two calibration images
    vcl_vector<vil_image_resource_sptr> get_cali_images() const;

    vcl_vector<dbil_bounded_image_view<double> *>  get_cali_bnded_view(
                                        vgl_box_2d<double> const& bounds);

    int max_intensity() const { return max_intensity_; }

    int min_intensity() const { return min_intensity_; }

    ~imgr_rsq() { if(stream_) stream_ ->unref(); }
   
  private:
    //: DEPRECATED: make a image resource from image id and vil_stream
    //
    vil_stream * stream_;

    //: It returns 4 time large of the original images because the sensor of scanco machine is
    // one quater of the full sensor. To avoid the problem of objects moving outside of 
    // images which is a trouble for vil), we use 0 padded virtual image.
    vcl_vector<vil_image_resource_sptr> images(vil_stream *vs) const;

    //: the index  of image with shutter close
    int dark_index_;

    //: the index of image with shutter full open
    int bright_index_;

    int min_intensity_;
    int max_intensity_;

};

class imgr_rsq_scan : public xscan_scan
{
  public:
    imgr_rsq_scan(imgr_rsq_header* is);
  
};

//: this is a helper class which should not used expliciently
class imgr_rsq_image : public vil_image_resource
{
  public:

    //: construct a image from stream. header store the header information of the rsq file
    // img_id from 0 tp number of views tells the image identification
    imgr_rsq_image(vil_stream * is, 
        const imgr_rsq_header * header, 
        unsigned int img_id);

    ~imgr_rsq_image();

#if 0
    imgr_rsq_image(vil_stream* is, unsigned ni, unsigned nj,
        unsigned nplanes = 1, vil_pixel_format format);
#endif

    //: Create a read/write view of a copy of this data.
    // \return 0 if unable to get view of correct size.
    virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
        unsigned j0, unsigned nj) const;

    //: TODO implement a real working put_view
    virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

    //: Dimensions.  Planes x W x H
    virtual unsigned int ni() const { return vni_; }
    virtual unsigned int nj() const { return vnj_; }
    virtual unsigned int nplanes() const { return nplanes_; }

    virtual enum vil_pixel_format pixel_format() const 
    {
      return VIL_PIXEL_FORMAT_UINT_16;
    }

    //: Extra property information
    virtual bool get_property(char const* tag, void* property_value = 0) const
    {
      return true;
    }

  protected:
    vil_image_view_base_sptr get_raw_copy_view(unsigned i0, unsigned ni, 
        unsigned j0, unsigned nj) const;


  private:

    //: real size of the image
    unsigned ni_;

    unsigned nj_;

    //: virtual size
    unsigned vni_;

    unsigned vnj_;

    unsigned nplanes_;

    //: stream of rsq file
    vil_stream* is_;

    imgr_rsq_header header_;

#if VXL_HAS_INT_64
    //: starting position in the rsq file
    vxl_int_64 start_pos_;

    //: number of bytes per page
    vxl_int_64 bytes_per_page_;
#else

    vxl_int_32 start_pos_;

    vxl_int_32 bytes_per_page_;
#endif

    vil_pixel_format format_;
};

#endif
