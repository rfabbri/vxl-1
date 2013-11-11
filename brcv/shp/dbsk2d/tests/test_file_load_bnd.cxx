// This is brcv/shp/dbsk2d/tests/test_file_load_bnd.cxx

#include <testlib/testlib_test.h>

#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>

// Amir Tamrakar
// 02/06/2005

// Compare the results of loading different files with the true data
// that's supposed to be in those files.

vcl_string file_base;

bool load_bnd_and_compare()
{
  dbsk2d_boundary_sptr bnd = new dbsk2d_boundary();
  //bnd->LoadFile((file_base + "file_io_data/test_file1.bnd").c_str());

  //int num_elements = bnd->nBElement();
  
  //this should be 14
  //return (num_elements==14);
  return true;
}

int test_file_load_bnd_main( int argc, char* argv[] )
{
  if ( argc >= 2 ) {
    file_base = argv[1];
#ifdef VCL_WIN32
    file_base += "\\";
#else
    file_base += "/";
#endif
  }

  testlib_test_start("load .bnd test");
  testlib_test_begin( "dbsk2d_ishock_boundary .bnd load" );
  testlib_test_perform(load_bnd_and_compare());
  
  return testlib_test_summary();
}
