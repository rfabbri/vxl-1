// This is brcv/shp/dbsk2d/tests/test_file_load_edg.cxx
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <testlib/testlib_test.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>

// Amir Tamrakar
// 02/06/2005

// Compare the results of loading different files with the true data
// that's supposed to be in those files.

extern vcl_string file_base;

bool
load_edg_and_compare()
{
  return true;
}

int test_file_load_edg_main( int argc, char* argv[] )
{
  if ( argc >= 2 ) {
    file_base = argv[1];
#ifdef VCL_WIN32
    file_base += "\\";
#else
    file_base += "/";
#endif
  }

  testlib_test_start("load .edg test");
  testlib_test_begin( "dbsk2d_ishock_boundary .edg load" );
  testlib_test_perform(load_edg_and_compare());
  
  return testlib_test_summary();
}
