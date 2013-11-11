#include <testlib/testlib_test.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <bio_defs.h>

static void test_grid_worldpt_roster()
{
   biob_worldpt_box box(worldpt(0,0,0), worldpt(1,2,4));
   
   biob_grid_worldpt_roster grid_roster(box, .49);
   
   TEST_NEAR("num points", grid_roster.num_points(), 64, 0);
   
   biob_worldpt_index last_pti(grid_roster.num_points() - 1);
   
   TEST_NEAR("last point x", grid_roster.point(last_pti).x(), 1, .11);
   
   TEST_NEAR("last point y", grid_roster.point(last_pti).y(), 2, .22);
   
   TEST_NEAR("last point z", grid_roster.point(grid_roster.last_pti).z(), 4, .44);
   
   biob_worldpt_index middle_pti(32);

   TEST_NEAR("middle point x", grid_roster.point(middle_pti).x(), .5, .11);
   
   TEST_NEAR("middle point y", grid_roster.point(middle_pti).y(), 0, 0);
   
   TEST_NEAR("middle point z", grid_roster.point(middle_pti).z(), 0, 0);
}

TESTMAIN(test_grid_worldpt_roster);



   
 
