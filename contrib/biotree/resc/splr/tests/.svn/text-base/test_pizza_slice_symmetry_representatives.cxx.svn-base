#include <testlib/testlib_test.h>
#include <splr/splr_pizza_slice_symmetry_representatives.h>
#include <biob/biob_explicit_worldpt_roster.h>

static void test_pizza_slice_symmetry_representatives(){
  biob_explicit_worldpt_roster roster;
  worldpt maxpt(1,1,.9), minpt(-2,-4,1.05);
  biob_worldpt_box box(minpt, maxpt);
  double spacing = 0.1;
  unsigned int num_slices = 4;
  splr_pizza_slice_symmetry_representatives(&roster, num_slices, box, spacing);
}

TESTMAIN(test_pizza_slice_symmetry_representatives);

