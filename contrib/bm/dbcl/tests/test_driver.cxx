//this is /contrib/bm/dbcl/tests/test_driver.cxx

#include <testlib/testlib_register.h>

DECLARE( test_linear_classifier );
DECLARE( test_linear_classifier_3_classes );

void register_tests()
{
    REGISTER( test_linear_classifier );
    REGISTER( test_linear_classifier_3_classes );
}

DEFINE_MAIN;