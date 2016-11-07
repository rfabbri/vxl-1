// This is /contrib/biotree/biocbs/biocbs_scan.h

#ifndef biocbs_scan_h_
#define biocbs_scan_h_

//: 
// \file    biocbs_scan.h
// \brief   Cone-Beam CT Simulator
//          Lower left corner 
//
// \author  H. Can Aras
// \date    2006-07-26
// \verbatim
// Modifications

// \endverbatim

#include <vcl_string.h>
#include <xscan/xscan_dummy_scan.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_int_3.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_box_3d.h>

enum {NO_INTERP, LINEAR_INTERP};

class biocbs_scan
{
public:
  //: constructor from full parameter list, check the units
  //  of the variables in this class for the units of the
  //  constructor parameters
  biocbs_scan(double sensor_pix_size,
    double source_origin_dist, double source_sensor_dist,
    vnl_int_2 sensor_dim, vnl_double_2 principal_point,
    vgl_point_3d<double> init_sorce_pos, 
    vnl_double_3 rot_axis, double rot_step_angle, 
    int number_of_views, 
    double grid_resolution,
    vbl_array_3d<unsigned short> *vol, 
    vgl_point_3d<double> vol_center_pos,
    int interp_type);

  //: do projection for a single view
  void project_volume(int viewno);
  //: convert from grid coordinates to world coordinates
  vgl_point_3d<double> grid_to_world(vnl_double_3 gp);
  //: convert from world coordinates to grid coordinates
  vnl_double_3 world_to_grid(vgl_point_3d<double> wp);
  //: find the intersection of the line and the box
  //  return true and the points if they intersect, false otherwise
  bool intersect_line_and_box(vgl_box_3d<double> box, 
                              vgl_point_3d<double> ray_start, 
                              vgl_vector_3d<double> ray_direction, 
                              vgl_point_3d <double> &points);
  //: compute the 3D box in world coordinates for the given volume
  void compute_3D_box_of_interest();
  //: compute discrete line integral
  double discrete_line_integral(vgl_point_3d <double> point,
                                vgl_vector_3d<double> ray_direction);
  double interpolate_subvoxel_value(vgl_point_3d<double> subvox);

  void adjust_data_range(double adjust_val);
  void write_data_3d(vcl_string fname);
  void write_data_2d_views(vcl_string fnamebase);
  void write_scan(vcl_string scanfile);
  void write_box(vcl_string boxfile);
  void write_view(vbl_array_2d<double> view);

  //: getters
  xscan_scan scan()
  { return scan_; };

  xmvg_perspective_camera<double> camera(int t) 
  { return  cameras_[t]; };
  
  double sensor_pixel_size()
  { return sensor_pix_size_; };

  double source_origin_dist()
  { return source_origin_dist_; };

  double source_sensor_dist()
  { return source_sensor_dist_; };

  vnl_int_2 sensor_dim()
  { return sensor_dim_; };

  vnl_double_2 principal_point()
  { return principal_point_; };

  vgl_point_3d<double> init_sorce_pos()
  { return init_sorce_pos_; };

  vnl_double_3 rot_axis()
  { return rot_axis_; };

  double rot_step_angle()
  { return rot_step_angle_; };

  int number_of_views()
  { return number_of_views_; };

  double grid_resolution()
  { return grid_resolution_; };

  vgl_point_3d<double> vol_center_pos()
  { return vol_center_pos_; };

  unsigned short vol_val(int i, int j, int k)
  { return (*vol_)(i,j,k); };

  vnl_int_3 vol_dim()
  { return vol_dim_; };

  vnl_int_3 vol_center()
  { return vol_center_; };

  vgl_box_3d<double> box()
  { return box_; };

protected:
  //: unit in microns
  double sensor_pix_size_;
  //: unit in millimeters
  double source_origin_dist_; 
  //: unit in millimeters
  double source_sensor_dist_;
  //: unit in sensor pixels
  vnl_int_2 sensor_dim_;
  //: unit in sensor pixels
  vnl_double_2 principal_point_;
  //: unit in millimeters
  vgl_point_3d<double> init_sorce_pos_;
  //: no unit
  vnl_double_3 rot_axis_;
  //: unit in radians
  double rot_step_angle_;
  //: no unit
  int number_of_views_;
  //: in microns
  double grid_resolution_;
  //: in millimeters
  vgl_point_3d<double> vol_center_pos_;
  //: volume data
  vbl_array_3d<unsigned short> *vol_;
  //: volume dimensions
  vnl_int_3 vol_dim_;
  //: volume centre voxel in number of voxel units
  vnl_int_3 vol_center_;
  //: interpolation type
  int interp_type_;

  vgl_box_3d<double> box_;
  xscan_dummy_scan scan_;
  vcl_vector <xmvg_perspective_camera<double> > cameras_;
  //: maximum recommended sensor dimensions are 512x512 with 512 view images
  //  each view image is about 2MB.
  vbl_array_3d<double> proj_data_;
};

#endif
