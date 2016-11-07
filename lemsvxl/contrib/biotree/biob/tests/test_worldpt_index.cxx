#include <testlib/testlib_test.h>
#include <biob/biob_worldpt_index.h>

static void test_worldpt_index(){
 biob_worldpt_index pti(7);
 TEST_NEAR("index", pti.index(), 7, 0);
 biob_worldpt_index other_pti = pti;
 TEST_NEAR("other index", other_pti.index(), 7, 0);
}

TESTMAIN(test_worldpt_index);
