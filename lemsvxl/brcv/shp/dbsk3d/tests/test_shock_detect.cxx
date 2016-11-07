//This is lemsvxl/brcv/shp/dbsk3d/tests/test_shock_detect.cxx
//  Ming-Ching Chang  Mar 02, 2005
//
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\shp\dbsk3d\tests\data 
//

#include <vcl_string.h>
#include <testlib/testlib_test.h>

#include <dbsk3d/pro/dbsk3d_process.h>

// Default Parameters:
#define MAX_SAMPLE_RATIO        5.0f
#define PRUNE_BOXRATIO          0.1f        
#define COST_TH                 5.0f
#define RMIN_RATIO              1.0f

bool test_shock_detection (vcl_string prefix,
                           const float reg_th, const float rmin_ratio,
                           const float prune_boxr)
{
  bool result = false;

  dbsk3d_pro sp;
  sp.set_dir_prefix (prefix);

  //Recover full scaffold from QHull.
  result = sp.fs_recover_qhull ();
  vcl_cout << vcl_endl << vcl_endl; 
  TEST("fs_recover_qhull (should be 1/true) ", result, 1);

  //Surface Segregation.
  sp.fs_segre()->run_surface_segre (MAX_SAMPLE_RATIO, 0, 1.0f, false);

  //Shock regularization.
  sp.fs_regul()->trim_bnd_A122_FFs (rmin_ratio);
  sp.fs_ss()->build_fs_sheet_set ();
  sp.fs_regul()->run_shock_regul (reg_th, false);

  ////Prune boundary shock sheets with low compactness.
  int iter = 1;
  float c_th = 0.1f;
  sp.run_compactness_trim_xforms (iter, c_th, false);

  //Bounding box pruning
  sp.run_bbox_pruning (prune_boxr, false);
  
  //Detect valid link types.
  sp.fs_mesh()->detect_valid_FE_type ();

  //Extract the largest (i-th) inside shock component (delete all non-component shocks)
  vcl_set<int> ith_comp_list;
  ith_comp_list.insert (0);
  result = sp.determine_inside_shock_comp (ith_comp_list, false);
  TEST("determine_inside_shock_comp (should be 1/true) ", result, 1);

  result = sp.build_ms_hypg (0);
  vcl_cout << vcl_endl << vcl_endl;
  TEST("build_ms_hypg (should be 1/true) ", result, 1);

  return result;
}


MAIN_ARGS (dbsk3d_test_shock_detect)
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

  testlib_test_start ("dbsk3d_test_shock_detect: bones1");
  vcl_string data_prefix = dir_base + "bones1";
  test_shock_detection (data_prefix, COST_TH, RMIN_RATIO, PRUNE_BOXRATIO); //, 1.5f, 0.1f);

  testlib_test_start ("dbsk3d_test_shock_detect: tetra2");
  data_prefix = dir_base + "tetra2";
  test_shock_detection (data_prefix, 1.0f, 1.0f, 5.0f);

  return testlib_test_summary();   
}

