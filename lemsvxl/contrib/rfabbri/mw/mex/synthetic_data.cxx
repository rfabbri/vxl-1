#include <vcl_iostream.h>
#include <vcl_streambuf.h>
#include <math.h>
#include <dbdif/dbdif_analytic.h>
#include <dbdif/algo/dbdif_data.h>
#include <mw/mw_util.h>
#include <mw/algo/mw_stereo_app.h>

#include "mex.h"

extern void _main();

static
void mymex(
      mxArray **p_pts0,
      mxArray **p_pts1,
      mxArray **p_tgts0,
      mxArray **p_tgts1,
      mxArray **p_fmatrix,
      mxArray **p_proj0,
      mxArray **p_proj1,
      mxArray **p_pts3d,
      mxArray **p_tgts3d,
      vcl_string &angles,
      double yaw,
      bool do_occl
      )
{
  // Initialize app

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

  args.synth_data_med->value_ = true;
  char *a = (char *) malloc(angles.length()*sizeof(char));
  strcpy(a,angles.c_str());
  args.angles->parse(&a);

  mw_stereo_app app;
  if (!app.init(args)) {
    mexErrMsgTxt("Problem with initializing internal app args\n");
    return;
  }

  if ( !(app.vsols_.size() == app.nviews_ 
        && app.nviews_ == 2 
        && app.vsols_[0].size() == app.vsols_[1].size()) )
    mexErrMsgTxt("Inconsistent error being returned from internal app\n");

  vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > data_pts_occl(app.nviews_);
  if (do_occl) {
    // Generate and project the occluding contours in 2 views.

    double radius = 10;
    dbdif_vector_3d s0;
    s0[0] = 0; s0[1] = 0; s0[2] = 0;

    for (unsigned v=0; v < app.nviews_; ++v) {
      const dbdif_vector_3d &c = app.cam_gt_[v].c;
      
      vcl_vector<dbdif_3rd_order_point_3d> crv3d;
      dbdif_vector_3d Gamma_center;
      double Gamma_radius;

      dbdif_analytic::sphere_occluding_contour(radius, s0, c, crv3d, Gamma_center, Gamma_radius);

      // we need to regenerate the curve for each view; we can't just
      // project crv3d to all cams at once.
      vcl_vector<dbdif_camera> cam;
      cam.push_back(app.cam_gt_[v]);

      vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > xi; //:< image coordinates
      dbdif_data::project_into_cams( crv3d, cam, xi); 

      data_pts_occl[v] = xi[0];
    }
  }

  unsigned npts_fixed = app.vsols_[0].size();

  unsigned npts_2d = npts_fixed;
  if (do_occl)
    npts_2d += data_pts_occl[0].size();

  // Points in Matlab to be returned
  mxArray *mx_pts0 = mxCreateDoubleMatrix(npts_2d,2,mxREAL);
  double *pts0 = mxGetPr(mx_pts0);

  mxArray *mx_pts1 = mxCreateDoubleMatrix(npts_2d,2,mxREAL);
  double *pts1 = mxGetPr(mx_pts1);

  mxArray *mx_tgts0 = mxCreateDoubleMatrix(npts_2d,1,mxREAL);
  double *tgts0 = mxGetPr(mx_tgts0);

  mxArray *mx_tgts1 = mxCreateDoubleMatrix(npts_2d,1,mxREAL);
  double *tgts1 = mxGetPr(mx_tgts1);
  
  //: the fixed contours
  for (unsigned i=0; i < npts_fixed; ++i) {
    pts0[i] = app.vsols_[0][i]->x();
    pts0[i+npts_2d] = app.vsols_[0][i]->y();

    pts1[i] = app.vsols_[1][i]->x();
    pts1[i+npts_2d] = app.vsols_[1][i]->y();

    tgts0[i] = vcl_atan2(app.crv2d_gt_[0][i].t[1], app.crv2d_gt_[0][i].t[0]);
    tgts1[i] = vcl_atan2(app.crv2d_gt_[1][i].t[1], app.crv2d_gt_[1][i].t[0]);
  }

  //: the occluding contours
  if (do_occl) {
    for (unsigned i=0; i + npts_fixed < npts_2d; ++i) {
      pts0[i + npts_fixed] = data_pts_occl[0][i].gama[0];
      pts0[i+npts_2d + npts_fixed] =  data_pts_occl[0][i].gama[1];

      pts1[i + npts_fixed] = data_pts_occl[1][i].gama[0];
      pts1[i+npts_2d + npts_fixed] =  data_pts_occl[1][i].gama[1];

      tgts0[i + npts_fixed] = vcl_atan2(data_pts_occl[0][i].t[1], data_pts_occl[0][i].t[0]);
      tgts1[i + npts_fixed] = vcl_atan2(data_pts_occl[1][i].t[1], data_pts_occl[1][i].t[0]);
    }
  }

  mxArray *mx_pts3d = mxCreateDoubleMatrix(npts_fixed,3,mxREAL);
  double *pts3d = mxGetPr(mx_pts3d);

  mxArray *mx_tgts3d = mxCreateDoubleMatrix(npts_fixed,3,mxREAL);
  double *tgts3d = mxGetPr(mx_tgts3d);

  unsigned is=0;
  for (unsigned ic=0; ic < app.crv3d_gt_.size(); ++ic) {
    for (unsigned ip=0; ip <app.crv3d_gt_[ic].size() ; ++ip) {
      pts3d[is] = app.crv3d_gt_[ic][ip].Gama[0];
      pts3d[is+npts_fixed] = app.crv3d_gt_[ic][ip].Gama[1];
      pts3d[is+2*npts_fixed] = app.crv3d_gt_[ic][ip].Gama[2];

      tgts3d[is] = app.crv3d_gt_[ic][ip].T[0];
      tgts3d[is+npts_fixed] = app.crv3d_gt_[ic][ip].T[1];
      tgts3d[is+2*npts_fixed] = app.crv3d_gt_[ic][ip].T[2];
      ++is;
    }
  }

  // Fundamental matrix and projection matrices

  mxArray *mx_fm = mxCreateDoubleMatrix(3,3,mxREAL);
  double *p_fm = mxGetPr(mx_fm);

  mxArray *mx_pr0 = mxCreateDoubleMatrix(3,4,mxREAL);
  double *p_pr0 = mxGetPr(mx_pr0);

  mxArray *mx_pr1 = mxCreateDoubleMatrix(3,4,mxREAL);
  double *p_pr1 = mxGetPr(mx_pr1);

  vpgl_fundamental_matrix <double> fm;
  vpgl_perspective_camera<double> pr0;
  vpgl_perspective_camera<double> pr1;
  if (yaw != 0) {
    vcl_cout << "yaw != 0" << vcl_endl;
    // create rotation around x-axis 
    pr0 = app.cam_gt_[0].Pr_;
    //  double yaw = -13.0*vnl_math::pi/180.0;
    vgl_rotation_3d<double> rot(yaw,0,0);
    vgl_rotation_3d<double> rot_result = rot*pr0.get_rotation();
    pr0.set_rotation(rot_result);

    pr1 = app.cam_gt_[1].Pr_;
    vgl_rotation_3d<double> rot_result1 = rot*pr1.get_rotation();
    pr1.set_rotation(rot_result);
  } else {
    vcl_cout << "yaw == 0" << vcl_endl;
    pr0 = app.cam_gt_[0].Pr_;
    pr1 = app.cam_gt_[1].Pr_;
  }

  fm = vpgl_fundamental_matrix <double> (pr0, pr1);

  vcl_cout << "Fundamental Matrix from Ground Truth \n" << fm.get_matrix() << vcl_endl;
  // Write the fundamental matrix to matlab
  //mexPrintf("Fm: ");
  for (unsigned i=0; i < 9; ++i) {
    //mexPrintf("%g\t",fm.get_matrix())
    p_fm[i] = fm.get_matrix()(i%3,i/3);
  }

  vcl_cout << "Proj matrix 0\n" << pr0.get_matrix() << vcl_endl;
  vcl_cout << "Proj matrix 1\n" << pr1.get_matrix() << vcl_endl;
  for (unsigned i=0; i < 12; ++i) {
    p_pr0[i] = pr0.get_matrix()(i%3,i/3);
    p_pr1[i] = pr1.get_matrix()(i%3,i/3);
  }

  *p_pts0 = mx_pts0;
  *p_pts1 = mx_pts1;
  *p_tgts0 = mx_tgts0;
  *p_tgts1 = mx_tgts1;
  *p_fmatrix = mx_fm;
  *p_proj0 = mx_pr0;
  *p_proj1 = mx_pr1;

  *p_pts3d = mx_pts3d;
  *p_tgts3d = mx_tgts3d;

  return;
}

//
//  [pts0,pts1, fmatrix, proj0, proj1, tgts0, tgts1, pts3d, tgts3d] = synthetic_data(angles_s);  
//  [pts0,pts1, fmatrix, proj0, proj1, tgts0, tgts1, pts3d, tgts3d] = synthetic_data(angles_s, yaw);  
//  [pts0,pts1, fmatrix, proj0, proj1, tgts0, tgts1, pts3d, tgts3d] = synthetic_data(angles_s, yaw, do_occl);  
//
//  angles_s: comma-separated list of angles 
//  yaw: add some yaw to the cameras in order to cause a shift the image.
//  defaults to 0
//
//  do_occl: if non-zero, add some occluding contours to the 2D points. Note
//  that, for this case, no 3D points are appended. The corresponding
//  projections for the 3D points pts3d(i,:) are still just pts0(i,:), 
//  but any additional points in pts0 and pts1 beyond size(pts3d,1) are
//  projections of occluding contours.
// 
void mexFunction(
     int          nlhs,
     mxArray      *plhs[],
     int          nrhs,
     const mxArray *prhs[]
     )
{
  vcl_string angles;
  double yaw = 0;
  bool do_occl = false;

  if (nrhs > 3) {
    mexErrMsgTxt("Too many input arguments");
  }

  if (nrhs == 0) {
    angles = vcl_string("30,60");
  } else {
    mxClassID category = mxGetClassID(prhs[0]);
    if (category != mxCHAR_CLASS)
      mexErrMsgTxt("First argument must be a string of comma-separated angles.\n");

    char *buf;
    mwSize buflen = mxGetNumberOfElements(prhs[0]) + 1;
    buf = (char *) mxCalloc(buflen, sizeof(char));

    if (mxGetString(prhs[0], buf, buflen) != 0)
      mexErrMsgTxt("Could not convert string data.\n");

    angles = vcl_string(buf);
    if (nrhs >= 2) {
      mwSize arg2_nrows = mxGetM(prhs[1]);
      mwSize arg2_ncols = mxGetN(prhs[1]);
      if (arg2_nrows!= 1 || arg2_ncols != 1 || !mxIsDouble(prhs[1]) || mxIsComplex(prhs[1]))
        mexErrMsgTxt("Input 2 must be noncomplex scalar.\n");
      yaw = *(mxGetPr(prhs[1]));

      if (nrhs == 3) {
        mwSize arg3_nrows = mxGetM(prhs[1]);
        mwSize arg3_ncols = mxGetN(prhs[1]);
        if (arg3_nrows!= 1 || arg3_ncols != 1 || !mxIsDouble(prhs[2]) || mxIsComplex(prhs[2]))
          mexErrMsgTxt("Input 3 must be noncomplex scalar.\n");
        double do_occl_dbl = *(mxGetPr(prhs[2]));
        if (do_occl_dbl) {
          do_occl = true;
          vcl_cout << "Doing occluding contours\n";
        }
      }
    }
  }

  if (nlhs > 9) {
    mexErrMsgTxt("Too many output arguments");
  }

  vcl_cout.sync_with_stdio(true);
  vcl_cerr.sync_with_stdio(true);

  vcl_streambuf* cout_sbuf = vcl_cout.rdbuf();
  vcl_stringbuf myout_sbuf;
  vcl_cout.rdbuf(&myout_sbuf);

  vcl_streambuf* cerr_sbuf = vcl_cerr.rdbuf();
  vcl_stringbuf myerr_sbuf;
  vcl_cerr.rdbuf(&myerr_sbuf);

  mymex(&(plhs[0]),&(plhs[1]),
        &(plhs[5]),&(plhs[6]),
        &(plhs[2]),&(plhs[3]),&(plhs[4]), 
        &(plhs[7]), &(plhs[8]), 
        angles,yaw, do_occl);

  mexPrintf("cout_contents: \n%s\n",myout_sbuf.str().c_str());
  mexPrintf("cerr_contents: \n%s\n",myerr_sbuf.str().c_str());

  vcl_cout.rdbuf(cout_sbuf);
  vcl_cerr.rdbuf(cerr_sbuf);
  vcl_flush(vcl_cout);
  vcl_flush(vcl_cerr);

  return;
}
