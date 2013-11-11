//: 
// \file  imgr_isq_file_format.cxx
// \brief read isq file as a set of image file 
//NOTE: that is Brown Copyrighted material which contains Scanso 
// proprietary information. Please don't disseminate any information 
// from this file.
// \author    Kongbin Kang
// \date        2005-04-29
//
//
#include "imgr_isq_file_format.h"
#include <vcl_cstring.h>
#include <vcl_algorithm.h> //  std::swap()
#include <vil/vil_image_view.h>
#include <vcl_exception.h>
 
const char *imgr_isq_format_tag = "isq";

char const* imgr_isq_file_format :: tag() const
{
  return imgr_isq_format_tag;
}

imgr_isq_file_format :: imgr_isq_file_format(vil_stream* vs)
{
  stream_ = vs;

  if(! stream_)
    vcl_throw vcl_exception();

  if(! stream_->ok())
    vcl_throw vcl_exception();

  imgr_isq_header header(stream_);
  
  if(header.is_valid()){

    xsize_ = header.dimx_um_ *0.001;

    ysize_ = header.dimy_um_ * 0.001;

    dx_ = header.dimx_um_ / header.dimx_p_ * 0.001;
    dy_ = header.dimy_um_ / header.dimy_p_ * 0.001;
    dz_ = header.dimz_um_ / header.dimz_p_  * 0.001;
  }else{
    xsize_ = ysize_ = dx_ = dy_ = dz_ = 0;
  }
}

vcl_vector<vil_image_resource_sptr> 
imgr_isq_file_format :: get_images()
{
  vcl_vector<vil_image_resource_sptr> images;

  images.clear();

  imgr_isq_header header(stream_);

  if(header.is_valid()){

    min_intensity_ = 0;

    max_intensity_ = header.max_data_value_ - header.min_data_value_ ;
    
    unsigned npics = static_cast<unsigned>(header.dimz_p_);

    for(unsigned i = 0; i<npics; i++){
            
      vil_image_resource_sptr img_sptr = new imgr_isq_image(stream_, &header, i);

      images.push_back(img_sptr);
      
    }

    
  }
  return images;
}

//: return a world point  
vgl_point_3d<double> imgr_isq_file_format::world_point(double i, double j, double k)
{


    return vgl_point_3d<double>(i*dx_ - 0.5*xsize_ , -j*dy_ + 0.5*ysize_, -k*dz_);
  
}

imgr_isq_image :: imgr_isq_image(vil_stream *is, imgr_isq_header* header, unsigned img_id) 
   : is_(is)
{
  is_ -> ref();

  if(is_->ok())
  {
    int data_offset = header->data_offset_;

   
    ni_ = header->dimx_p_;
    
    nj_ = header->dimy_p_;

    min_intensity_ = header->min_data_value_;

    max_intensity_ = header->max_data_value_;

    nplanes_ = 1;
#if VXL_HAS_INT_64
    start_pos_ = (vxl_int_64)512 + data_offset * 512 + (vxl_int_64)(img_id) * ni_ * nj_ * sizeof (vxl_uint_16);
#else
    start_pos_ = (vxl_int_32)512 + data_offset * 512 + (vxl_int_32)(img_id) * ni_ * nj_ * sizeof (vxl_uint_16);
    
#endif
  }
  else
  {
    start_pos_ = 0;
    ni_ = nj_ = 0; nplanes_ = 1;
  }

  format_ = VIL_PIXEL_FORMAT_UINT_16;

}

imgr_isq_image :: ~imgr_isq_image()
{
  is_->unref();
}

#if 0
imgr_isq_image :: imgr_isq_image(vil_stream* is,
                 unsigned int ni, unsigned int nj, unsigned int nplanes,
                 vil_pixel_format format)
  : ni_(ni), nj_(nj), nplanes_(nplanes), format_(format)
{

  
}

#endif

vil_image_view_base_sptr imgr_isq_image::get_copy_view(unsigned i0, unsigned ni,
    unsigned j0, unsigned nj) const
{
  if(i0+ni > ni_ || j0+nj > nj_)
    return 0;

  unsigned const byte_per_pixel = sizeof(vxl_uint_16);

  unsigned long want_bytes_per_raster = ni*byte_per_pixel;
  unsigned long have_bytes_per_raster = ni_ * byte_per_pixel;

  vil_memory_chunk_sptr buf = new 
    vil_memory_chunk(want_bytes_per_raster*nj, VIL_PIXEL_FORMAT_BYTE);

  // read each raster in turn. if the client wants the whole image, it may
  // be faster to read() it all in one chunk
  if(ni == ni_)
  {
    is_->seek(start_pos_ + have_bytes_per_raster*j0);
    is_->read(reinterpret_cast<vxl_byte*>(buf->data()), want_bytes_per_raster * nj);
  }
  else{
    for (unsigned j=0; j<nj; ++j)
    {
      is_->seek(start_pos_ + have_bytes_per_raster*(j+j0) + i0*byte_per_pixel);
      is_->read(reinterpret_cast<vxl_byte *>(buf->data()) + want_bytes_per_raster*j, want_bytes_per_raster);
    }
  }

  // correct negative part
  vxl_int_16* shorts = reinterpret_cast<vxl_int_16*> (buf->data());
  for(unsigned k=0; k<ni*nj; k++){
    shorts[k] -= (short)min_intensity_;
  }
  
#if VXL_BIG_ENDIAN 
    vxl_byte* data = reinterpret_cast<vxl_byte*>( buf->data());
    
    for(unsigned k=0; k<want_bytes_per_raster*nj; k+=byte_per_pixel){
      vcl_swap(data[k], data[k+1]);
    }
#endif
  
   return new vil_image_view<vxl_uint_16>(
      buf,
      reinterpret_cast<vxl_uint_16*>(buf->data()),
      ni, nj, 1, 1, ni, 1);
}

 bool imgr_isq_image:: put_view(const vil_image_view_base& im, 
     unsigned i0, unsigned j0) 
{
  return false;
}

