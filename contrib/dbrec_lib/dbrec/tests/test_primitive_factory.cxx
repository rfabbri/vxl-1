// This is dbrec/tests/test_primitive_factory.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   April 23, 2009
//
#include <testlib/testlib_test.h>
#include <dbrec/dbrec_primitive_factory.h>
#include <dbrec/dbrec_primitive_factory_sptr.h>
#include <dbrec/dbrec_gaussian_primitive.h>
#include <dbrec/dbrec_part.h>
#include <dbrec/dbrec_part_sptr.h>

static void test_primitive_factory()
{
  dbrec_primitive_factory pf;
  TEST("testing primitive factory base class", pf.get_primitive(0), 0);

  dbrec_gaussian_factory* gpf = new dbrec_gaussian_factory();
  gpf->populate(4, 2.0f, 1.0f, true, false);
  gpf->populate(4, 2.0f, 1.0f, false, false);
  dbrec_primitive_factory_sptr gpfs = gpf;
  dbrec_hierarchy_sptr h = gpfs->construct_random_classifier(2);
  TEST("test random classifier construction", h->class_cnt(), 2);
  h->print(vcl_cout);
}

TESTMAIN( test_primitive_factory );
