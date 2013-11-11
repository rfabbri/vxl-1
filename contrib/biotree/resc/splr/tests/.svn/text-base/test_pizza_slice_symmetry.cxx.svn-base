#include <testlib/testlib_test.h>
#include <splr/splr_pizza_slice_symmetry.h>
#include <vnl/vnl_math.h>
#include <biob/biob_explicit_worldpt_roster.h>
#include <xscan/xscan_uniform_orbit.h>
#include <xscan/xscan_dummy_scan.h>
#include <vgl/vgl_homg_point_3d.h> //for testing test
#include <biob/biob_worldpt_transformation.h>

static void test_pizza_slice_symmetry(){
  //just to test my understanding of converting a quaternion to a biob_worldpt_transformation
  vnl_quaternion<double> q(vnl_double_3(0.,0.,1.), -2*vnl_math::pi/3);
  vnl_double_3x3 R(q.rotation_matrix_transpose());
  vgl_h_matrix_3d<double> vgmat(R.transpose(),vnl_double_3(0.,0.,0.));
  vcl_cout << vgmat << "\n";
  vcl_cout << "transform of (1,0,0): " << biob_worldpt_transformation(vgmat)( worldpt(1.0,0.0,0.0)) << "\n";

  biob_explicit_worldpt_roster representatives;
  double pi = vnl_math::pi;
  double angle0 = pi/4;
  double angle1 = pi/2;
  worldpt pt0(cos(angle0),sin(angle0), 2.0);
  worldpt pt1(cos(angle1), sin(angle1), 1.0);
  
  //create some representatives
  representatives.add_point(pt0);
  representatives.add_point(pt1);
  unsigned int num_views = 3;
  //create a dummy scan
  xscan_dummy_scan scan(10.0, 40., 160., vnl_int_2(256, 256),vnl_double_2(128.0, 128.0),
                  vgl_point_3d<double>(40.0,0.0,0.0), vnl_double_3(0.0,0.0,1.0),
                        2*pi/num_views, num_views);
  //Find the quaternion that takes camera 0 to camera 1
  vnl_quaternion<double> camera0_to_camera1 = dynamic_cast<xscan_uniform_orbit*>(scan.orbit().ptr())->rot().inverse();
  //create the symmetry
  TEST("quaternion taking camera 0 to camera 1", camera0_to_camera1, q);
  splr_pizza_slice_symmetry symmetry(camera0_to_camera1);

  TEST("number of points", representatives.num_points() * symmetry.size(), 2*num_views);
  /*  splr_symmetry_struct result2 = symmetry.apply(0, biob_worldpt_index(2));
  TEST("representative of pt 2", result2.representative_pti_, biob_worldpt_index(0));
  //apply transformation of result2 to the representative of point 2
  worldpt rep_of_pt2 = symmetry.representatives()->point(biob_worldpt_index(0)); //previous line should work
  worldpt derived_pt2 = result2.from_representative_(rep_of_pt2);
double rotation1 = -2*pi/3;//negative because rotation is clockwise
  worldpt true_pt2(cos(angle0+rotation1), sin(angle0+rotation1), 2.0);
  TEST_NEAR("x coord of pt 2", derived_pt2.x(), true_pt2.x(), 1e-6);
  TEST_NEAR("y coord of pt 2", derived_pt2.y(), true_pt2.y(), 1e-6);
  TEST_NEAR("z coord of pt 2", derived_pt2.z(), true_pt2.z(), 1e-6);
  splr_symmetry_struct result3 = symmetry.apply(0, biob_worldpt_index(3));
  TEST("representative of pt 3", result3.representative_pti_, biob_worldpt_index(1));
  splr_symmetry_struct result4 = symmetry.apply(0, biob_worldpt_index(4));
  TEST("representative of pt 4", result4.representative_pti_, biob_worldpt_index(0));
  worldpt derived_pt4 = result4.from_representative_(symmetry.all_points()->point(result4.representative_pti_));
  double rotation2 = -2*2*pi/3;//negative because rotation is clockwise
  worldpt true_pt4(cos(angle0+rotation2), sin(angle0+rotation2), 2.0);
  TEST_NEAR("x coord of pt 4", derived_pt4.x(), true_pt4.x(), 1e-6);
  TEST_NEAR("y coord of pt 4", derived_pt4.y(), true_pt4.y(), 1e-6);
  TEST_NEAR("z coord of pt 4", derived_pt4.z(), true_pt4.z(), 1e-6);
  */
}

TESTMAIN(test_pizza_slice_symmetry);
