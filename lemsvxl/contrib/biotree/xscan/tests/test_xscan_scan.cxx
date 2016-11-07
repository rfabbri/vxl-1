//: 
// \file    test_xscan_scan.h
// \brief   testing the xscan_scan class
// \author  Kongbin Kang and H. Can Aras
// \date    2005-03-03
// 
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <testlib/testlib_test.h>
#include <xscan/xscan_uniform_orbit.h>
#include <xscan/xscan_scan.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3x3.h>

static void test_xscan_scan()
{
  xscan_scan scan;
  
  // number of views
  scan.set_scan_size(180);


  // image file path
  scan.set_image_file_path("dummy");

  // source
  xmvg_source source;
  scan.set_source(source);

  // calibration matrix

  vnl_double_3x3 m(0.0);

  double pix_size = 1;

  m[0][0] = 1000 / pix_size;

  m[0][1] = 0;

  m[0][2] = 1;

  m[1][1] = 1000 / pix_size;

  m[1][2] = 1;

  m[2][2] = 1;

  vpgl_calibration_matrix<double> K(m);

  scan.set_calibration_matrix(K);

  // orbit
  vnl_double_3 rot_axis(0, 0, 1);
  
  vnl_quaternion<double> rot(rot_axis, -2 * vnl_math::pi/180);

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

  vnl_double_3 ttemp(0, -40, 0);

  vnl_double_3 t0(- R0 * ttemp);

  xscan_orbit_base_sptr orbit = new xscan_uniform_orbit(rot, t,  r0, t0);

  scan.set_orbit(orbit);

  vgl_point_3d<double> pt = scan(0).camera_center();
  TEST_NEAR("x of camera center at tick 0", pt.x(), 0, 1e-6);
  TEST_NEAR("y of camera center at tick 0", pt.y(), -40, 1e-6);
  TEST_NEAR("z of camera center at tick 0", pt.z(), 0, 1e-6);

  pt = scan(45).camera_center();
  TEST_NEAR("x of camera center at tick 45", pt.x(), 40, 1e-6);
  TEST_NEAR("y of camera center at tick 45", pt.y(), 0, 1e-6);
  TEST_NEAR("z of camera center at tick 45", pt.z(), 0, 1e-6);

  pt = scan(90).camera_center();
  TEST_NEAR("x of camera center at tick 90", pt.x(), 0, 1e-6);
  TEST_NEAR("y of camera center at tick 90", pt.y(), 40, 1e-6);
  TEST_NEAR("z of camera center at tick 90", pt.z(), 0, 1e-6);

  // test stream
  vcl_ofstream ofile("test_scan.txt");
  ofile << scan;
  ofile.close();

  xscan_scan scan_from_file;
  vcl_ifstream ifile("test_scan.txt"); 
  ifile >> scan_from_file;
  vcl_cout << "Tests for input-output" << vcl_endl;
  TEST("number of views", scan.n_views(), scan_from_file.n_views());
  TEST("image file path", scan.image_file_path(), scan_from_file.image_file_path());
  TEST("calibration matrix", scan.kk().get_matrix(), scan_from_file.kk().get_matrix());

  vcl_cout << scan << vcl_endl;
  vcl_cout << scan_from_file << vcl_endl;
}

TESTMAIN(test_xscan_scan)

