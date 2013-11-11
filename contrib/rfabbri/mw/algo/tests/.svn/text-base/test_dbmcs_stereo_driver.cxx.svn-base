//:
//\file
//\brief Batch execution of multiview curve stereo
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//
#include <testlib/testlib_test.h>
#include <mw/pro/dbmcs_stereo_driver.h>

MAIN( test_dbmcs_stereo_driver )
{
   mw_curve_stereo_data_path dpath;
   vcl_string fname_path("/usr/local/moredata/subset/");

  // Hardcoded:
  // mw_data::get_capitol_building_subset(&dpath);

  // Files:
  bool retval = mw_data::read_frame_data_list_txt(
      fname_path,
      &dpath,
      mw_util::MW_INTRINSIC_EXTRINSIC
      );
  TEST("data list from file", retval, true);
  if (!retval)
    exit(1);

  // unsigned nviews = dpath.nviews();

  dbmcs_stereo_instance_views frames_to_match;

  // Hardcoded:
  // dbmcs_view_set::get_capitol_subset_view_instances(&frames_to_match);

  // Files:
  retval =  dbmcs_view_set::read_txt(
      fname_path + vcl_string("/mcs_stereo_instances.txt"), 
      &frames_to_match);

  TEST("frames to match from file", retval, true);
  if (!retval)
    exit(1);

  // Alternative ways to generate views
//  dbmcs_view_set view_gen;
//  view_gen.set_stereo_baseline(xx);
//  view_gen.set_num_confirmation_views(xx);
//  view_gen.set_confirmation_view_baseline(xx);
//  view_gen.set_cams(&cams);
//  view_gen.set_stereo_overlap(&delta_frame_between_stereos);
//  view_gen.populate(&instance);

  dbmcs_concurrent_stereo_driver s(dpath, frames_to_match);

  //: How many stereo matchers can run simultaneously
  s.set_max_concurrent_matchers(4);

  retval = s.init();
  TEST("Stereo driver init return value", retval, true);

  //: Run many pairwise stereo programs, as many as
  // frames_to_match.num_binocular_stereo_runs();
  retval = s.run();
  TEST("Stereo driver run return value", retval, true);

//  TODO: update - Using curve sketch now: 
//
//  TEST("Curve stereo interface: one attribute struct per curve", s.curves_3d().size(),
//  s.curves_3d_attributes().size());

  // s.curves_3d_attributes(); //:< returns ref to a vcl_vector of attributes

  //: Write 3D curves to file. using curve sketcnb
  /* TODO update for 3d curve sketch
  retval = mywritev(
      fname_path + vcl_string("/out/test-3Dcurve"), ".dat", s.curves_3d());
  if(!retval) {
    vcl_cerr << "Error while trying to write file.\n";
    abort();
  }
  */

  SUMMARY();
}
