#include <testlib/testlib_test.h>
#include <mw/algo/mw_stereo_app.h>

static bool test_band_basic();


MAIN( test_stereo_band )
{
  START ("Stereo correspondence algorithms that use epipolar bands");

  //---- STATISTICS ----------------------------------------------------------

  bool ret = 
  test_band_basic();

  TEST("Return value:",ret,true);

  SUMMARY();
}

static bool test_contains_all_gt_synth(mw_stereo_app &app);

//: \return true if success
bool
test_band_basic()
{

  // set default params

  vul_arg<vcl_string> a_prefix("-prefix", "path to directory of files (suffixed with slash)","./"); // < diff from default
  vul_arg<vcl_string> a_out_path("-out_path", "path to output. Defaults to -prefix");

  vul_arg<vcl_string> a_fname1("-image1", "image fname 1","000-maximage.png");
  vul_arg<vcl_string> a_fname2("-image2", "image fname 2","003-maximage.png");
  vul_arg<vcl_string> a_fname3("-image3", "image fname 3","006-maximage.png");
  vul_arg<vcl_vector<vcl_string> > a_fnames("-images","fname's of images (space-separated)");

  vul_arg<vcl_string> a_corresp_out("-corresp_out", "correspondence output fname 1","out/david-0-3-edges_t10-stereo_result.corresp");
  vul_arg<vcl_string> a_corresp_gt("-corresp_gt", "correspondence ground-truth fname 1","david-0-3-edges_t10.corresp");
  vul_arg<vcl_string> a_precomputed_epip("-corresp_epip","correspondence (pre-computed epipolar candidates) fname", "david-0-3-edges_t10-epipolar.corresp");
  vul_arg<vcl_string> a_corresp_in("-corresp_in", "correspondence (pre-computed) fname","david-0-3-edges_t10-stereo_result-trinocular_costs.corresp");
  
  vul_arg<vcl_string> a_edgels1("-edgels1", "edgels fname 1","000-maximage.nms.t10.edgels");
  vul_arg<vcl_string> a_edgels2("-edgels2", "edgels fname 2","003-maximage.nms.t10.edgels");
  vul_arg<vcl_string> a_edgels3("-edgels3", "edgels fname 3","006-maximage.nms.t10.edgels");
  vul_arg<vcl_string> a_edgel_type("-edgel_type","file format 'edg' or 'vsol'");
  vul_arg<vcl_vector<vcl_string> > a_edgels("-edgels","fname's of edgels (space-separated)");

  vul_arg<bool> a_run_trinocular("-trinocular", "compute trinocular reprojection costs based on point distance",false);
  vul_arg<bool> a_run_trinocular_t_diff("-trinocular_tangent_diff", "compute trinocular reprojection costs based on tangent difference",false);
  vul_arg<bool> a_run_trinocular_k_diff("-trinocular_curvature_diff", "compute trinocular reprojection costs based on curvature difference",false);
  vul_arg<bool> a_write_epipolar_corresp("-write_epip", "write epipolar correspondents to file out/<corresp_epip>",false);
  vul_arg<bool> a_read_epipolar_corresp("-read_epip", "read epipolar correspondents from file <corresp_epip>",false);
  vul_arg<bool> a_compare_corresp("-compare_corresp", "print info on difference between ground-truth and <a_corresp_in>",false);
  vul_arg<bool> a_compute_epipolars("-compute_epipolars", "compute epipolar constraint",false);
  vul_arg<bool> a_symmetric_3("-symmetric_3", "treat trinocular constraints symmetrically",false);
  vul_arg<bool> a_symmetric_n("-symmetric_n", "apply n-view symmetric constraints",false);

  vul_arg<bool> a_run_sel_geometry("-sel_geometry", "compute costs based on differential geometry of Amir's edge linking",false);
  vul_arg<bool> a_read_sel("-read_sel", "read edge linking hypotheses from binary files",false);
  vul_arg<vcl_string> a_sel_in1("-sel_file1", "filename of edge linking hypotheses", "000-maximage.nms.t10.sel2");
  vul_arg<vcl_string> a_sel_in2("-sel_file2", "filename of edge linking hypotheses", "003-maximage.nms.t10.sel2");
  vul_arg<vcl_string> a_sel_in3("-sel_file3", "filename of edge linking hypotheses", "006-maximage.nms.t10.sel2");

  vul_arg<bool> a_synth_data_1("-synthetic_1", "Use synthetic data (ctspheres camera setup)", false);
  vul_arg<bool> a_synth_data_2("-synthetic_2", "Use synthetic data (digicam turntable setup)", false);
  vul_arg<bool> a_synth_data_3("-synthetic_3", "Use synthetic data (digicam turntable setup - sandbox)", false);
  vul_arg<bool> a_synth_data_med("-synthetic_med", "Use synthetic data (digicam turntable setup - medium-sized)", false);
  vul_arg<bool> a_run_synth_geometry("-run_synth_geometry", "Compute costs based on differential geometry from synthetic data ", false);
  vul_arg<bool> a_synth_write_edgels("-synth_write_edgels", "Write edgels to a_edgels1 and a_edgels2 when using synthetic data", false);

  vul_arg<double> a_angle1("-angle1", "angle of 1st view in Deg (for synthetic data)", 0);
  vul_arg<double> a_angle2("-angle2", "angle of 2nd view in Deg (for synthetic data)", 5);
  vul_arg<double> a_angle3("-angle3", "angle of 3rd view in Deg (for synthetic data)", 60);
  vul_arg<vcl_vector<double> > a_angles("-angles","angles of views (for synthetic data)");

  vul_arg<bool> a_perturb_camera("-perturb_camera", "perturb camera for synthetic data. -synthetic_x must be set.", false);
  vul_arg<bool> a_write_perturb_camera("-write_perturb_camera", "write perturbed cameras", false);
  vul_arg<unsigned> a_trinocular_radius("-trinocular_nrad", "trinocular neighborhood radius", 10);
  vul_arg<double> a_err_pos("-err_pos", "localization error range", 2);
  vul_arg<bool> a_remove_epitangency("-no_epitangency",false);
  vul_arg<vcl_string> a_cam_type("-cam_type","camera type: intrinsic_extrinsic or projcamera","");

  mw_stereo_app_args args;

  args.set(
    a_prefix, 
    a_out_path, 
    a_fname1,
    a_fname2, 
    a_fname3,
    a_fnames,
    a_corresp_gt,
    a_precomputed_epip,
    a_edgels1,
    a_edgels2,
    a_edgels3,
    a_edgel_type,
    a_edgels,
    a_run_trinocular,
    a_run_trinocular_t_diff,
    a_run_trinocular_k_diff,
    a_write_epipolar_corresp,
    a_read_epipolar_corresp,
    a_corresp_in,
    a_compare_corresp,
    a_compute_epipolars,
    a_symmetric_3,
    a_symmetric_n,
    a_run_sel_geometry,
    a_read_sel,
    a_sel_in1,
    a_sel_in2,
    a_sel_in3,
    a_synth_data_1,
    a_synth_data_2,
    a_synth_data_med,
    a_synth_data_3,
    a_run_synth_geometry,
    a_synth_write_edgels,
    a_angle1,
    a_angle2,
    a_angle3,
    a_angles,
    a_perturb_camera,
    a_write_perturb_camera,
    a_trinocular_radius,
    a_err_pos,
    a_remove_epitangency,
    a_cam_type
      );

//  const bool full_test = true;
//  if (full_test)
//    args.synth_data_2->value_ = true;
//  else
    args.synth_data_3->value_ = true;
//    args.synth_data_med->value_ = true;

  args.precomputed_epip->value_ = "tst.corresp";
  args.write_epipolar_corresp->value_ = false;


  // Test if constraint really is symmetric:
  
  // scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "xx,yy" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix "./"
  args.compute_epipolars->value_ = true;
  args.symmetric_n->value_ = true;
  args.err_pos->value_ = 0.01;


  bool done=false;
  while (!done) {
    /*
    // 0,60
    unsigned ntups2_a;
    {
      // args.angles->value_.clear(); args.angles->value_.push_back(0); args.angles->value_.push_back(60);
      // OR
      char *a = "0,60"; args.angles->parse(&a);

      mw_stereo_app app;
      if (!app.init(args))
        return false;
      app.run_stereo();
      ntups2_a = app.corr_ep_n_.size();
      if (args.err_pos->value_ <= 1e-8)
      TEST("For precise epipolars and synthetic data, n_tuplets_2 = n_pts?",ntups2_a == app.vsols_[0].size(),true);

      test_contains_all_gt_synth(app);
    }

    // 60,0
    unsigned ntups2_b;
    {
      char *a = "60,0"; args.angles->parse(&a);

      mw_stereo_app app;
      if (!app.init(args))
        return false;
      app.run_stereo();
      ntups2_b = app.corr_ep_n_.size();

      test_contains_all_gt_synth(app);
    }
    */

    // 30,60
    unsigned ntups_30_60;
    {
      // args.angles->value_.clear(); args.angles->value_.push_back(0); args.angles->value_.push_back(60);
      // OR
      char *a = "30,60"; args.angles->parse(&a);

      mw_stereo_app app;
      if (!app.init(args))
        return false;
      app.run_stereo();
      ntups_30_60= app.corr_ep_n_.size();
      test_contains_all_gt_synth(app);
    }

    // 60,30
    unsigned ntups_60_30;
    {
      char *a = "60,30"; args.angles->parse(&a);

      mw_stereo_app app;
      if (!app.init(args))
        return false;
      app.run_stereo();
      ntups_60_30= app.corr_ep_n_.size();

      test_contains_all_gt_synth(app);
    }

    TEST("Equal in reverse (30,60): ",ntups_30_60,ntups_60_30);

    /*
    unsigned ntups3_a;
    // 0,30,60
    {
      // args.angles->value_.clear(); args.angles->value_.push_back(0); args.angles->value_.push_back(60);
      // OR
      char *a = "0,30,60"; args.angles->parse(&a);

      mw_stereo_app app;
      if (!app.init(args))
        return false;
      app.run_stereo();
     
      ntups3_a = app.corr_ep_n_.size();
      if (args.err_pos->value_ <= 1e-8)
      TEST("For precise epipolars and synthetic data, n_tuplets_3 = n_pts?",ntups3_a == app.vsols_[0].size(),true);
      test_contains_all_gt_synth(app);
    }

    unsigned ntups3_b;
    // 60,30,0
    {
      char *a = "60,0,30"; args.angles->parse(&a);

      mw_stereo_app app;
      if (!app.init(args))
        return false;
      app.run_stereo();
      ntups3_b = app.corr_ep_n_.size();
      test_contains_all_gt_synth(app);
    }

    TEST("Equal in reverse (0,30,60), (60,0,30), and (60,30,0): ",ntups3_a,ntups3_b);
    
    // 0,30,60,90
    unsigned ntups_5views;
    {
      char *a = "0,30,60,90,130.77"; args.angles->parse(&a);

      mw_stereo_app app;
      if (!app.init(args))
        return false;
      app.run_stereo();
      ntups_5views = app.corr_ep_n_.size();

      TEST("Sanity nviews",app.nviews_,5);
      bool is_equal=true;
      for (unsigned i=1; i < app.nviews_; ++i) {
        if (app.vsols_[i].size() != app.vsols_[i-1].size())
          is_equal = false;
      }
      TEST("app.vsols_ npts consistent for synthetic data",is_equal,true);

      // test all ground-truth tups (i1,i2,i3,i4,i5) are present

      test_contains_all_gt_synth(app);

      if (args.err_pos->value_ <= 1e-8)
        TEST("For precise epipolars and synthetic data, n_tuplets_5 = n_pts?",ntups_5views == app.vsols_[0].size(),true);
    }

    if(args.err_pos->value_ == 1)
      args.err_pos->value_ = 1e-8;
    else {
      //args.err_pos->value_ == 1e-8;


      done = true;
    }
    */
    done = true;
  }


  // Test ntups = ntups_reverse for some isolated examples of pairs and triplets

  // Test ntups -> n_pts for e_pos near zero, for many tuplet sizes

  return true;
}



bool
test_contains_all_gt_synth(mw_stereo_app &app)
{
  bool contains_all_gt = true;
  for (unsigned i=0; i < app.vsols_[0].size(); ++i) {
    mw_ntuplet tup(app.nviews_, i);
    if (!app.corr_ep_n_.l_.fullp(tup)) {
      contains_all_gt = false;
      vcl_cout << "Epipolar geometry missed corresp: " << tup << vcl_endl;
    }
  }

  TEST("contains_all_gt",contains_all_gt, true);

  return contains_all_gt;
}
