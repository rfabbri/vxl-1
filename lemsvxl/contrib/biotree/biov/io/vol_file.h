// This is contrib/biotree/biov/io/vol_file.h

#ifndef _vol_file_h
#define _vol_file_h

// \file vol_file.h
// \brief Writes a VOL file, given a vbl_array_3d data type
// \author Michael Morgenstern
// \date March 9, 2005
//
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vxl_config.h>
#include <vbl/vbl_array_3d.h>
class vol_file {

   private:
    vxl_uint_8 magic_number_[4];
    vxl_uint_32 header_length_;
    vxl_uint_32 width_;
    vxl_uint_32 height_;
    vxl_uint_32 images_;
    vxl_uint_32 bits_per_voxel_;
    vxl_uint_32 index_bits_;
    float scaleX_, scaleY_, scaleZ_;
    float rotX_, rotY_, rotZ_;
    vcl_vector<vxl_uint_8> data_;

      
   public:
      vol_file(vbl_array_3d<vxl_uint_8>& source)
      {
         magic_number_[0] = 0x0B;
         magic_number_[1] = 0x7E;
         magic_number_[2] = 0x77;
         magic_number_[3] = 0x59;
         header_length_ = 52;
         width_ = source.get_row1_count();
         height_ = source.get_row2_count();
         images_ = source.get_row3_count();
         bits_per_voxel_ = 8;
         index_bits_ = 0;
         scaleX_ = .5;
         scaleY_ = .5;
         scaleZ_ = .5;
         rotX_ = 0;
         rotY_ = 0;
         rotZ_ = 0;
         data_.resize(width_ * height_ * images_);
         
   // fill data
   for (vxl_uint_32 i = 0; i < images_; i++)
      for (vxl_uint_32 j = 0; j < height_; j++)
         for (vxl_uint_32 k = 0; k < width_; k++)
            data_[i*height_*width_ + j*width_ + k] = source[k][j][i];

      }
      
      bool write(const char *filename_char);
};

  
  #endif
