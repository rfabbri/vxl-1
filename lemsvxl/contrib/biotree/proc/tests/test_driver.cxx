#include <testlib/testlib_register.h>

// DECLARE();
DECLARE( test_bioproc_dummy_splat_resource );
DECLARE( test_bioproc_filtering_proc );
//DECLARE( test_bioproc_splr_proc );  
//DECLARE( test_bioproc_splr_proc_splats );  
void register_tests()
{
  REGISTER( test_bioproc_dummy_splat_resource);
  REGISTER( test_bioproc_filtering_proc);
  //  REGISTER( test_bioproc_splr_proc );
  //  REGISTER( test_bioproc_splr_proc_splats );
}

DEFINE_MAIN;
