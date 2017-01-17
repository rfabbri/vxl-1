// This is dbrec/tests/test_visitors.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   May 01, 2009
//
#include <testlib/testlib_test.h>

#include <dbrec_mic/dbrec_pca_utils.h>
#include <dbrec_mic/dbrec_pca_utils_sptr.h>

static void test_pca_util()
{
  dbrec_template_matching_sptr tm = new dbrec_template_matching;
  
  TEST("testing dbrec_template_matching constructor", !tm, false);  

  vnl_vector<double> v(3);
  v[0] = 2.0;
  v[1] = 4.0;
  v[2] = 1.0;
  tm->standardize_vector(v);

}

TESTMAIN( test_pca_util );
