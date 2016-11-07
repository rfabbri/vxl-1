#ifndef imgr_isq_file_format_h_
#define imgr_isq_file_format_h_

//: 
// \file  isq_file_format.h
// \brief  file format for scanco rsq data
//NOTE: that is Brown Copyrighted material which contains Scanso 
// proprietary information. Please don't disseminate any information 
// from this file.
// \author    Kongbin Kang
// \date        2005-04-12
// 

#include <vcl_vector.h>
#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_pixel_format.h>
#include <vgl/vgl_point_3d.h>
#include "imgr_isq_header.h"

class imgr_isq_file_format 
{
  public:
    //: ctor
    imgr_isq_file_format(vil_stream *vs);

    virtual ~imgr_isq_file_format(){};

    virtual char const *tag() const;

    int max_intensity() const { return max_intensity_; }

    int min_intensity() const { return min_intensity_; }

    //: return it to a voxel world coordinates: i, j, k are 0-based index integer
    vgl_point_3d<double> world_point(double i, double j, double k);
   
    //: make a image resource from image id and vil_stream
    vcl_vector<vil_image_resource_sptr> get_images();

  private:

    //: string associated with the isq file
    vil_stream* stream_;

    int min_intensity_;
    int max_intensity_;

    //: specification of boxes
    double xsize_, ysize_, dx_, dy_, dz_;

};

class imgr_isq_image : public vil_image_resource
{
  public:

    //: construct a image from stream. header store the header information of the rsq file
    // img_id from 0 tp number of views tells the image identification
    imgr_isq_image(vil_stream* is, imgr_isq_header* header, unsigned int img_id);

    ~imgr_isq_image();

    //: Create a read/write view of a copy of this data.
    // \return 0 if unable to get view of correct size.
    virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
        unsigned j0, unsigned nj) const;

    //: TODO implement a real working put_view
    virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

    //: Dimensions.  Planes x W x H
    virtual unsigned int ni() const { return ni_; }
    virtual unsigned int nj() const { return nj_; }
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


  private:
    unsigned ni_;

    unsigned nj_;

    unsigned nplanes_;

    //: stream of rsq file
    vil_stream* is_;

    //: starting position in the rsq file
#if VXL_HAS_INT_64
    vxl_int_64 start_pos_;
#else
    vxl_int_32 start_pos_;
#endif

    //: number of bytes per page
    long bytes_per_page_;

    vil_pixel_format format_;

    short min_intensity_;
    short max_intensity_;
};

#endif
