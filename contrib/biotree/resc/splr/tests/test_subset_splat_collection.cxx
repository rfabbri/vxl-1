#include <testlib/testlib_test.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <splr/splr_explicit_splat_collection.txx>
#include <splr/splr_subset_splat_collection.txx>
#include <splr/tests/dummy_filter_2d.h>
#include <splr/tests/dummy_filter_3d.h>
#include <splr/tests/dummy_camera.h>
#include <splr/tests/dummy_scan.h>

static void test_subset_splat_collection(){

  //dummy filter 3d
  dummy_filter_3d filter_3d;
  //dummy scan
  dummy_scan scan(1);
  //roster
   biob_worldpt_box box(worldpt(0,0,0), worldpt(1,2,4));
   //   biob_grid_worldpt_roster roster(box, .49);
  biob_worldpt_roster_sptr roster = new biob_grid_worldpt_roster(box, .49);
  splr_explicit_splat_collection<double, int, dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera> explicit_splat_collection(scan, 1, &filter_3d, roster);
  splr_subset_splat_collection<double, dummy_filter_2d> subset_splat_collection(&explicit_splat_collection, roster);
  biob_subset_worldpt_roster::which_points_t & subset_which_points = subset_splat_collection.subset_roster()->which_points();
  subset_which_points.push_back(biob_worldpt_index(10));
  subset_which_points.push_back(biob_worldpt_index(25));
  
  //try enumerator
  biob_worldpt_index_enumerator_sptr enumer = subset_splat_collection.enumerator();
  enumer->has_next();
  TEST("enumerator has a point", enumer->has_next(), true);
  biob_worldpt_index nextA = enumer->next();
  TEST("enumerator has a point", enumer->has_next(), true);
  biob_worldpt_index nextB = enumer->next();
  TEST("first point is 0 or 1", nextA.index() == 0 || nextA.index() == 1, true);
  TEST("second point is 0 or 1", nextB.index() == 0 || nextB.index() == 1, true);
  TEST("points are different", nextA.index() != nextB.index(), true);
  TEST("enumerator has no point", enumer->has_next(), false);
}

TESTMAIN(test_subset_splat_collection);
