#include <testlib/testlib_test.h>
#include <splr/splr_pizza_slice_symmetry.h>
#include <splr/splr_symmetry_worldpt_roster.h>
#include <biob/biob_explicit_worldpt_roster.h>
#include <splr/splr_explicit_splat_collection.h>
#include <splr/tests/dummy_filter_2d.h>
#include <splr/tests/dummy_filter_3d.h>
#include <splr/tests/dummy_camera.h>
#include <splr/tests/dummy_scan.h>
#include <splr/splr_symmetry_splat_collection.h>
#include <xscan/xscan_uniform_orbit.h>
#include <xscan/xscan_dummy_scan.h>


static void test_symmetry_splat_collection(){
  //make a symmetry object
  biob_explicit_worldpt_roster_sptr representatives = new biob_explicit_worldpt_roster();
  double pi = vnl_math::pi;
  double angle0 = pi/4;
  double angle1 = pi/2;
  worldpt pts[6];
  pts[0] = worldpt(cos(angle0),sin(angle0), 2.0);
  pts[1] = worldpt(cos(angle1), sin(angle1), 1.0);
  pts[2] = worldpt(cos(2*pi/3+angle0), sin(2*pi/3+angle0), 2.0);
  pts[3] = worldpt(cos(2*pi/3+angle1), sin(2*pi/3+angle1), 1.0);
  pts[4] = worldpt(cos(4*pi/3+angle0), sin(4*pi/3+angle0), 2.0);
  pts[5] = worldpt(cos(4*pi/3+angle1), sin(4*pi/3+angle1), 1.0);
  
  biob_worldpt_index pti0 = representatives->add_point(pts[0]);
  biob_worldpt_index pti1 = representatives->add_point(pts[1]);
  unsigned int num_views = 3;
  //create a dummy scan
  xscan_dummy_scan xscan_scan(10.0, 40., 160., vnl_int_2(256, 256),vnl_double_2(128.0, 128.0),
                  vgl_point_3d<double>(40.0,0.0,0.0), vnl_double_3(0.0,0.0,1.0),
                  2*pi/num_views, num_views);
//Find the quaternion that takes camera 0 to camera 1
  vnl_quaternion<double> camera0_to_camera1 = dynamic_cast<xscan_uniform_orbit*>(xscan_scan.orbit().ptr())->rot().inverse();
  //create the symmetry
  splr_pizza_slice_symmetry symmetry(camera0_to_camera1);

  //construct dummy filter_3d and scan
  //dummy filter 3d
  dummy_filter_3d filter_3d;
  //dummy scan
  dummy_scan scan(3);


  //construct an explicit_splat_collection
    splr_explicit_splat_collection<double, int, 
      dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera> sub_splat_collection(scan, 3, &filter_3d, *(reinterpret_cast<biob_worldpt_roster_sptr*>(&representatives)));

  //construct the symmetry_splat_collection
    splr_symmetry_splat_collection<double, dummy_filter_2d> splat_collection(&symmetry, &sub_splat_collection, representatives->num_points());

  biob_subset_worldpt_roster::which_points_t which_points;
  which_points.push_back(biob_worldpt_index(0));
  which_points.push_back(biob_worldpt_index(1));
  which_points.push_back(biob_worldpt_index(2));
  which_points.push_back(biob_worldpt_index(3));
  which_points.push_back(biob_worldpt_index(4));
  which_points.push_back(biob_worldpt_index(5));


  dummy_filter_2d splats[3][6];
  biob_worldpt_index_enumerator_sptr enumer = splat_collection.enumerator(which_points);
  while (enumer->has_next()){
    biob_worldpt_index pti = enumer->next();
    for (int i = 0; i < 3; ++i){
      splats[i][pti.index()] = splat_collection.splat(i, pti);
    }
  }

  TEST("splat of pts[0] onto t=1 equals splat of pts[2] onto t=2",
       splats[1][0],
       splats[2][2]);
  TEST("splat of pts[1] onto t=0 equals splat of pts[3] onto t=1",
       splats[0][1],
       splats[1][3]);
  TEST("splat of pts[0] onto t=2 equals splat of pts[4] onto t=1",
       splats[2][0],

       splats[1][4]);

#if 0 //binary io removed
  //test io
  // write to a file
  vsl_b_ofstream  outfs("test_file");
  vsl_b_write(outfs, splat_collection);
  outfs.close();
  //construct another explicit_splat_collection
    splr_explicit_splat_collection<double, int, 
    dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera> sub_splat_collection2(scan, 3, &filter_3d, &representatives);

  //construct another symmetry_splat_collection
  splr_symmetry_splat_collection<double, dummy_filter_2d> splat_collection2(&symmetry, &sub_splat_collection2);

  // read from the file
  vsl_b_ifstream infs("test_file");
  vsl_b_read(infs, splat_collection2);
  TEST("splat from original == splat from file",
       splat_collection.splat(1, biob_worldpt_index(3)),
       splat_collection2.splat(1, biob_worldpt_index(3)));
#endif
}

TESTMAIN(test_symmetry_splat_collection);
