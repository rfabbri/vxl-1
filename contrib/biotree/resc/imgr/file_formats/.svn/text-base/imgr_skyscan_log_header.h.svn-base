//: This is /contrib/biotree/resc/imgr/file_formats/imgr_skyscan_log_header.h

#ifndef imgr_skyscan_log_header_h_
#define imgr_skyscan_log_header_h_

//: 
// \file    imgr_skyscan_log_header.h
// \brief   Header information for the SkyScan log file 
//          NOTE: This is Brown copyrighted material which contains SkyScan 
//          proprietary information. Disseminating any information 
//          from this file is prohibited.
// \author  H. Can Aras
// \date    2005-06-14
// 

#include <vcl_cstdio.h>
#include <vxl_config.h>
#include <vcl_string.h>
#include <vcl_iostream.h>

class imgr_skyscan_log_header
{
public:
  imgr_skyscan_log_header() {};
  virtual ~imgr_skyscan_log_header() {};

  //: read parameters from file
  imgr_skyscan_log_header(vcl_FILE *fp);
  //: camera pixel size in X direction of the sensor
  double cam_pixel_size_;
  //: camera pixel size ratio X/Y
  double cam_xy_ratio_;
  //: rotation axis to source distance along principal ray
  double object_to_source_dist_;
  //: principal point to source distance
  double camera_to_source_dist_;
  //: number of projection files in the scan
  int number_of_files_;
  //: number of rows of the sensor
  int number_of_rows_;
  //: number of columns of the sensor
  int number_of_columns_;
  //: Y coordinate of principal point
  double optical_axis_;
  //: voxel size of reconstructed volume
  double image_pixel_size_;
  //: rotation step angle
  double rotation_step_;
  //: relative angular position of reconstructed volume and scanner coordinate system
  double cs_static_rotation_;
  //: number of actual camera pixels merged into one virtual camera pixel
  unsigned cam_pixel_multiplier_;
  vcl_string Use_360_Rotation_;

friend  vcl_ostream& operator << (vcl_ostream& stream, const imgr_skyscan_log_header& header);

private:
  //: correct the virtual camera pixel size
  void update_camera_pixel_size();
};

void x_write(vcl_ostream& oc, imgr_skyscan_log_header header, vcl_string name);

#endif
