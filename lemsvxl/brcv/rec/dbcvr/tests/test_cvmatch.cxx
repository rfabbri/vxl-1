#include <testlib/testlib_test.h>
#include <dbcvr/dbcvr_cvmatch.h>
#include <vcl_iostream.h>
//#include <vcl_cmath.h>

//To trace in VC++, put in argument (use your own dir):
// test_cvmatch D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbcvr\tests\file_read_data

MAIN_ARGS(test_cvmatch)
{

  vcl_string dir_base;

  if ( argc >= 2 ) {
    dir_base = argv[1];
#ifdef VCL_WIN32
    dir_base += "\\";
#else
    dir_base += "/";
#endif
  }

  testlib_test_start("testing curve matching ");

  vcl_string data_file = "line1.con";
  // Test generic file loads
  bsol_intrinsic_curve_2d_sptr curve_2d1 = new bsol_intrinsic_curve_2d;
  curve_2d1->readCONFromFile( (dir_base+data_file).c_str() );
  TEST("load line1.con ", curve_2d1->size(), 10);

  bsol_intrinsic_curve_2d_sptr curve_2d2 = new bsol_intrinsic_curve_2d(*curve_2d1);
  //: Actually testing copy constructor of bsol_intrinsic_curve_2d
  TEST("create curve2 from curve1 ", curve_2d2->size(), 10);
  
  dbcvr_cvmatch* curveMatch = new dbcvr_cvmatch();
  curveMatch->setCurve1 (curve_2d1);
  curveMatch->setCurve2 (curve_2d2);
  curveMatch->Match ();
  vcl_cout << "Final cost is: " << curveMatch->finalCost() << vcl_endl;
  TEST("matching curve1 with itself ", curveMatch->finalCost(), 0.0);

  return testlib_test_summary();
}
