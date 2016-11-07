#include <testlib/testlib_test.h>
#include <biob/biob_explicit_neighborhood_structure.h>
#include <biob/biob_explicit_worldpt_roster.h>

static void test_explicit_neighborhood_structure(){
  biob_explicit_worldpt_roster roster;
  for (double z = 0.0; z <= .4001; z += 0.2){
    for (double y = 0.0; y <= .3001; y += 0.1){
      for (double x = 0.0; x <= .3001; x += 0.1){
        roster.add_point(worldpt(x,y,z));
      }
    }
  }

  /*
      03  07  11  15
          
      02  06  10  14

      01  05  09  13

      00  04  08  12
  */


  biob_explicit_neighborhood_structure neighborhood_structure;
  neighborhood_structure.populate(&roster, 0.11);
  biob_explicit_neighborhood_structure::neighbors_t neighbors;
  neighbors = neighborhood_structure.neighbors(biob_worldpt_index(1));
  //unused variable
  //int debugint = neighbors.size();
  //neighbors should be 0 and 2
  vcl_vector<bool> is_neighbor(roster.num_points());
  for (biob_explicit_neighborhood_structure::neighbors_t::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it){
    is_neighbor[it->index()] = true;
  }
  for (int i = 0; i <= 2; ++i){
    TEST("should be neighbor", is_neighbor[i], true);
  }
  for (int i = 6; i < static_cast<int>(roster.num_points()); ++i){
    TEST("should not be neighbor", is_neighbor[i], false);
  }
  TEST("should not be neighbor", is_neighbor[4], false);
  TEST("should be neighbor", is_neighbor[5], true);
}

TESTMAIN(test_explicit_neighborhood_structure);
