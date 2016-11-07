//: 
// \file    test_xscan_uniform_orbit.h
// \brief   testing the xscan_uniform_orbit class
// \author  Kongbin Kang and H. Can Aras
// \date    2005-03-03
// 
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_ios.h>
#include <testlib/testlib_test.h>
#include <xscan/xscan_uniform_orbit.h>
#include <vnl/vnl_math.h>

static void test_xscan_uniform_orbit()
{
  // rotation axis and angle
  vnl_double_3 axis(0,0,1);
  
  vnl_double_3 t(0,0,0); // pure rotation
  
  vnl_quaternion<double> rot(axis, 0.6*vnl_math::pi/180);

  // rotation and translation to the first camera
  vnl_double_3 axis1(0,0,1);
  vnl_quaternion<double> r0(axis1, 0);

  vnl_double_3 t0(18, 0, 0);

  // set up the orbit 
  xscan_uniform_orbit orbit(rot, t, r0, t0);

  vcl_cout << orbit;

  vnl_double_4x4 trans = orbit.global_transform(1).get_matrix();

  // test stream to file
  vcl_string filename("test_uniform_orbit.txt");
  vcl_ofstream ofile(filename.c_str());
  ofile << orbit;
  ofile.close();

  xscan_uniform_orbit orbit_from_file;
  vcl_ifstream ifile(filename.c_str());
  ifile >> orbit_from_file;
  ifile.close();

  TEST_NEAR("rotation quaternion test-0", orbit.rot().get(0), orbit_from_file.rot().get(0),1e-10);
  TEST_NEAR("rotation quaternion test-1", orbit.rot().get(1), orbit_from_file.rot().get(1),1e-10);
  TEST_NEAR("rotation quaternion test-2", orbit.rot().get(2), orbit_from_file.rot().get(2),1e-10);
  TEST_NEAR("rotation quaternion test-3", orbit.rot().get(3), orbit_from_file.rot().get(3),1e-10);
  TEST_NEAR("pitch test-0", orbit.t().get(0), orbit_from_file.t().get(0), 1e-10);
  TEST_NEAR("pitch test-1", orbit.t().get(1), orbit_from_file.t().get(1), 1e-10);
  TEST_NEAR("pitch test-2", orbit.t().get(2), orbit_from_file.t().get(2), 1e-10);
  TEST_NEAR("rotation quaternion to the first camera test-0", orbit.r0().get(0), orbit_from_file.r0().get(0),1e-10);
  TEST_NEAR("rotation quaternion to the first camera test-1", orbit.r0().get(1), orbit_from_file.r0().get(1),1e-10);
  TEST_NEAR("rotation quaternion to the first camera test-2", orbit.r0().get(2), orbit_from_file.r0().get(2),1e-10);
  TEST_NEAR("rotation quaternion to the first camera test-3", orbit.r0().get(3), orbit_from_file.r0().get(3),1e-10);
  TEST_NEAR("translation to the first camera test-0", orbit.t0().get(0), orbit_from_file.t0().get(0), 1e-10);
  TEST_NEAR("translation to the first camera test-1", orbit.t0().get(1), orbit_from_file.t0().get(1), 1e-10);
  TEST_NEAR("translation to the first camera test-2", orbit.t0().get(2), orbit_from_file.t0().get(2), 1e-10);
}

TESTMAIN( test_xscan_uniform_orbit )



