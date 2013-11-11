#ifndef xscan_dummy_scan_h_
#define xscan_dummy_scan_h_

//: 
// \file  xscan_dummy_scan.h
// \brief  a duumy scan based on a real scan plan 
// \author    Kongbin Kang
// \date        2005-03-03
// 
#include <xscan/xscan_scan.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_int_2.h>

class xscan_dummy_scan : public xscan_scan
{
  public:

    //: full constructor with default value
    xscan_dummy_scan(
        double pixel_size = 10,
        double src_org_dist = 40.0,
        double src_sns_dist = 160.0,
        vnl_int_2 const & sensor_dim= vnl_int_2(256, 256),
        vnl_double_2 const& principle_pt = vnl_double_2(128.0, 128.0),
        vgl_point_3d<double> const& init_src_pos = vgl_point_3d<double>(0.0,-40.0,0.0),
        vnl_double_3 const& rot_axis = vnl_double_3(0.0,0.0,1.0),
        double rot_step = 0.0174532925199433, // 1 degrees in radius
        unsigned nview = 360,
        char const * file_path = "dummy");
    //: getter functions
    double pixel_size() { return pixel_size_; }
    double source_origin_dist() { return source_origin_dist_; }
    double source_sensor_dist() { return source_sensor_dist_; }
    vnl_int_2 sensor_dimensions() { return sensor_dimensions_; }
    vnl_double_2 principal_point() { return principal_point_; }
    vgl_point_3d<double> initial_source_position() { return initial_source_position_; }
    vnl_double_3 rotation_axis() { return rotation_axis_; }
    double rotation_step_angle() { return rotation_step_angle_; }

  private:

    double pixel_size_;
    
    double source_origin_dist_;

    double source_sensor_dist_;
    
    vnl_int_2 sensor_dimensions_;
    
    vnl_double_2 principal_point_;
    
    vgl_point_3d<double> initial_source_position_;

    vnl_double_3 rotation_axis_;

    double rotation_step_angle_;
    
    
};

#endif

