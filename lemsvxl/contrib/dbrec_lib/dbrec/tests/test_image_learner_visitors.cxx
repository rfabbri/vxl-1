// This is dbrec/tests/test_stat_collector.cxx
//:
// \file
// \brief Various tests for dbrec
// \author Ozge C. Ozcanli (Brown)
// \date   April 30, 2009
//
#include <testlib/testlib_test.h>
#include <dbrec/dbrec_image_learner_visitors.h>
#include <dbrec/dbrec_image_learner_visitors_sptr.h>
#include <dbrec/dbrec_part_context.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

static void test_image_learner_visitors()
{
  vil_image_view<float> img(100, 100);
  img.fill(1.0f);
  vil_image_view<bool> mask(100, 100);
  mask.fill(true);

}

TESTMAIN( test_image_learner_visitors );
