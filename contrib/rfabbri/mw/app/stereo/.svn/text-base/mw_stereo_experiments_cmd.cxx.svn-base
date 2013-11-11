#include <testlib/testlib_test.h>
#include <mw/mw_util.h>
#include <vgl/vgl_box_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <dbecl/dbecl_epiband.h>
#include <dbecl/dbecl_epiband_builder.h>
#include <mw/algo/mw_stereo_app.h>

//: Command to run some experiments based on epipolar bands and
//differential-geometric stereo

struct mw_stereo_experiments_args : public mw_stereo_app_args {
  vul_arg<vcl_string> *fname_id;
  vul_arg<vcl_string> *simulation_type;
  vul_arg<unsigned> *max_nviews;
};

class mw_stereo_experiments_app {
public:

  mw_stereo_experiments_app() {}
  ~mw_stereo_experiments_app() {}

  mw_stereo_experiments_args args;

  bool band_statistics();
  void initialize(int argc, char ** argv);
  void run_ntuplet_test_avg(vcl_string fname_part);
  bool ntuplet_test_avg(
      vcl_vector<double> *min_ntup,
      vcl_vector<double> *max_ntup,
      vcl_vector<double> *med_ntup,
      vcl_vector<double> *avg_ntup,
      bool verbose);

  bool 
  gain_in_epipolar();
  
  static void
  gain_in_epipolar_for_exactly_nviews(
    unsigned n_v,
    vcl_vector<bool> &is_specified,
    vcl_vector<vsol_point_2d_sptr> &specified_pts,
    vcl_vector<vcl_vector<vsol_point_2d_sptr> > &vsols,
    vcl_vector< vcl_vector<dbecl_epiband *> > &epband_,
    const vcl_vector <vsol_box_2d_sptr> &bbox_,
    const vcl_vector< vcl_vector<vpgl_fundamental_matrix<double> > > &fm,
    double err_pos,
    vcl_vector<double> &gain);
};

//static bool band_statistics(mw_stereo_app_args &args, vcl_string &);
//static void initialize(int argc, char ** argv, mw_stereo_app_args &);


int
main(int argc, char ** argv)
{

  START ("Statistics on stereo correspondence algorithms that use epipolar bands");

  mw_stereo_experiments_app app;
  app.initialize(argc,argv);

  bool ret=true;

  if (app.args.simulation_type->value_ == "band_stat") {
    ret= app.band_statistics() && ret;
    TEST("Test return value:",ret,true);
  }

  if (app.args.simulation_type->value_ == "epi_gain") {
    ret = app.gain_in_epipolar() && ret;
    TEST("Test return value:",ret,true);
  }

  SUMMARY();

  return ret;
}

void mw_stereo_experiments_app::
initialize(int argc, char ** argv)
{
  // set default params

  static   vul_arg<vcl_string> a_prefix("-prefix", "path to directory of files (suffixed with slash)","./out/"); // < diff from default
  static   vul_arg<vcl_string> a_out_path("-out_path", "path to output. Defaults to -prefix");
  
  static   vul_arg<vcl_string> a_fname1("-image1", "image fname 1","000-maximage.png");
  static   vul_arg<vcl_string> a_fname2("-image2", "image fname 2","003-maximage.png");
  static   vul_arg<vcl_string> a_fname3("-image3", "image fname 3","006-maximage.png");
  static   vul_arg<vcl_vector<vcl_string> > a_fnames("-images","fname's of images (space-separated)");
 
  static   vul_arg<vcl_string> a_corresp_out("-corresp_out", "correspondence output fname 1","out/david-0-3-edges_t10-stereo_result.corresp");
  static   vul_arg<vcl_string> a_corresp_gt("-corresp_gt", "correspondence ground-truth fname 1","david-0-3-edges_t10.corresp");
  static   vul_arg<vcl_string> a_precomputed_epip("-corresp_epip","correspondence (pre-computed epipolar candidates) fname", "david-0-3-edges_t10-epipolar.corresp");
  static   vul_arg<vcl_string> a_corresp_in("-corresp_in", "correspondence (pre-computed) fname","david-0-3-edges_t10-stereo_result-trinocular_costs.corresp");

  static   vul_arg<vcl_string> a_edgels1("-edgels1", "edgels fname 1","000-maximage.nms.t10.edgels");
  static   vul_arg<vcl_string> a_edgels2("-edgels2", "edgels fname 2","003-maximage.nms.t10.edgels");
  static   vul_arg<vcl_string> a_edgels3("-edgels3", "edgels fname 3","006-maximage.nms.t10.edgels");
  static   vul_arg<vcl_string> a_edgel_type("-edgel_type","file format 'edg' or 'vsol'");
  static   vul_arg<vcl_vector<vcl_string> > a_edgels("-edgels","fname's of edgels (space-separated)");

  static   vul_arg<bool> a_run_trinocular("-trinocular", "compute trinocular reprojection costs based on point distance",false);
  static   vul_arg<bool> a_run_trinocular_t_diff("-trinocular_tangent_diff", "compute trinocular reprojection costs based on tangent difference",false);
  static   vul_arg<bool> a_run_trinocular_k_diff("-trinocular_curvature_diff", "compute trinocular reprojection costs based on curvature difference",false);
  static   vul_arg<bool> a_write_epipolar_corresp("-write_epip", "write epipolar correspondents to file out/<corresp_epip>",false);
  static   vul_arg<bool> a_read_epipolar_corresp("-read_epip", "read epipolar correspondents from file <corresp_epip>",false);
  static   vul_arg<bool> a_compare_corresp("-compare_corresp", "print info on difference between ground-truth and <a_corresp_in>",false);
  static   vul_arg<bool> a_compute_epipolars("-compute_epipolars", "compute epipolar constraint",false);
  static   vul_arg<bool> a_symmetric_3("-symmetric_3", "treat trinocular constraints symmetrically",false);
  static   vul_arg<bool> a_symmetric_n("-symmetric_n", "apply n-view symmetric constraints",false);

  static   vul_arg<bool> a_run_sel_geometry("-sel_geometry", "compute costs based on differential geometry of Amir's edge linking",false);
  static   vul_arg<bool> a_read_sel("-read_sel", "read edge linking hypotheses from binary files",false);
  static   vul_arg<vcl_string> a_sel_in1("-sel_file1", "filename of edge linking hypotheses", "000-maximage.nms.t10.sel2");
  static   vul_arg<vcl_string> a_sel_in2("-sel_file2", "filename of edge linking hypotheses", "003-maximage.nms.t10.sel2");
  static   vul_arg<vcl_string> a_sel_in3("-sel_file3", "filename of edge linking hypotheses", "006-maximage.nms.t10.sel2");

  static   vul_arg<bool> a_synth_data_1("-synthetic_1", "Use synthetic data (ctspheres camera setup)", false);
  static   vul_arg<bool> a_synth_data_2("-synthetic_2", "Use synthetic data (digicam turntable setup)", false);
  static   vul_arg<bool> a_synth_data_med("-synthetic_med", "Use synthetic data (digicam turntable setup - sandbox)", false);
  static   vul_arg<bool> a_synth_data_3("-synthetic_3", "Use synthetic data (digicam turntable setup - sandbox)", false);
  static   vul_arg<bool> a_run_synth_geometry("-run_synth_geometry", "Compute costs based on differential geometry from synthetic data ", false);
  static   vul_arg<bool> a_synth_write_edgels("-synth_write_edgels", "Write edgels to a_edgels1 and a_edgels2 when using synthetic data", false);

  static   vul_arg<double> a_angle1("-angle1", "angle of 1st view in Deg (for synthetic data)", 0);
  static   vul_arg<double> a_angle2("-angle2", "angle of 2nd view in Deg (for synthetic data)", 5);
  static   vul_arg<double> a_angle3("-angle3", "angle of 3rd view in Deg (for synthetic data)", 60);
  static   vul_arg<vcl_vector<double> > a_angles("-angles","angles of views (for synthetic data)");

  static   vul_arg<bool> a_perturb_camera("-perturb_camera", "perturb camera for synthetic data. -synthetic_x must be set.", false);
  static   vul_arg<bool> a_write_perturb_camera("-write_perturb_camera", "write perturbed cameras", false);
  static   vul_arg<unsigned> a_trinocular_radius("-trinocular_nrad", "trinocular neighborhood radius", 10);
  static   vul_arg<double> a_err_pos("-err_pos", "localization error range", 2);
  static   vul_arg<bool> a_remove_epitangency("-no_epitangency",false);

  static   vul_arg<vcl_string> a_fname_id("-fname_id","special name part to be included in filename (e.g. PID)","");

  static   vul_arg<vcl_string> a_simulation_type("-simulation_type","simulation name to be run: band_stat or epi_gain","");
  static   vul_arg<vcl_string> a_cam_type("-cam_type","camera type: intrinsic_extrinsic or projcamera","");
  static   vul_arg<unsigned> a_max_nviews("-max_nviews","maximum number of views to be considered by the #tuplets test",2);

  vul_arg_parse(argc,argv);
  vcl_cout << "Fname id: " << a_fname_id() << vcl_endl;
  vcl_cout << "Filenames: \n";
  print_value(vcl_cout, a_fnames);
  vcl_cout << vcl_endl;

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

  args.fname_id = &a_fname_id;
  args.simulation_type = &a_simulation_type;
  args.max_nviews = &a_max_nviews;
}

/*
static bool
direct_ntuplet_test(mw_stereo_app_args &args, const vcl_vector<double> &all_angles, vcl_vector<double> *n_of_tuplets,bool verbose);

bool
direct_ntuplet_test(mw_stereo_app_args &args, const vcl_vector<double> &all_angles, vcl_vector<double> *n_of_tuplets,bool verbose)
{
  args.angles->value_.clear();
  args.angles->value_.push_back(all_angles[0]);
  for (unsigned iv=1; iv < all_angles.size(); ++iv) {
     
    args.angles->value_.push_back(all_angles[iv]);
    mw_stereo_app app;
    if (!app.init(args))
      return false;
    app.run_stereo();

    double ntup = (double)(app.corr_ep_n_.size());

    n_of_tuplets->push_back(ntup);
    if (verbose) {
      vcl_cout << "-----------------------------------------------------\n" << vcl_endl;
      vcl_cout << "Number of tuplets: "  << ntup << vcl_endl;
      vcl_cout << "=====================================================\n" << vcl_endl;
    }
  }
  return true;
}
*/

bool mw_stereo_experiments_app::
ntuplet_test_avg(
    vcl_vector<double> *min_ntup,
    vcl_vector<double> *max_ntup,
    vcl_vector<double> *med_ntup,
    vcl_vector<double> *avg_ntup,
    bool verbose)
{

  unsigned max_nviews = args.max_nviews->value_; 
  assert(max_nviews >= 2);

  bool real_data;
  
  if (args.edgels->value_.size()) {
//    max_nviews = args.edgels->value_.size();
    real_data = true;
    assert (max_nviews <= args.edgels->value_.size());
  } else {
//    max_nviews  = args.angles->value_.size();
    real_data = false;
    assert (max_nviews <= args.angles->value_.size());
  }

  vcl_vector<vcl_string> all_fnames; 
  vcl_vector<vcl_string> all_edgel_fnames;
  vcl_vector<double> all_angles;

  if (real_data) {
    all_fnames = args.fnames->value_;
    all_edgel_fnames = args.edgels->value_;
    assert (all_fnames.size() == all_edgel_fnames.size());
  } else {
    all_angles = args.angles->value_;
  }

  //: maximum number of tuplets to consider for statistics
  vcl_vector<unsigned> n_stat(max_nviews+20,1);

  n_stat[1] = 5; // max pairs
  n_stat[2] = 3; // max triplets 
  n_stat[3] = 3; // max quadruplets
  n_stat[4] = 3; // max quintuplets 
  n_stat[5] = 3; // max sextuplets
  n_stat[6] = 3; // max septuplets

//  n_stat[1] = 2; // max pairs
//  n_stat[2] = 2; // max triplets 
//  n_stat[3] = 2; // max quadruplets
//  n_stat[4] = 2; // max quintuplets 
//  n_stat[5] = 2; // max sextuplets
//  n_stat[6] = 2; // max septuplets

  for (unsigned n_v=2; n_v <= max_nviews; ++n_v) {
    if (verbose) {
      vcl_cout << "===================================\n";
      vcl_cout << "Doing ntuplets size: " << n_v << vcl_endl;
      vcl_cout << "-----------------------------------\n";
    }
    vcl_vector<double> ntup_nv;

    for (unsigned i=0; i <= max_nviews-n_v && i < n_stat[n_v-1]; ++i) {

      if (real_data) {
        args.fnames->value_.clear();
        args.edgels->value_.clear();
        for (unsigned k=i; k < i+n_v; ++k) {
          args.fnames->value_.push_back(all_fnames[k]);
          args.edgels->value_.push_back(all_edgel_fnames[k]);
        }
      } else {
        args.angles->value_.clear();
        for (unsigned k=i; k < i+n_v; ++k) {
          args.angles->value_.push_back(all_angles[k]);
        }
      }

      mw_stereo_app app;
      if (!app.init(args))
        return false;
      app.run_stereo();

      ntup_nv.push_back((double)(app.corr_ep_n_.size()));
    }

    unsigned idx;
    max_ntup->push_back(mw_util::max(ntup_nv, idx));
    min_ntup->push_back(mw_util::min(ntup_nv, idx));
    med_ntup->push_back(mw_util::median(ntup_nv));
    avg_ntup->push_back(mw_util::mean(ntup_nv));
  }

  return true;
}

void mw_stereo_experiments_app::
run_ntuplet_test_avg(vcl_string fname_part)
{
  vcl_string fname(fname_part + vcl_string(".txt"));

  vcl_vector<double> max_ntup,min_ntup,med_ntup,avg_ntup;

  bool ret = ntuplet_test_avg(&min_ntup, &max_ntup, &med_ntup, &avg_ntup, true);
  TEST("Terminated ok?",ret,true);

  {
  vcl_string fname_tmp  = args.out_path->value_ + vcl_string("min") + fname;
  vcl_cout << "Writing " << fname_tmp << vcl_endl;
  mywrite_ascii(fname_tmp,min_ntup);
  }
  {
  vcl_string fname_tmp  = args.out_path->value_ + vcl_string("max") + fname;
  vcl_cout << "Writing " << fname_tmp << vcl_endl;
  mywrite_ascii(fname_tmp,max_ntup);
  }
  {
  vcl_string fname_tmp  = args.out_path->value_ + vcl_string("med") + fname;
  vcl_cout << "Writing " << fname_tmp << vcl_endl;
  mywrite_ascii(fname_tmp,med_ntup);
  }
  {
  vcl_string fname_tmp  = args.out_path->value_ + vcl_string("avg") + fname;
  vcl_cout << "Writing " << fname_tmp << vcl_endl;
  mywrite_ascii(fname_tmp,avg_ntup);
  }
}

//: \return true if success
bool mw_stereo_experiments_app::
band_statistics()
{
  vcl_string fname_id(args.fname_id->value_);
  args.compute_epipolars->value_ = true;
  args.symmetric_n->value_ = true;

//  args.write_epipolar_corresp->value_ = true;

//  {
//  args.angles->parse(&a);
//  vcl_vector<double>   all_angles;
//  all_angles = args.angles->value_;

//  args.precomputed_epip->value_ = vcl_string(a) + vcl_string("-tst.corresp");

//  vcl_string fname(a);
//  vcl_string ext(".txt");
//  fname = args.out_path->value_ + vcl_string("ntups-digicam_turntable-") + vcl_string("err_pos_") + vcl_string(pval) + vcl_string("-angles_") + fname + ext;
//  vcl_vector<double> n_of_tuplets;
//  bool ret=direct_ntuplet_test(args,all_angles,&n_of_tuplets,/*verbose*/ true);
//  TEST("Terminated ok?",ret,true);

//  vcl_cout << "Writing " << fname << vcl_endl;
//  mywrite_ascii(fname,n_of_tuplets);
//  }

//  char *a = "0,30,60,90,120";
//  char *a = "0,30,60,90,120,150,15,75";
//  args.angles->parse(&a);

//  vcl_string fname_part(a);
//  vcl_string ext(".txt");
//  fname_part = vcl_string("ntups-digicam_turntable-") + vcl_string("err_pos_") + vcl_string(pval) + vcl_string("-angles_") + fname_part + ext;
  run_ntuplet_test_avg(fname_id);

  // A permutation
//  char *a = "0,75,105,135,15,160,50,120,90,30,35,140";
//  char *a = "120,30,60,90,0";
//  {
//  args.angles->parse(&a);
//  vcl_vector<double>   all_angles;
//  all_angles = args.angles->value_;

//  args.precomputed_epip->value_ = vcl_string(a) + vcl_string("-tst.corresp");

//  vcl_string fname(a);
//  vcl_string ext(".txt");
//  fname = args.out_path->value_ + vcl_string("digicam_turntable-") + vcl_string("err_pos_") + vcl_string(pval) + vcl_string("-angles_") + fname + ext;
//  vcl_vector<double> n_of_tuplets;
//  bool ret=direct_ntuplet_test(args,all_angles,&n_of_tuplets,/*verbose*/ true);
//  TEST("Terminated ok?",ret,true);

//  vcl_cout << "Writing " << fname << vcl_endl;
//  mywrite_ascii(fname,n_of_tuplets);
//  }

  return true;
}

//#define DEBUG 1

//: returns true if success
bool mw_stereo_experiments_app::
gain_in_epipolar()
{
  const bool verbose=true;

  args.compute_epipolars->value_ = true;
  args.symmetric_n->value_ = true;


  mw_stereo_app app;

  bool ret = app.init(args);
  if (!ret) return false; 

  vcl_vector<vcl_vector <vpgl_fundamental_matrix<double> > > fms_lst;


  fms_lst.resize(app.nviews_);
  for (unsigned i=0; i < app.nviews_; ++i) {
    fms_lst[i].resize(app.nviews_);
    for (unsigned k=0; k < app.nviews_; ++k) {
      if (k == i)
        continue;
      fms_lst[i][k] = vpgl_fundamental_matrix <double> (app.cam_[i].Pr_,app.cam_[k].Pr_);
    }
  }

  vcl_vector <vsol_box_2d_sptr> bbox_lst(app.nviews_);

  unsigned total_npts=0;
  for (unsigned i=0; i < app.nviews_; ++i) {
    vsol_polyline_2d poly(app.vsols_[i]);
    bbox_lst[i] = poly.get_bounding_box();
    total_npts+=app.vsols_[i].size();
  }


  if (verbose)
    vcl_cout << "Done getting bounding box and computing fmatrices\n";


  unsigned const max_nviews = app.nviews_;

  vcl_vector<double> max_gain, min_gain, med_gain, avg_gain;
  for (unsigned n_v=2; n_v <=max_nviews; ++n_v) {
    if (verbose) {
      vcl_cout << "===================================\n";
      vcl_cout << "Computing gain in epipolar for #views = " << n_v << vcl_endl;
      vcl_cout << "-----------------------------------\n";
    }

    vcl_vector< vcl_vector<dbecl_epiband *> > epband;
    vcl_vector<vcl_vector <vpgl_fundamental_matrix<double> > > fms;
    vcl_vector< vcl_vector<vsol_point_2d_sptr> > vsols;
    vcl_vector <vsol_box_2d_sptr> bbox;

    vsols.resize(n_v);
    fms.resize(n_v);
    bbox.resize(n_v);
    epband.resize(n_v);
    for (unsigned i=0; i < n_v; ++i) {
      epband[i].resize(n_v,0);
      fms[i].resize(n_v);
    }

    vcl_vector<double> gain;
    gain.reserve(total_npts*n_v*n_v*(max_nviews-n_v+1));

    vcl_vector<vsol_point_2d_sptr> specified_pts;
    vcl_vector<bool> is_specified;

    specified_pts.resize(n_v);
    is_specified.resize(n_v,false);


    for (unsigned offset=0; offset <= (max_nviews-n_v); ++offset) {
      for (unsigned i=0; i < n_v; ++i) {
        bbox[i] = bbox_lst[i+offset];
        vsols[i] = app.vsols_[i+offset];
        for (unsigned k=0; k < n_v; ++k) {
          if (epband[i][k]) {
            delete epband[i][k];
            epband[i][k] = 0;
          }
          if (k != i) {
            fms[i][k] = fms_lst[i+offset][k+offset];
          }
        }
      }

      gain_in_epipolar_for_exactly_nviews(
        n_v,
        is_specified,
        specified_pts,
        vsols,
        epband,
        bbox,
        fms,
        app.err_pos_,
        gain
      );
    }

    unsigned idx;
    assert(gain.size());
    max_gain.push_back(mw_util::max(gain,idx));
    min_gain.push_back(mw_util::min(gain,idx));
    avg_gain.push_back(mw_util::mean(gain));
    med_gain.push_back(mw_util::median(gain));
  }

  // write to file

  vcl_string fname_id(args.fname_id->value_ + vcl_string(".txt"));

  {
  vcl_string fname_tmp  = args.out_path->value_ + vcl_string("min-gain") + fname_id;
  vcl_cout << "Writing " << fname_tmp << vcl_endl;
  mywrite_ascii(fname_tmp,min_gain);
  }
  {
  vcl_string fname_tmp  = args.out_path->value_ + vcl_string("max-gain") + fname_id;
  vcl_cout << "Writing " << fname_tmp << vcl_endl;
  mywrite_ascii(fname_tmp,max_gain);
  }
  {
  vcl_string fname_tmp  = args.out_path->value_ + vcl_string("med-gain") + fname_id;
  vcl_cout << "Writing " << fname_tmp << vcl_endl;
  mywrite_ascii(fname_tmp,med_gain);
  }
  {
  vcl_string fname_tmp  = args.out_path->value_ + vcl_string("avg-gain") + fname_id;
  vcl_cout << "Writing " << fname_tmp << vcl_endl;
  mywrite_ascii(fname_tmp,avg_gain);
  }
  return true;
}


//: \param[in] is_specified: just a vcl_vector that must be resized to n_v. It is not
// input nor output.
// \param[in] specified_pts: similar
//
//#define DEBUG 1
void mw_stereo_experiments_app::
gain_in_epipolar_for_exactly_nviews(
  unsigned n_v,
  vcl_vector<bool> &is_specified,
  vcl_vector<vsol_point_2d_sptr> &specified_pts,
  vcl_vector<vcl_vector<vsol_point_2d_sptr> > &vsols,
  vcl_vector< vcl_vector<dbecl_epiband *> > &epband,
  const vcl_vector <vsol_box_2d_sptr> &bbox,
  const vcl_vector< vcl_vector<vpgl_fundamental_matrix<double> > > &fm,
  double err_pos,
  vcl_vector<double> &gain
)
{
  for (unsigned iv=0 ; iv<n_v; ++iv) {
#ifdef DEBUG
    bool verbose=true;
    if (verbose)
      vcl_cout << "View FROM : " << iv << vcl_endl;
#endif
    is_specified[iv] = true;
    for (unsigned k_pts = 0; k_pts < vsols[iv].size(); ++k_pts) {
#ifdef DEBUG
      if (verbose)
        vcl_cout << "  pt #" << k_pts << vcl_endl;
#endif

      specified_pts[iv] = vsols[iv][k_pts];
      dbecl_epiband_builder::build_epibands_iteratively(
            true, iv,
            is_specified,
            specified_pts,
            epband,
            bbox,
            fm,
            err_pos);

      for (unsigned m=0; m < n_v; ++m) {
        if (m == iv)
          continue;


        dbecl_epiband epband_iv_m(vgl_box_2d<double>(bbox[m]->get_min_x(),bbox[m]->get_max_x(), bbox[m]->get_min_y(), bbox[m]->get_max_y()));

        epband_iv_m.compute(vsols[iv][k_pts]->get_p(), fm[iv][m], err_pos);

        double area_normal = epband_iv_m.area();

        double area_new = epband[m][m]->area();
#ifdef DEBUG
        if (verbose) {
          vcl_cout 
            << "      View TO: " << m
            << " area_normal: " << area_normal
            << " area_new: " << area_new ;
          vcl_cout.flush();
        }
#endif

        //: this could be an automated test for the system (using small
        //dataset):
        assert (area_new <= area_normal || mw_util::near_zero(area_new-area_normal,1e-8));
        double thisgain=-1;
        if (!mw_util::near_zero(area_normal,1e-10)) {
          thisgain = (area_normal - area_new) / area_normal;
          gain.push_back(thisgain);
        }

#ifdef DEBUG
        if (verbose) {
            vcl_cout << " gain: " << thisgain
            << vcl_endl;
        }
#endif
      }
    }
    is_specified[iv] = false;
  }
}

