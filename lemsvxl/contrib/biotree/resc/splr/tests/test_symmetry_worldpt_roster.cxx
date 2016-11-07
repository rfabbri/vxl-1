#include <testlib/testlib_test.h>
#include <splr/splr_pizza_slice_symmetry.h>
#include <splr/splr_symmetry_worldpt_roster.h>
#include <biob/biob_explicit_worldpt_roster_sptr.h>
#include <xscan/xscan_dummy_scan.h>

static void test_symmetry_worldpt_roster(){
  //make a symmetry object
  biob_explicit_worldpt_roster_sptr representatives = new biob_explicit_worldpt_roster();
  double pi = vnl_math::pi;
  double angle0 = pi/4;
  double angle1 = pi/2;
  double angle_increment = -2*pi/3;//for dummy_scan, moving from camera0 to camera1 involves clockwise rotation
  worldpt pts[6];
  pts[0] = worldpt(cos(angle0),sin(angle0), 2.0);
  pts[1] = worldpt(cos(angle1), sin(angle1), 1.0);
  pts[2] = worldpt(cos(angle_increment+angle0), sin(angle_increment+angle0), 2.0);
  pts[3] = worldpt(cos(angle_increment+angle1), sin(angle_increment+angle1), 1.0);
  pts[4] = worldpt(cos(2*angle_increment+angle0), sin(2*angle_increment+angle0), 2.0);
  pts[5] = worldpt(cos(2*angle_increment+angle1), sin(2*angle_increment+angle1), 1.0);
  
  biob_worldpt_index pti0 = representatives->add_point(pts[0]);
  biob_worldpt_index pti1 = representatives->add_point(pts[1]);
  unsigned int num_views = 3;
  //create a dummy scan
  xscan_dummy_scan scan(10.0, 40., 160., vnl_int_2(256, 256),vnl_double_2(128.0, 128.0),
                  vgl_point_3d<double>(40.0,0.0,0.0), vnl_double_3(0.0,0.0,1.0),
                  2*pi/num_views, num_views);
//Find the quaternion that takes camera 0 to camera 1
  vnl_quaternion<double> camera0_to_camera1 = dynamic_cast<xscan_uniform_orbit*>(scan.orbit().ptr())->rot().inverse();
  //create the symmetry
  splr_pizza_slice_symmetry symmetry(camera0_to_camera1);
  //make a symmetry_worldpt_roster
  splr_symmetry_worldpt_roster roster(&symmetry, *(reinterpret_cast<biob_worldpt_roster_sptr*>(&representatives)));
  TEST("number of points", roster.num_points(), 2*num_views);

  for (int i = 0; i < 6; ++i){
    TEST_NEAR("x coordinate", pts[i].x(), roster.point(biob_worldpt_index(i)).x(), 1e-6);
    TEST_NEAR("y coordinate", pts[i].y(), roster.point(biob_worldpt_index(i)).y(), 1e-6);
    TEST_NEAR("z coordinate", pts[i].z(), roster.point(biob_worldpt_index(i)).z(), 1e-6);
  }
}

TESTMAIN(test_symmetry_worldpt_roster);
