// This is /contrib/biotree/proj/bioproj_io.h

#ifndef bioproj_io_h_
#define bioproj_io_h_

//: 
// \file    bioproj_io.h
// \brief   a class for the input and output issues of bioproj
// \author  H. Can Aras
// \date    2006-07-12
// 
// add a function to check for file name extensions
// we assume that the camera is on -y axis and rotates towards x axis for now
// we assume square image pixels on the sensor for now

#include <vcl_string.h>
#include <xscan/xscan_scan.h>
#include <vgl/vgl_box_3d.h>

class bioproj_io{
public:
  //: constructor getting the parameters one-by-one
  bioproj_io(vcl_string scan_fname, vcl_string box_fname, 
          double resolution, double sensor_pix_size, 
          double source_origin_dist, double source_sensor_dist,
          vnl_int_2 const sensor_dim, double rot_step_angle, vcl_string file_path,
          double sigma_r, double sigma_z, int sigma_r_extent, int sigma_z_extent,
          int num_comp_planes);

  xscan_scan scan_;
  //: in millimeters (world coordinates)
  vgl_box_3d<double> box_;
  //: in microns
  double resolution_;
  //: in microns
  double sensor_pix_size_;
  //: in millimeters
  double source_origin_dist_;
  //: in millimeters
  double source_sensor_dist_;
  vnl_int_2 const sensor_dim_;
  unsigned nviews_;
  //: in degrees
  double rot_step_angle_;
  vcl_string file_path_;

  //: in microns, used either for sigma_x or sigma_y
  double sigma_r_;
  //: in microns
  double sigma_z_;
  //: the view filter size is this*sigma_r at each side of the filter center
  int sigma_r_extent_;
  //: the spatial filter size is this*sigma_z at each side of the filter center
  int sigma_z_extent_;
  //: additional number of (theta,xi)-planes extracted from the volume to compensate
  //  for the wobble angle effect, should be picked carefully such that all needed
  //  planes are extracted, but we do not go outside the view images volume
  int num_comp_planes_;
};

#endif
