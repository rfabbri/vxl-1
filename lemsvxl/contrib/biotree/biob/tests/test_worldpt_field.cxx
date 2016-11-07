#include <testlib/testlib_test.h>
#include <biob/biob_worldpt_field.h>
#include <biob/biob_grid_worldpt_roster.h>

static void test_worldpt_field(){
  biob_worldpt_box box(1.0, 1.0, 1.0, 2.0, 2.0, 2.0);
  biob_grid_worldpt_roster roster(box, .1);
  biob_worldpt_roster_sptr p = &roster;
  biob_worldpt_field<double> field(p);
  field.values().resize(10);
}

TESTMAIN(test_worldpt_field);
