#include <testlib/testlib_test.h>
#include <mw/mw_util.h>
#include <mw/algo/mw_stereo_app.h>
#include <dbdif/algo/dbdif_transfer.h>


static bool test_band_reprojection();

static void test_has_tangent_band_intersection();

//: Currently tests both IO and other functions of mw_discrete_corresp_3
MAIN( test_tangent_band )
{
  START ("Stereo correspondence algorithms that use epipolar bands");

  //---- STATISTICS ----------------------------------------------------------

  bool ret = test_band_reprojection();
  TEST("Return value:",ret,true);

  test_has_tangent_band_intersection();

  SUMMARY();
}

static void test_band_transfer(mw_stereo_app &app);

//: \return true if success
bool
test_band_reprojection()
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
//    args.synth_data_3->value_ = true;
    args.synth_data_med->value_ = true;

  args.precomputed_epip->value_ = "tst.corresp";
  args.write_epipolar_corresp->value_ = false;

  // scmd -compute_epipolars -synthetic_3 -symmetric_n -angles "xx,yy" -err_pos 1 -write_epip -corresp_epip tst.corresp -prefix "./"
  args.compute_epipolars->value_ = true;
  args.symmetric_n->value_ = true;
  args.err_pos->value_ = 0.5;

  bool done=false;
  while (!done) {

    // 30,60,90
    {
      // args.angles->value_.clear(); args.angles->value_.push_back(0); args.angles->value_.push_back(60);
      // OR
      char *a = "30,60,90"; args.angles->parse(&a);

      mw_stereo_app app;
      if (!app.init(args))
        return false;

      test_band_transfer(app);
    }

    done = true;
  }

  return true;
}


  
/*
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
*/

//
void
test_band_transfer(mw_stereo_app &app)
{
  assert(app.nviews_ == 3);
  unsigned npts = app.crv2d_gt_[0].size();
  vcl_cout << "size: " <<  npts << vcl_endl;

  for (unsigned i=0; i < npts; ++i) {

    const double t_err = vnl_math::pi/180.; // 1 deg


    dbdif_2nd_order_point_2d p0 = app.crv2d_gt_[0][i]; //: slicing
    dbdif_2nd_order_point_2d p1 = app.crv2d_gt_[1][i]; //: slicing
    dbdif_2nd_order_point_2d p2_gt = app.crv2d_gt_[2][i]; //: slicing
    dbdif_2nd_order_point_2d p2_reproj;

    double theta_min_reproj,theta_max_reproj;

    dbdif_rig rig(app.cam_[0].Pr_, app.cam_[1].Pr_);

    dbdif_transfer::transfer_tangent_band(
        p0,p1,t_err, &theta_min_reproj,&theta_max_reproj, 
        app.cam_[2], rig );

    double perturb = 1 * t_err;
    double s = vcl_sin(perturb);
    double c = vcl_cos(perturb);
    p2_gt.t = mw_vector_3d (c*p2_gt.t[0] + s*p2_gt.t[1], -s*p2_gt.t[0] + c*p2_gt.t[1], 0);
    bool has_intersection = dbdif_transfer::has_tangent_band_intersection(p2_gt.t[0],p2_gt.t[1],t_err, theta_min_reproj,theta_max_reproj);

    TEST("transferred and ground truth are compatible",has_intersection,true);
  }
}

void
test_has_tangent_band_intersection()
{
  bool has_intersection;
  double tx,ty;
 
  // Test cases from may 15 2008 notes
  //
  {
    mw_vector_2d t(1,1);

    t.normalize();
    tx = t[0];
    ty = t[1];

    double t_err = vnl_math::pi/12;

    double theta_min = vnl_math::pi/4;
    double theta_max = 2*vnl_math::pi/3+vnl_math::pi/12;

    // Case A
    has_intersection = dbdif_transfer::has_tangent_band_intersection(tx,ty,t_err, theta_min,theta_max);
    TEST("Case A",has_intersection,true); 

    has_intersection = dbdif_transfer::has_tangent_band_intersection(tx,ty,t_err, theta_max,theta_min+vnl_math::pi);
    TEST("Case A complement second sector",has_intersection,true); 

    // Case B
    has_intersection = dbdif_transfer::has_tangent_band_intersection(tx,ty,t_err, 0,vnl_math::pi/12);
    TEST("Case B",has_intersection,false); 
    
    // Case C
    has_intersection = dbdif_transfer::has_tangent_band_intersection(-ty,tx,0.5*(vnl_math::pi-vnl_math::pi/6), theta_max,theta_min+vnl_math::pi);
    TEST("Case C",has_intersection,true); 
    
    // Case D
    has_intersection = dbdif_transfer::has_tangent_band_intersection(-ty,tx,t_err, vnl_math::pi/4,3*vnl_math::pi/4);
    TEST("Case D",has_intersection,true); 
    
    // Case D where the green sector was changed to be outside and including x axis
    has_intersection = dbdif_transfer::has_tangent_band_intersection(-1,0,t_err/100.0, vnl_math::pi/4,3*vnl_math::pi/4);
    TEST("Case D2",has_intersection,false); 
    
    // Case E
    has_intersection = dbdif_transfer::has_tangent_band_intersection(0,1,t_err, 3*vnl_math::pi/4,vnl_math::pi+vnl_math::pi/4);
    TEST("Case E",has_intersection,false); 

    t[0] = -1;
    t[1] = 1;
    t.normalize();
    tx = t[0];
    ty = t[1];

    // Case F
    has_intersection = dbdif_transfer::has_tangent_band_intersection(tx,ty,vnl_math::pi/4 + vnl_math::pi/12, vnl_math::pi-vnl_math::pi/6, vnl_math::pi+vnl_math::pi/6);
    TEST("Case F",has_intersection,true); 
  }
}
