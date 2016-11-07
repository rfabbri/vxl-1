//: 
// \file  test_xscan_dummy_scan.h
// \brief  testing the xscan_dummy_scan class
// \author    Kongbin Kang
// \date        2005-03-03
// 
#include <testlib/testlib_test.h>
#include <xscan/xscan_dummy_scan.h>

static void test_xscan_dummy_scan()
{
  xscan_dummy_scan scan;

#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  int nviews = scan.scan_size();
#endif
  
  vgl_point_3d<double> pt = scan(0).camera_center();
  TEST_NEAR("x of camera center at tick 0", pt.x(), 0, 1e-6);
  TEST_NEAR("y of camera center at tick 0", pt.y(), -40, 1e-6);
  TEST_NEAR("z of camera center at tick 0", pt.z(), 0, 1e-6);

  pt = scan(90).camera_center();
  TEST_NEAR("x of camera center at tick 90", pt.x(), 40, 1e-6);
  TEST_NEAR("y of camera center at tick 90", pt.y(), 0, 1e-6);
  TEST_NEAR("z of camera center at tick 90", pt.z(), 0, 1e-6);

  pt = scan(180).camera_center();
  TEST_NEAR("x of camera center at tick 180", pt.x(), 0, 1e-6);
  TEST_NEAR("y of camera center at tick 180", pt.y(), 40, 1e-6);
  TEST_NEAR("z of camera center at tick 180", pt.z(), 0, 1e-6);

}

TESTMAIN(test_xscan_dummy_scan)

