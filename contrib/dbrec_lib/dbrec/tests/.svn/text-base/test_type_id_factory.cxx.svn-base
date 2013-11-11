// This is dbrec/tests/test_type_id_factory.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   April 23, 2009
//
#include <testlib/testlib_test.h>
#include <dbrec/dbrec_type_id_factory.h>

static void test_type_id_factory()
{
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  TEST("type id new type", ins->new_type(), 1);
  TEST("type id new type", ins->new_type(), 2);
  ins->register_type(10);
  TEST("type id new type", ins->new_type(), 11);
}

TESTMAIN( test_type_id_factory );
