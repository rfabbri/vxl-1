#include <testlib/testlib_test.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <splr/splr_explicit_splat_collection.txx>
#include <splr/tests/dummy_filter_2d.h>
#include <splr/tests/dummy_filter_3d.h>
#include <splr/tests/dummy_camera.h>
#include <splr/tests/dummy_scan.h>

static void test_explicit_splat_collection(){

  //dummy filter 3d
  dummy_filter_3d filter_3d;
  //dummy scan
  dummy_scan scan(1);
  //roster
   biob_worldpt_box box(worldpt(0,0,0), worldpt(1,2,4));
   //   biob_grid_worldpt_roster roster(box, .49);

  biob_worldpt_roster_sptr roster = new biob_grid_worldpt_roster(box, .49);
  splr_explicit_splat_collection<double, int, 
    dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera> splat_collection(scan, 1, &filter_3d, roster);

  //populate the splat collection
  splat_collection.populate(biob_worldpt_index(0));

  /*  // write to a file
  vsl_b_ofstream  outfs("test_file");
  vsl_b_write(outfs, splat_collection);
  outfs.close();
  // read from the file
  splr_explicit_splat_collection<double, int, 
    dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera> splat_collection2(scan, 1, &filter_3d, roster);
  
  vsl_b_ifstream infs("test_file");
  vsl_b_read(infs, splat_collection2);
  */

  //get the camera from the scan
  dummy_camera camera = scan(0);
  //see if splat obtained from camera is the same as that obtained from the splat_collection
  TEST("splat from camera == splat from splat_collection", 
       splat_collection.splat(0, biob_worldpt_index(0)) == filter_3d.splat(camera, roster->point(biob_worldpt_index(0))), true);
  //    TEST("splat from original splat collection == splat from splat collection from file",
  //      splat_collection.splat(0, biob_worldpt_index(0)) ==  splat_collection2.splat(0, biob_worldpt_index(0)), true);

  //try enumerator
  biob_subset_worldpt_roster::which_points_t which_points(2);
  which_points[0] = biob_worldpt_index(0);
  which_points[1] = biob_worldpt_index(1);
  biob_worldpt_index_enumerator_sptr enumer = splat_collection.enumerator(which_points);
  TEST("enumerator has a point", enumer->has_next(), true);
  TEST("enumerator has point 0", enumer->next().index(), 0);
  TEST("enumerator has a point", enumer->has_next(), true);
  TEST("enumerator has point 1", enumer->next().index(), 1);
  TEST("enumerator has no point", enumer->has_next(), false);
}

TESTMAIN(test_explicit_splat_collection);
