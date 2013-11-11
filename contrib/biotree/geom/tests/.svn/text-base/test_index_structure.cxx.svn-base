#include <testlib/testlib_test.h>
#include <biob/biob_worldpt_roster_sptr.h>
#include <geom/geom_index_structure.h>
#include <biob/biob_explicit_worldpt_roster.h>
#include "dummy_probe_volume1.h"
#include <geom/geom_rectangular_probe_volume.h>

static void test_index_structure(){
  biob_explicit_worldpt_roster * roster = new biob_explicit_worldpt_roster();
  roster->add_point(worldpt(.5, .5, .5));
  geom_index_structure index_structure(roster, 1.);
  biob_worldpt_index_enumerator_sptr sptr = index_structure.enclosed_by(new dummy_probe_volume1());
  bool flag = sptr->has_next();
  TEST("found point with dummy probe volume 1", flag, true);
  if (flag){
    biob_worldpt_index pti = sptr->next();
    TEST("index of point found with dummy probe volume 1", pti.index(), 0);
    TEST("no more points with dummy probe volume 1", sptr->has_next(), false);
  }
  biob_worldpt_index_enumerator_sptr sptr2
    = index_structure.enclosed_by(new geom_rectangular_probe_volume(vgl_box_3d<double>(.3, .3, .3, .6, .6, .6)));
  bool flag2 = sptr2->has_next();
  TEST("found point with rectangular probe volume containing point", flag2, true);
  if (flag2){
    biob_worldpt_index pti = sptr2->next();
    TEST("index of point found with rectangular probe volume containing point", pti.index(), 0);
    TEST("no more points with rectangular probe volume containing point", sptr2->has_next(), false);
  }
  biob_worldpt_index_enumerator_sptr sptr3
    = index_structure.enclosed_by(new geom_rectangular_probe_volume(vgl_box_3d<double>(.55, .55, .55, .6, .6, .6)));
  bool flag3 = sptr3->has_next();
  TEST("did not find point with rectangular probe volume not containing point", flag3, false);
}

TESTMAIN(test_index_structure);
