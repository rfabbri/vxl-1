//: 
// \file  test_bioproc_dummy_splat_resource.h
// \brief  testing the bioproc_dummy_splat_resource class
// \author    Kongbin Kang
// \date        2005-03-03
// 
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <testlib/testlib_test.h>
#include <xscan/xscan_dummy_scan.h>
#include <xmvg/xmvg_no_noise_filter_3d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <proc/bioproc_dummy_splat_resource.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3x3.h>

static void test_bioproc_dummy_splat_resource(int argc, char *argv[])
{
  //  a orbit
  // scan
  xscan_dummy_scan scan(10, 40, 160, vnl_int_2(200, 200), vnl_double_2(100, 100),
      vgl_point_3d<double>(0.0, -40.0, 0.0), vnl_double_3(.0, .0, 1.), 
      2*vnl_math::pi/180, 5, argv[1]);

  xscan_scan const & scan_ref = dynamic_cast<xscan_scan const&>(scan);

  // box
  double xmin, ymin, zmin, xmax, ymax, zmax;

  xmin = ymin = zmin = -0.1;
  xmax = ymax = zmax = 0.1;

#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  vgl_box_3d<double> box(xmin, ymin, zmin, xmax, ymax, zmax);

  // resolution
  double resolution = 0.025; // 25 micro
#endif

  // filter 3d
  double f_radius = 0.0125;
  double f_length = 0.1;
  vgl_point_3d<double> f_centre(0.0, 0.0, 0.0);
  vgl_vector_3d<double> f_orientation_z(0.0, 0.0, 1.0);
  vgl_vector_3d<double> f_orientation_y(0.0, 1.0, 0.0);
  vgl_vector_3d<double> f_orientation_x(1.0, 0.0, 0.0);
  xmvg_no_noise_filter_descriptor fdx(f_radius, f_length, f_centre, f_orientation_x);
  xmvg_no_noise_filter_descriptor fdy(f_radius, f_length, f_centre, f_orientation_y);
  xmvg_no_noise_filter_descriptor fdz(f_radius, f_length, f_centre, f_orientation_z);
  // construct the filters
  xmvg_no_noise_filter_3d fx(fdx);
  xmvg_no_noise_filter_3d fy(fdy);
  xmvg_no_noise_filter_3d fz(fdz);

  vcl_vector<xmvg_no_noise_filter_3d> filters;
  filters.push_back(fx);
  filters.push_back(fy);
  filters.push_back(fz);

  xmvg_composite_filter_3d<double, xmvg_no_noise_filter_3d> comp3d(filters);

  bioproc_dummy_splat_resource<double, xmvg_no_noise_filter_3d> splr(scan_ref, comp3d);

  vcl_cout << splr.splat(0, vgl_point_3d<double>(0, 0, 0)). atomic_filter(2).weights();

}

TESTMAIN_ARGS(test_bioproc_dummy_splat_resource)

