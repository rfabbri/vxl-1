#include <testlib/testlib_register.h>
DECLARE( test_bioproj_io );
DECLARE( test_bioproj_mem_io );
DECLARE( test_bioproj_filters );
DECLARE( test_error_function );
DECLARE( test_simpson_integral );

void register_tests()
{
  REGISTER( test_bioproj_io);
  REGISTER( test_bioproj_mem_io);
  REGISTER( test_bioproj_filters);
  REGISTER( test_error_function);
  REGISTER( test_simpson_integral);
}

DEFINE_MAIN;
