#include <testlib/testlib_test.h>
#include <biob/biob_explicit_worldpt_roster.h>

static void test_explicit_worldpt_roster(){
    splr_explicit_worldpt_roster roster;
    biob_worldpt_index pti0 = roster.add_point(worldpt(1.0,2.0,3.0));//called for effect
    biob_worldpt_index pti1 = roster.add_point(worldpt(1.1,2.1,3.1));//called for effect
    biob_worldpt_index pti2 = roster.add_point(worldpt(1.2,2.2,3.3));//called for effect
    TEST("number of points", roster.num_points(), 3);
    TEST("point access", roster.point(biob_worldpt_index(2)), worldpt(1.2,2.2,3.3));

  //create a file output stream
  vsl_b_ofstream outfs("test-output-file");
//write the roster
  vsl_b_write(outfs, roster);
  //close the file
  outfs.close();
  //create a new, empty roster
  splr_explicit_worldpt_roster roster2;
  //open the file for reading
  vsl_b_ifstream infs("test-output-file");
  //read the data into the splat collection
  vsl_b_read(infs, roster2);
  //compare roster and roster2
  TEST("saved roster == original roster", roster == roster2, 1);
}

TESTMAIN(test_explicit_worldpt_roster);
