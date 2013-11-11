#include "imgr_skyscan_log_header.h"
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <vsl/vsl_basic_xml_element.h>

//added Use_360_Rotation_ to calculate the number of files from rotation step and Use_360_Rotation_.If Use_360_Rotation_ is ON,
// then number of files = 360/abs(rotation_step_) .If it is OFF, then number of files = 180/abs(rotation_step_)

imgr_skyscan_log_header::imgr_skyscan_log_header(vcl_FILE *fp)
{
  cam_pixel_size_       = -1; cam_xy_ratio_      = -1; object_to_source_dist_= -1;
  camera_to_source_dist_= -1; number_of_files_   = -1; number_of_rows_       = -1;
  number_of_columns_    = -1; optical_axis_      = -1; image_pixel_size_     = -1;
  rotation_step_        = -1; cs_static_rotation_= -1; Use_360_Rotation_ = "ON";
  
  if(fp != NULL)
  {
    int eof_flag = 0;
    int line_number = 0;
    while(1)
    {
      vcl_string left, right;
      int lr_flag = 0; // 0 is left, 1 is right
      while(1)
      {
        char one;
        int scanned = fscanf(fp,"%c",&one);
        if(scanned == -1)
        {
          eof_flag = 1;
          break;
        }
        else if(one == '\n')
          break;
        else if(one == '=')
        {
          lr_flag = 1;
          continue;
        }
        else
        {
          if(lr_flag == 0)
            left.append(1,one);
          else
            right.append(1,one);
        }
      }
      if(eof_flag == 1)
        break;
      line_number++;
      // Only get the necessary parameters
      if(vcl_strncmp(left.data(), "Camera Pixel Size", 17) == 0)
        cam_pixel_size_ = vcl_atof(right.data());
      else if(vcl_strncmp(left.data(), "Camera X/Y Ratio", 16) == 0 || 
              vcl_strncmp(left.data(), "CameraXYRatio", 13) == 0)
        cam_xy_ratio_ = vcl_atof(right.data());
      else if(vcl_strncmp(left.data(), "Object to Source", 16) == 0)
        object_to_source_dist_ = vcl_atof(right.data());
      else if(vcl_strncmp(left.data(), "Camera to Source", 16) == 0)
        camera_to_source_dist_ = vcl_atof(right.data());
      else if(vcl_strncmp(left.data(), "Number Of Files", 15) == 0 ||
               vcl_strncmp(left.data(), "Number of Files", 15) == 0 )
        number_of_files_ = static_cast<int>(vcl_atof(right.data()));
      else if(vcl_strncmp(left.data(), "Number Of Rows", 14) == 0 || 
              vcl_strncmp(left.data(), "Number of Rows", 14) == 0)
        number_of_rows_ = static_cast<int>(vcl_atof(right.data()));
      else if(vcl_strncmp(left.data(), "Number Of Columns", 17) == 0 || 
              vcl_strncmp(left.data(), "Number of Columns", 17) == 0)
        number_of_columns_ = static_cast<int>(vcl_atof(right.data()));
      else if(vcl_strncmp(left.data(), "Optical Axis", 12) == 0)
        optical_axis_ = vcl_atof(right.data());
      else if(vcl_strncmp(left.data(), "Pixel Size", 10) == 0 ||
              vcl_strncmp(left.data(), "Image Pixel Size", 16) == 0)
        image_pixel_size_ = vcl_atof(right.data());
      else if(vcl_strncmp(left.data(), "Rotation Step", 13) == 0 || 
              vcl_strncmp(left.data(), "Rotation Step (deg)", 19) == 0)
        rotation_step_ = vcl_atof(right.data());
      else if(vcl_strncmp(left.data(), "CS Static Rotation", 18) == 0)
        cs_static_rotation_ = vcl_atof(right.data());
      else if(vcl_strncmp(left.data(), "Use 360 Rotation", 16) == 0)
        Use_360_Rotation_  = right.data();
    }
  }
  if (number_of_files_ == -1)
  {
          vcl_cerr << "No number of files parsed from data file ... calculating\n";
    if (vcl_strncmp(Use_360_Rotation_.data(), "ON", 2) == 0 || 
        vcl_strncmp(Use_360_Rotation_.data(), "YES", 3) == 0)
      number_of_files_ = int(360/fabs(rotation_step_));
    else
      number_of_files_ = int(180/fabs(rotation_step_));

    vcl_cerr << "Calculated # of files to be : " << number_of_files_ << "\n";
  }
  update_camera_pixel_size();
}

void imgr_skyscan_log_header::update_camera_pixel_size()
{
  if(vcl_ceil(image_pixel_size_) == 9)
    cam_pixel_multiplier_ = 1;
  else if(vcl_ceil(image_pixel_size_) == 18)
    cam_pixel_multiplier_ = 2;
  else
    cam_pixel_multiplier_ = 4;

  cam_pixel_size_ *= cam_pixel_multiplier_;
}

void x_write(vcl_ostream& os, imgr_skyscan_log_header header, vcl_string name)
{

  vsl_basic_xml_element element(name);
  element.add_attribute("cam_pixel_size", header.cam_pixel_size_);
  element.add_attribute("cam_xy_ratio", header.cam_xy_ratio_);
  element.add_attribute("object_to_source_dist", header.object_to_source_dist_);
  element.add_attribute("camera_to_source_dist", header.camera_to_source_dist_);
  element.add_attribute("number_of_files", header.number_of_files_);
  element.add_attribute("number_of_rows", header.number_of_rows_);
  element.add_attribute("number_of_columns", header.number_of_columns_);
  element.add_attribute("optical_axis", header.optical_axis_);
  element.add_attribute("image_pixel_size", header.image_pixel_size_);
  element.add_attribute("rotation_step", header.rotation_step_);
  element.add_attribute("cs_static_rotation", header.cs_static_rotation_);
  element.add_attribute("cam_pixel_multiplier", (int) header.cam_pixel_multiplier_);
  element.add_attribute("Use_360_Rotation", header.Use_360_Rotation_);
  element.x_write(os);
}

vcl_ostream& operator << (vcl_ostream& stream, const imgr_skyscan_log_header& header)
{
stream << " Camera Pixel Size (um)= " <<header.cam_pixel_size_ <<vcl_endl;
stream << " CameraXYRatio= " <<header.cam_xy_ratio_ <<vcl_endl;
stream << " Number of Files= " <<header.number_of_files_ <<vcl_endl;
stream << " Number of Rows=  " <<header.number_of_rows_ <<vcl_endl;
stream << " Number of Columns= " <<header.number_of_columns_ <<vcl_endl;
stream << " Object to source = " <<header.object_to_source_dist_ <<vcl_endl;
stream << " camera to source = " <<header.camera_to_source_dist_ << vcl_endl;
stream << " Optical Axis (line)= " <<header.optical_axis_ <<vcl_endl;
stream << " Rotation Step (deg)= " <<header.rotation_step_ <<vcl_endl;
stream << " Use 360 Rotation= " <<header.Use_360_Rotation_ <<vcl_endl;
stream << " Image Pixel Size (um)= " <<header.image_pixel_size_ <<vcl_endl;


return stream;
}

