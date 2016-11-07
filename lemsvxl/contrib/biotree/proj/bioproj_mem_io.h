// This is /contrib/biotree/proj/bioproj_mem_io.h

#ifndef bioproj_mem_io_h_
#define bioproj_mem_io_h_

//: 
// \file    bioproj_mem_io.h
// \brief   a memory efficient class for the input and output issues of bioproj
// \author  H. Can Aras
// \date    2006-09-08
// 
// add a function to check for file name extensions
// we assume that the camera is on -y axis and rotates towards x axis for now
// we assume square image pixels on the sensor for now

#include <vcl_string.h>
#include <xscan/xscan_scan.h>
#include <vgl/vgl_box_3d.h>
#include <vil3d/vil3d_image_view.h>
#include <resc/imgr/file_formats/imgr_skyscan_log.h>
class bioproj_mem_io{
public:
  //: constructor getting the parameters one-by-one
  bioproj_mem_io(vcl_string log_fname, vcl_string scan_fname, vcl_string box_fname, vcl_string fname_pattern,
                 float i_zero, float voxel_size,
                 double sigma_r, double sigma_z);

  xscan_scan scan_;
  //: in millimeters (world coordinates)
  vgl_box_3d<double> box_;
  //: in microns 
  float voxel_size_;
  //: 
  float i_zero_;
  //: in voxels
  int grid_w_, grid_h_, grid_d_;
  //: in microns
  double sensor_pix_size_;
  //: in millimeters
  double source_origin_dist_;
  //: in millimeters
  double source_sensor_dist_;
  vnl_int_2 sensor_dim_;
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

  //: to hold the response values at grid points
  vil3d_image_view<float> grid_;
  //: parsed filenames
  vcl_vector<vcl_string> filenames_;
  bool found_filenames(){return (filenames_.size() > 0);}
};

#endif
