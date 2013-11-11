// This is brcv/shp/dbsk2d/tests/test_compute_ishock_bkt.cxx

#include <testlib/testlib_test.h>

#include <dbsk2d/dbsk2d_ishock_boundary_bkt_sptr.h>
#include <dbsk2d/dbsk2d_ishock_boundary_bkt.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_ishock_detector_bkt.h>

// Amir Tamrakar
// 02/06/2005

extern vcl_string file_base;

//: test shock computation using buckting
bool load_bnd_and_compute_shock_bkt()
{
  dbsk2d_ishock_boundary_bkt_sptr bnd = new dbsk2d_ishock_boundary_bkt(NO_LIMIT);

  vcl_string filename = file_base + "file_io_data/3points.bnd";
  bnd->LoadFile(filename.c_str());

  dbsk2d_ishock_detector_bkt my_detector(bnd);
  my_detector.detect_shocks();

  //: I'm satisfied if it doesn't crash
  // \todo after computation call validation function to truly make sure that it worked
  return true;
}

int test_compute_ishock_bkt_main( int argc, char* argv[] )
{
  if ( argc >= 2 ) {
    file_base = argv[1];
#ifdef VCL_WIN32
    file_base += "\\";
#else
    file_base += "/";
#endif
  }

  testlib_test_start(" Bucketing ishock computation test");
  testlib_test_begin( "  dbsk2d_ishock_detector_bkt " );
  testlib_test_perform( load_bnd_and_compute_shock_bkt());
  
  return testlib_test_summary();
}
