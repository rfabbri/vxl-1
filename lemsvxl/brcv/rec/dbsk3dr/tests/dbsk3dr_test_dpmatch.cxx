#include <testlib/testlib_test.h>
#include <dbsk3dr/dbsk3dr_dpmatch.h>
#include <vcl_iostream.h>

//To trace in VC++, put in argument (use your own dir):
// dbsk3dr_test_dpmatch D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbsk3dr\tests\data

MAIN_ARGS(dbsk3dr_test_dpmatch)
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

  //=======================================
  testlib_test_start ("Test matching identical curves");

  vcl_string data_file = "con_3d_1.con3";
  // Test generic file loads
  dbsk3dr_ms_curve* c1 = new dbsk3dr_ms_curve;
  c1->read_con3_file ((dir_base+data_file).c_str());
  TEST("load con_3d_1.con3 ", c1->size(), 10);

  dbsk3dr_ms_curve* c2 = new dbsk3dr_ms_curve;
  c2->read_con3_file ((dir_base+data_file).c_str());
  TEST("load con_3d_1.con3 ", c2->size(), 10);
  
  dbsk3dr_dpmatch* dpm = new dbsk3dr_dpmatch();
  dpm->setCurve1 (c1);
  dpm->setCurve2 (c2);

  dpm->Match ();

  vcl_cout << "Final cost is: " << dpm->finalCost() << vcl_endl;
  TEST("matching curve1 with itself ", dpm->finalCost(), 0.0);

  dpm->ListDPTable ();
  dpm->ListAlignCurve ();

  //=======================================
  testlib_test_start( "Test matching two curves");
  delete c2;
  delete dpm;

  c2 = new dbsk3dr_ms_curve;
  vcl_string data_file2 = "con_3d_2.con3";
  c2->read_con3_file ((dir_base+data_file2).c_str());
  TEST("load con_3d_2.con3 ", c2->size(), 12);

  dpm = new dbsk3dr_dpmatch();
  dpm->setCurve1 (c1);
  dpm->setCurve2 (c2);

  dpm->Match ();

  vcl_cout << "Final cost is: " << dpm->finalCost() << vcl_endl;
  
  TEST_NEAR ("matching curve1 with itself ", dpm->finalCost(), 10.2256, 0.001);

  dpm->ListDPTable ();
  dpm->ListAlignCurve ();

  return testlib_test_summary();
}
