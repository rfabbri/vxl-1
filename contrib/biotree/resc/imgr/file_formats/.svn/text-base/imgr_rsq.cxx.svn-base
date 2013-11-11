//: 
// \file  imgr_rsq.cxx
// \brief NOTE: that is Brown Copyrighted material which contains Scanso 
// proprietary information. Please don't disseminate any information 
// from this file.
// \author    Kongbin Kang
// \date        2005-04-21
//
//
#include "imgr_rsq.h"
#include <vcl_cstring.h>
#include <vcl_cmath.h>
#include <vcl_exception.h>
#include <vcl_algorithm.h> //  std::swap()
#include <vil/vil_image_view.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <vil/vil_stream_fstream.h>
#include <vnl/vnl_quaternion.h>
#include <xscan/xscan_uniform_orbit.h>

#include <xscan/xscan_dummy_scan.h>
#include <dbil/dbil_bounded_image_view.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/algo/vpgl_project.h>
#include <imgr/imgr_bounded_image_view_3d.h>
#include "imgr_ff_algos.h"

const char *imgr_rsq_format_tag = "rsq";

imgr_rsq:: imgr_rsq(const char*fname)
{

  stream_ = new vil_stream_fstream(fname, "r");

  if(stream_)  stream_ -> ref();

  //: the index  of image with shutter close
    dark_index_ = 0;

    //: the index of image with shutter full open
    bright_index_ = 1;
     // camera. Note the unit in the camera coordinate is milimeter

  
}

imgr_rsq:: imgr_rsq(vil_stream *vs )
  : imgr_scan_images_resource()
  
{
  stream_ = vs;

  if(vs)  stream_ -> ref();

  //: the index  of image with shutter close
    dark_index_ = 0;

    //: the index of image with shutter full open
    bright_index_ = 1;
     // camera. Note the unit in the camera coordinate is milimeter
 
}

char const* imgr_rsq :: tag() const
{
  return imgr_rsq_format_tag;
}


xscan_scan imgr_rsq :: get_scan() const
{
  imgr_rsq_header header(stream_);
 
  if( ! header.is_valid() ){
    
    vcl_cerr << " not a valid rsq header in imgr_rsq :: get_scan()\n";
    throw  vcl_exception();
  }
  
  return imgr_rsq_scan(&header);
}

void imgr_rsq::set_scan(xscan_scan scan)
{
  cached_scan_ = scan;
}


vcl_vector<vil_image_resource_sptr> 
imgr_rsq :: get_images() const 
{
  return images(stream_);
}
    //: get first two calibration images
vcl_vector<vil_image_resource_sptr> imgr_rsq :: get_cali_images() const
{
  vcl_vector<vil_image_resource_sptr> images;

  images.clear();

  imgr_rsq_header header(stream_);

  if(header.is_valid()){

    //unused variable 
    //unsigned npics = static_cast<unsigned>(header.dimy_p_) ;

    for(unsigned i = 0 ; i<2; i++){
            
      vil_image_resource_sptr img_sptr = new imgr_rsq_image(stream_, &header, i);

      images.push_back(img_sptr);
      
    }

  }
  
  return images;
  
}


vcl_vector<vil_image_resource_sptr> 
imgr_rsq :: images(vil_stream* vs) const 
{
  
  vcl_vector<vil_image_resource_sptr> images;

  images.clear();

  imgr_rsq_header header(vs);

  if(header.is_valid()){


    unsigned npics = static_cast<unsigned>(header.dimy_p_) ;

    for(unsigned i = 2 ; i<npics; i++){
            
      vil_image_resource_sptr img_sptr = new imgr_rsq_image(vs, &header, i);

      images.push_back(img_sptr);
      
    }

    
  }
  return images;
}

vcl_vector<vil_image_resource_sptr> 
imgr_rsq :: get_images(unsigned int interval) const 
{
  return interval_images(stream_,interval);
}

vcl_vector<vil_image_resource_sptr> 
imgr_rsq :: interval_images(vil_stream* vs,unsigned int interval) const 
{
  
  vcl_vector<vil_image_resource_sptr> images;

  images.clear();

  imgr_rsq_header header(vs);

  if(header.is_valid()){


    unsigned npics = static_cast<unsigned>(header.dimy_p_) ;

    for(unsigned i = 2 ; i<npics; i = i+interval){
            
      vil_image_resource_sptr img_sptr = new imgr_rsq_image(vs, &header, i);

      images.push_back(img_sptr);
      
    }

  }

    return images;

}

imgr_rsq_scan :: imgr_rsq_scan(imgr_rsq_header *header)
{
  if(header){

   raw_area_type scan_head = header->u_.area_;

    double source_sensor_dist = scan_head.detector_distance_um_*0.001;
    double source_center_dist = scan_head.rotation_center_distance_um_*0.001;

    double pixel_size_u = scan_head.detector_length_x_um_ / scan_head.nr_of_det_x_*0.001;

    double pixel_size_v = scan_head.detector_length_y_um_ / scan_head.nr_of_det_y_* 0.001;

    double principle_u = scan_head.detector_center_x_p_ / 10.0 + scan_head.nr_of_det_x_;
    double principle_v = scan_head.detector_center_y_p_ / 10.0 + scan_head.nr_of_det_y_;


    vnl_double_3x3 m(0.0);

    m[0][0] = source_sensor_dist / pixel_size_u;

    m[0][1] = 0;

    m[0][2] = principle_u;

    m[1][1] = source_sensor_dist / pixel_size_v;

    m[1][2] = principle_v;

    m[2][2] = 1;

    vpgl_calibration_matrix<double> K(m);

    kk_ = K;

    // orbit
    vnl_double_3 rot_axis(0.0, 0.0, 1.0);
    
    double rot_step = header->dimy_mdeg_ / (header->dimy_p_ - 2) * 0.001 * 3.1415926 / 180;

    vnl_quaternion<double> rot(rot_axis, rot_step);

    vnl_double_3 t(0.0, 0.0, 0.0);


    // set up the first rotation.
    vnl_double_3x3 R0(0.0);

    R0[0][0] = 1;
    R0[1][2] = -1;
    R0[2][1] = 1;

    vnl_double_3x3 Rx(0.0);
    double alpha = header->u_.area_.detector_angle_x_mdeg_*0.001*3.1415926/180;
    Rx[0][0] = 1;
    Rx[1][1] = vcl_cos(alpha); Rx[1][2] = vcl_sin(alpha);
    Rx[2][1] = -vcl_sin(alpha); Rx[2][2] = vcl_cos(alpha);

    
    double beta = header->u_.area_.detector_angle_y_mdeg_*0.001*3.1415926/180;

    vnl_double_3x3 Ry(0.0);
    Ry[0][0] = vcl_cos(beta); Ry[0][2] = -vcl_sin(beta);
    Ry[1][1] = 1;
    Ry[2][0] = vcl_sin(beta); Ry[2][2] = vcl_cos(beta);


    double gamma = header->u_.area_.detector_angle_z_mdeg_*0.001*3.1415926/180;
    vnl_double_3x3 Rz(0.0);
    Rz[0][0] = vcl_cos(gamma); Rz[0][1] = vcl_sin(gamma);
    Rz[1][0] = -vcl_sin(gamma); Rz[1][1] = vcl_cos(gamma);
    Rz[2][2] = 1;



    R0 = R0*Rz*Ry*Rx;

    vnl_quaternion<double> r0(R0);

    // camera center in the world coordinates system
    vnl_double_3 center(0, -source_center_dist, 0);

    vnl_double_3 t0(-R0*center);

    xscan_orbit_base_sptr orbit = new xscan_uniform_orbit(rot, t, r0, t0);

    orbit_ = orbit;


    // number of views_ note: first two views are calibration views
    xscan_scan::n_views_ = header->dimy_p_ - 2;

   
  }
}

#if 1
//: get cropped images from original box
vcl_vector<dbil_bounded_image_view<double> *>  
  imgr_rsq :: get_cali_bnded_view(vgl_box_2d<double> const& bounds)
{
  vcl_vector<dbil_bounded_image_view<double>* > double_views_2d;

  xscan_scan scan = get_scan();
  vcl_vector<vil_image_resource_sptr> resources = get_images();

  vil_image_resource_sptr I_max = resources[bright_index_];

  vil_image_resource_sptr I_min = resources[dark_index_];

  //Fill the view_3d with image_view_2d(s)
  unsigned nk = scan.scan_size();

  for(orbit_index i=0; i<nk; ++i)
    {
      //compute the image bounds for each image by projecting the 3-d box
      xmvg_perspective_camera<double> cam = scan(i);

      //extract the bounds
     //The file-based resource
      vil_image_resource_sptr r = resources[i];

      vcl_cout << "calibrate and bound " << i << "th images\n";
           
      dbil_bounded_image_view<double>* bview = imgr_ff_algos::calibrate_image(r, I_max, I_min, bounds); 

      
      double_views_2d.push_back(bview);
    }

  return double_views_2d;
}
#endif

imgr_rsq_image :: imgr_rsq_image(vil_stream *is, const imgr_rsq_header * header, unsigned img_id) 
   : is_(is), header_(*header)
{

  if(is_)
    is_->ref();

  if(is_->ok())
  {
    int data_offset = header->data_offset_;

   
    ni_ = header->dimx_p_;
    
    nj_ = header->dimz_p_;

    nplanes_ = 1;
#if VXL_HAS_INT_64
    start_pos_ = 512 + data_offset * 512 + (vxl_int_64)img_id * ni_ * sizeof (vxl_uint_16);
#else
    start_pos_ = 512 + data_offset * 512 + (vxl_int_32)img_id * ni_ * sizeof (vxl_uint_16);
#endif
    

    bytes_per_page_ = header->dimy_p_ * ni_ * sizeof(vxl_uint_16);

    vni_ = 2*ni_;

    vnj_ = 2*nj_;

#if 0
    vcl_cout << "start_pos_ = " << start_pos_ << '\n';
#endif

  }
  else
  {
    start_pos_ = 0;
    ni_ = nj_ = 0; nplanes_ = 1;

    vni_ = vnj_ = 0;
  }

  format_ = VIL_PIXEL_FORMAT_UINT_16;

}

imgr_rsq_image :: ~imgr_rsq_image()
{
  if(is_ != 0){
    is_->unref();
    is_ = 0;
  }
}

//: since the scanco only has half sensor. we only need a good example for it.
vil_image_view_base_sptr imgr_rsq_image::get_raw_copy_view(unsigned i0, unsigned ni,
    unsigned j0, unsigned nj) const
{

  unsigned const byte_per_pixel = sizeof(vxl_uint_16);

  //: how many byte need to be read from file
  unsigned long want_bytes_per_raster ;
  
  // all in the data region
  if(i0 >= ni_)  want_bytes_per_raster =  ni*byte_per_pixel ;
  
  else 
    if( i0 + ni  > ni_) want_bytes_per_raster = (ni + i0 - ni_)*byte_per_pixel;
    else want_bytes_per_raster = 0;
  

  // how many bytes in the file per raw
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  unsigned long have_bytes_per_raster = ni_ * byte_per_pixel;
#endif

  // how many bytes in a raw of one view 
  unsigned long bytes_view_raster = ni*byte_per_pixel;

  unsigned num_byte_zeros = bytes_view_raster - want_bytes_per_raster;
  
  // if view outside of actual image, pad actual image with zeros.
  if(i0+ni > vni_ || j0+nj > vnj_ ){

    return 0;
    
  }
  else{
    vil_memory_chunk_sptr buf = new 
      vil_memory_chunk(bytes_view_raster*nj, VIL_PIXEL_FORMAT_BYTE);

    // part of the region asked is outside of real data
    if( j0 < nj_ || i0 < ni_){
      for(unsigned j=0; j<nj; j++){
        
        vxl_byte* tmp = reinterpret_cast<vxl_byte*>( buf->data() )+ bytes_view_raster*j;
       
        if(j + j0 < nj_){
          for(unsigned k = 0; k < bytes_view_raster; k++)
            *(tmp + k) = 0;
        }

        else{ // part of view is on the right of the real data
          for(unsigned k = 0; k < num_byte_zeros; k++)
            *(tmp+k) = 0;
// vcl_cout<< " offset "<<start_pos_ + (unsigned long)bytes_per_page_*(j + j0 - nj_)<<vcl_endl;
// if (start_pos_ + (unsigned long)bytes_per_page_*(j + j0 - nj_)<=2148450816)
{
          is_->seek(start_pos_ + (unsigned long)bytes_per_page_*(j + j0 - nj_));

          is_->read(tmp+num_byte_zeros, want_bytes_per_raster);
}
        }
      }
    } // end of crop with zero padding

    // normal image crop
    else{
      // read each raster in turn. if the client wants the whole image, it may
      // be faster to read() it all in one chunk
      for (unsigned j=0; j<nj; ++j)
      {
        is_->seek(start_pos_ + (unsigned long)bytes_per_page_*(j+j0 - nj_) + (unsigned long)(i0 - ni_)*byte_per_pixel);
        is_->read(reinterpret_cast<vxl_byte *>(buf->data()) + want_bytes_per_raster*(unsigned long)j, want_bytes_per_raster);

      }

    } // end of normal image crop

#if VXL_BIG_ENDIAN 
        vxl_byte* data = reinterpret_cast<vxl_byte*>( buf->data());

        
        for(unsigned k=0; k< bytes_view_raster*nj; k+=byte_per_pixel){
          vcl_swap(data[k], data[k+1]);
        }
#endif

    return new vil_image_view<vxl_uint_16>(
        buf,
        reinterpret_cast<vxl_uint_16*>(buf->data()),
        ni, nj, 1, 1, ni, 1);
  }
}

vil_image_view_base_sptr imgr_rsq_image :: get_copy_view(unsigned i0, unsigned ni, 
        unsigned j0, unsigned nj) const
{
  if(i0+ni > vni_ || j0+nj > vnj_ )
    return 0;
    
  else{
    vil_memory_chunk_sptr buf = new 
      vil_memory_chunk(ni*nj*sizeof(vxl_uint_16), VIL_PIXEL_FORMAT_BYTE);

    vxl_uint_16* data = reinterpret_cast<vxl_uint_16*> (buf->data());

    imgr_rsq_image dark(is_, &header_, 0);
    
    imgr_rsq_image bright(is_, &header_, 1);

   
    vil_image_view<vxl_uint_16> dark_view = dark.get_raw_copy_view(i0, ni, j0, nj);

    vil_image_view<vxl_uint_16> bright_view = bright.get_raw_copy_view(i0, ni, j0, nj);
    
    vil_image_view<vxl_uint_16> cur_view = this->get_raw_copy_view(i0, ni, j0, nj);

    // pixel value in dark view
    //used
    //vxl_uint_16* d = dark_view.top_left_ptr();

    // pixel value in bright view
    //unused
    //vxl_uint_16* b = bright_view.top_left_ptr();

    // pixel in current view
    vxl_uint_16* c = cur_view.top_left_ptr();


    unsigned long num_pixels = ni * nj;
    for(unsigned long l=0; l < num_pixels; l++){
      /*unused variable
      double cd = c[l] - d[l];
      double bd = b[l] - d[l];
      */
#if 0
      if(cd == 0 && bd == 0)
        data[l] = 0;
      else
        //data[l] = -vcl_log( double(cd) / bd)*65535;
        data[l] = static_cast<vxl_uint_16>(cd / bd * 65535 );
#endif
      data[l] = c[l];
    }

   
    return new vil_image_view<vxl_uint_16>(
        buf,
        reinterpret_cast<vxl_uint_16*>(buf->data()),
        ni, nj, 1, 1, ni, 1);
  }

}


 bool imgr_rsq_image:: put_view(const vil_image_view_base& im, 
     unsigned i0, unsigned j0) 
{
  return false;
}

