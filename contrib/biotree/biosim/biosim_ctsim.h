// This is /contrib/biotree/biosim/biosim_ctsim.h

#ifndef biosim_ctsim_h_
#define biosim_ctsim_h_

//: 
// \file    biosim_ctsim.h
// \brief   CT-Simulator
//
//          Simulates CT using a circular orbit around the specified axis
//          The rotation axis is assumed to pass through the origin
//          We use no noise filter class files to represent the cylinder,
//          the only difference is in the splatting part, i.e. the cylinder
//          here does not have a negative component since it is not a filter.
//          
//          A typical input file looks like below:
//
//          biosim_ctsim_input
//          r 0.0 -0.5 -0.5 0.5 0.5 0.5
//          c (0.05,0.0,0.0) 0.05 0.2 (0.0,0.0,1.0)
//          e
//
//          The first line is common for all input files. It must be included.
//          A line starting with the letter 'c' describes a cylinder in 3D
//          with center, radius, length and orientation information respectively.
//          A line starting with the letter 'r' describes a cuboid (rectangular
//          parallelepiped) with min_x, min_y, min_z, max_x, max_y and max_z
//          information respectively(equivalent to specifying a vgl_box_3d).
//
// \author  H. Can Aras
// \date    2005-03-26
// \verbatim
// Modifications
// 2005-10-13 H. Can Aras: Added splatting of a cuboid
// 2006-05-30 H. Can Aras: Added an optional parameter, which prevents the volume 
//            to be scaled between [0-65535] for writing 3D images
// \endverbatim

#include <vnl/vnl_int_2.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_vector_3d.h>
#include <xmvg/xmvg_perspective_camera.h>
#include <vnl/vnl_math.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <xscan/xscan_scan.h>
#include <xscan/xscan_uniform_orbit.h>
#include <xscan/xscan_dummy_scan.h>
#include <vbl/vbl_array_3d.h>
#include <vnl/vnl_quaternion.h>
#include <biocts/biocts_rvgs.h>
#include <vcl_ctime.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

class biosim_ctsim
{
public:
  //: default constructor, constructs a ctsim with default parameter values
  biosim_ctsim();
  //: constructor with the provided values
  biosim_ctsim(double pixel_size, double source_origin_dist, double source_sensor_distance, 
               vnl_int_2 sensor_dimensions, vnl_double_2 principal_point, 
               vgl_point_3d<double> initial_source_position, vnl_double_3 rotation_axis,
               double rotation_step_size, vcl_string noise_type);
  //: constructor from a xscan_dummy_scan object (KK requested this)
  biosim_ctsim(xscan_dummy_scan dummy_scan, vcl_string noise_type);

  //: getters
  double pix_size(){ return pix_size_; }
  double src_org_d(){ return src_org_d_; }
  double src_sns_d(){ return src_sns_d_; }
  vnl_int_2 sns_dim(){ return sns_dim_; }
  vnl_double_2 prn_pnt(){ return prn_pnt_; }
  vgl_point_3d<double> init_src_pos(){ return init_src_pos_; }
  vnl_double_3 rot_axis(){ return rot_axis_; }
  double rot_step_size(){ return rot_step_size_; }
  double num_views(){ return num_views_; }
  xmvg_perspective_camera<double> init_cam(){ return init_cam_; }
  double data(int x, int y, int z) { return data_[x][y][z]; }

  //: write the scan to a file
  void write_scan(vcl_string scanfile);

  //: construct the camera with the given index (this is not used any more,
  //  but let's wait a little before deleting it)
  xmvg_perspective_camera<double> construct_camera(int index);

  //: do the scan using the given input file
  void scan(char *fname);

  void splat_cylinder(xmvg_no_noise_filter_descriptor descriptor, 
                      xmvg_perspective_camera<double> cam, int step);

  double line_integral_cylinder(xmvg_no_noise_filter_descriptor descriptor, 
                                vgl_point_3d<double> ray_start, 
                                vgl_vector_3d<double> ray_direction, 
                                double radius);

  void splat_cuboid(vgl_box_3d<double> cuboid, xmvg_perspective_camera<double> cam, int step);

  double line_integral_cuboid(vgl_box_3d<double> cuboid, vgl_point_3d<double> ray_start, 
                              vgl_vector_3d<double> ray_direction);

  //: writes the projection data to file as a whole for using in MatLab
  //  takes a file name
  void write_data_3d(char* fname);

  //: writes the projection data as tiff images slice by slice
  //  takes a file template name, i.e. numbers will be appended to the end of 
  //  the filename for every single slice
  void write_data_2d(char* folder, char *file, bool normalize=true);

private:
  //: construct the initial camera (this is not used any more,
  //  but let's wait a little before deleting it)
  void construct_initial_camera();
  //: populate the scan member
  void set_xscan();
  //: adjust the data such that the maximum value is the pgm_max value
  void adjust_data_range(int pgm_max);
  //: scale the data up or down to have reasonable values before writing as pgm images
  void scale_data(double scale_factor);

private:
  //: sensor pixel size, assuming xy-ratio is 1, in microns
  double pix_size_;
  //: x-ray source to origin distance, in millimeters
  double src_org_d_;
  //: x-ray source to sensor distance, in millimeters
  double src_sns_d_;
  //: sensor dimensions
  vnl_int_2 sns_dim_;
  //: principal point (the point where a perpendicular line from the source
  //  to the sensor intersects the sensor plane) in sensor image coordinates
  //  assuming the origin of the sensor plane is the upper left corner
  vnl_double_2 prn_pnt_;
  //: initial x-ray source point, in world coordinates and millimeters
  vgl_point_3d<double> init_src_pos_;
  //: rotation axis
  vnl_double_3 rot_axis_;
  //: rotation step size angle in degrees (this is the rotation angle of the
  //  turning table, so we use (-) since we rotate the camera instead of the table
  double rot_step_size_;
  //: noise type
  //  no_noise : no noise
  //  from-file : use the noise file "bnoise.pgm"
  vcl_string noise_;
  //: number of views
  int num_views_;
  //: initial camera
  xmvg_perspective_camera<double> init_cam_;
  //: scan result
  vbl_array_3d<double> data_;
  //: x-ray source
  xmvg_source source_;
  //: scan object for KK
  xscan_scan scan_;
};

void increment_number_string(char *num, int length);

#endif
