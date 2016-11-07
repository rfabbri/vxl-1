#include "xscan_dummy_scan.h"
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <xscan/xscan_uniform_orbit.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3x3.h>

xscan_dummy_scan::xscan_dummy_scan(
    double pixel_size ,
    double src_org_dist ,
    double src_sns_dist,
    vnl_int_2 const & sensor_dim ,
    vnl_double_2 const& principle_pt ,
    vgl_point_3d<double> const& init_src_pos,
    vnl_double_3 const& rot_axis,
    double rot_step,
    unsigned nviews,
    char const * file_path) 
: pixel_size_(pixel_size), source_origin_dist_(src_org_dist), source_sensor_dist_(src_sns_dist),
  sensor_dimensions_(sensor_dim), principal_point_(principle_pt),
  initial_source_position_(init_src_pos), rotation_axis_(rot_axis), rotation_step_angle_(rot_step)
  
{
  image_file_path_ = file_path;
  n_views_ = nviews;
  // source
  xmvg_source source;
  source_ = source;

  // calibration matrix

  vnl_double_3x3 m(0.0);

  m[0][0] = source_sensor_dist_*1000 / pixel_size;

  m[0][1] = 0;

  m[0][2] = principle_pt[0];

  m[1][1] = source_sensor_dist_*1000 / pixel_size_;

  m[1][2] = principle_pt[1];

  m[2][2] = 1;

  vpgl_calibration_matrix<double> K(m);

  kk_ = K;

  // orbit
  vnl_quaternion<double> rot(rot_axis, -rot_step);

  // translation step is zero for now (circular)

  vnl_double_3 t(0.0, 0.0, 0.0);

  // the initial camera assumed to sit on x-axis and look towards

  // the negative x-axis for now

  vnl_double_3x3 R0(0.0);

//  R0[0][1] = 1;
//  R0[1][2] = -1;
//  R0[2][0] = -1;

  R0[0][0] = 1;
  R0[1][2] = -1;
  R0[2][1] = 1;

  vnl_quaternion<double> r0(R0);

  vnl_double_3 center(init_src_pos.x(), init_src_pos.y(), init_src_pos.z());

  vnl_double_3 t0(- R0 * center);

  xscan_orbit_base_sptr orbit = new xscan_uniform_orbit(rot, t,  r0, t0);

  orbit_ = orbit;
} 
