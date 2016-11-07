#include <testlib/testlib_test.h>

#include <vcl_limits.h>
#include <vcl_vector.h>
#include <mw/mw_util.h>
#include <dbdif/dbdif_camera.h>
#include <dbdif/dbdif_rig.h>
#include <dbdif/algo/dbdif_data.h>
#include <vnl/vnl_double_3x3.h>
#include <vcl_algorithm.h>

static const double tolerance=vcl_numeric_limits<double>::epsilon()*100;

static void 
test_reprojection_errors(
    const vcl_vector<dbdif_camera> &cam_gt_,  //:< ideal cams
    const vpgl_calibration_matrix<double> &K,
    const vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > &crv3d);

static void 
compute_and_print_info(
    const vcl_vector<double> &err_v,
    const vcl_vector<unsigned> &valid_idx,
    vcl_vector<double> &max_err_v,
    vcl_vector<double> &min_err_v,
    vcl_vector<double> &mean_err_v,
    vcl_vector<double> &median_err_v,
    double err_tolerance,
    const dbdif_rig &rig,
    const vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > &crv2d_gt_
    );

//: tests multiview projection + reconstruction of differential geometry
MAIN( test_reprojection_errors )
{
  START ("Multiview reprojection errors");

    //: ideal cams
  vcl_vector<dbdif_camera> cam_gt_;
  unsigned nviews_=3;
  cam_gt_.resize(nviews_);

  { // ---------- Digital Camera Setup
    vcl_cout << "\n\n\n ========== TESTING FOR DIGITAL CAMERA TURNTABLE ========== \n\n\n";

  unsigned  crop_origin_x_ = 450;
  unsigned  crop_origin_y_ = 1750;
  double x_max_scaled = 500;

  vnl_double_3x3 Kmatrix;
  dbdif_turntable::internal_calib_olympus(Kmatrix, x_max_scaled, crop_origin_x_, crop_origin_y_);

  vpgl_calibration_matrix<double> K(Kmatrix);

  vpgl_perspective_camera<double> *P;

  P = dbdif_turntable::camera_olympus(0, K);
  cam_gt_[0].set_p(*P);
  P = dbdif_turntable::camera_olympus(5, K);
  cam_gt_[1].set_p(*P);
  P = dbdif_turntable::camera_olympus(60, K);
  cam_gt_[2].set_p(*P);

//  P = dbdif_turntable::camera_olympus(0, K);
//  cam_gt_[0].set_p(*P);
//  P = dbdif_turntable::camera_olympus(60, K);
//  cam_gt_[1].set_p(*P);
//  P = dbdif_turntable::camera_olympus(5, K);
//  cam_gt_[2].set_p(*P);

  //unused unsigned  npts=0;
  vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > crv3d;
  dbdif_data::space_curves_olympus_turntable( crv3d );


  test_reprojection_errors(cam_gt_, K, crv3d);

  }

  SUMMARY();
}

void 
test_reprojection_errors(
    const vcl_vector<dbdif_camera> &cam_gt_,  //:< ideal cams
    const vpgl_calibration_matrix<double> &K,
    const vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > &crv3d)
{

  vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > crv2d_gt_;
  dbdif_data::project_into_cams(crv3d, cam_gt_, crv2d_gt_);


  { //: first, distance to reprojection using ideal cameras should be low for all non-degenerate points:

    double err_pos, err_t, err_k, err_kdot;
    unsigned i_pos, i_t, i_k, i_kdot;
    unsigned  nvalid;

    vcl_cout << "\nErrors of reprojection using IDEAL cams\n";
    dbdif_rig rig(cam_gt_[0].Pr_, cam_gt_[1].Pr_);
    
    dbdif_data::max_err_reproj_perturb(crv2d_gt_, cam_gt_, rig, err_pos,err_t,err_k,err_kdot, i_pos, i_t, i_k, i_kdot, nvalid);

    vcl_cout 
      << "MAX: err_pos = " << err_pos  << "(" << i_pos 
      << "); err_t = " << err_t << "(" << i_t 
      << "); err_k = " << err_k << "(" << i_k 
      << "); err_kdot = " << err_kdot << "(" << i_kdot
      << ")"<<vcl_endl;

    vcl_cout << "Valid points: " << 100.0*(double)nvalid/(double)crv2d_gt_[0].size() << "% (" << nvalid << "/" << crv2d_gt_[0].size() << ")\n";
    TEST_NEAR("err_pos ideal cams",err_pos,0,1e-5);
    TEST_NEAR("err_t ideal cams",err_t,0,1e-5);
    TEST_NEAR("err_k ideal cams",err_k,0,1e-5);
    TEST_NEAR("err_kdot ideal cams",err_kdot,0,1e-3);
  }

  // PERTURBED CAMS
  
  vcl_vector<dbdif_camera> cam_;
  cam_.resize(3);

  vcl_vector<double> perturb_angle_v, 
    max_err_pos_v, max_err_t_v, max_err_k_v, max_err_kdot_v,
    min_err_pos_v, min_err_t_v, min_err_k_v, min_err_kdot_v,
    mean_err_pos_v, mean_err_t_v, mean_err_k_v, mean_err_kdot_v,
    median_err_pos_v, median_err_t_v, median_err_k_v, median_err_kdot_v;

//    vcl_cout << "\nErrors of reprojection using PERTURBED cams\n";
//  const double dtheta=1; // deg
  for (double dtheta=0; dtheta <=1; dtheta+=0.01) {
    perturb_angle_v.push_back(dtheta);
//  /*shortbaseline:*/
    const double angle1_perturb = 0 +dtheta;
    const double angle2_perturb = 5 -dtheta;
    const double angle3_perturb = 60+dtheta;
//  /*widebaseline:*/
//    const double angle1_perturb = 0 +dtheta;
//    const double angle2_perturb = 60 -dtheta;
//      const double angle3_perturb = 5+dtheta;

    vpgl_perspective_camera<double> *P;
    // Using simple perturb criteria for now
    P = dbdif_turntable::camera_olympus(angle1_perturb, K);
    cam_[0].set_p(*P); delete P;
    P = dbdif_turntable::camera_olympus(angle2_perturb, K);
    cam_[1].set_p(*P); delete P;
    P = dbdif_turntable::camera_olympus(angle3_perturb, K);
    cam_[2].set_p(*P); delete P;

    // Second, distance to reprojection using perturbed cameras 
    dbdif_rig rig(cam_[0].Pr_, cam_[1].Pr_);
    
    vcl_vector<double> err_pos_sq_v;
    vcl_vector<double> err_t_v;
    vcl_vector<double> err_k_v;
    vcl_vector<double> err_kdot_v;
    vcl_vector<unsigned> valid_idx;

    vcl_cout << "\nPerturbation angle:" << dtheta << " deg\n";

    dbdif_data::err_reproj_perturb(crv2d_gt_, cam_, rig, err_pos_sq_v, err_t_v, err_k_v, err_kdot_v, valid_idx);

    for (unsigned i=0; i < err_pos_sq_v.size(); ++i)
      err_pos_sq_v[i] = vcl_sqrt(err_pos_sq_v[i]);

    const double pos_tolerance = 1.0;

    vcl_cout << " Pos: ";
    compute_and_print_info(
        err_pos_sq_v,
        valid_idx,
        max_err_pos_v,
        min_err_pos_v,
        mean_err_pos_v,
        median_err_pos_v,
        pos_tolerance, rig, crv2d_gt_);

    const double t_tolerance = (1.0/180.0)*vnl_math::pi;
    vcl_cout << "   T: ";
    compute_and_print_info(
        err_t_v,
        valid_idx,
        max_err_t_v,
        min_err_t_v,
        mean_err_t_v,
        median_err_t_v,
        t_tolerance, rig, crv2d_gt_);

    const double k_tolerance = 0.01;
    vcl_cout << "   K: ";
    compute_and_print_info(
        err_k_v,
        valid_idx,
        max_err_k_v,
        min_err_k_v,
        mean_err_k_v,
        median_err_k_v,
        k_tolerance, rig, crv2d_gt_);

    const double kdot_tolerance = 0.01;
    vcl_cout << "Kdot: ";
    compute_and_print_info(
        err_kdot_v,
        valid_idx,
        max_err_kdot_v,
        min_err_kdot_v,
        mean_err_kdot_v,
        median_err_kdot_v,
        kdot_tolerance, rig, crv2d_gt_);


//    if (perturb_angle_v.size() % 10 == 0) {
      vcl_cout << "Valid points: " << 100.0*(double)valid_idx.size()/(double)crv2d_gt_[0].size() << "% (" << valid_idx.size() << "/" << crv2d_gt_[0].size() << ")\n";
//    }

  }

  // Write to files
  vcl_string path("../app/dat/synthetic-stereo/perturb/reprojection-precision-tests/");
  mywrite(path+vcl_string("perturb_angle_v.dat"), perturb_angle_v);
  mywrite(path+vcl_string("mean_err_pos_v.dat"), mean_err_pos_v);
  mywrite(path+vcl_string("mean_err_t_v.dat"), mean_err_t_v);
  mywrite(path+vcl_string("mean_err_k_v.dat"), mean_err_k_v);
  mywrite(path+vcl_string("mean_err_kdot_v.dat"), mean_err_kdot_v);

  mywrite(path+vcl_string("max_err_pos_v.dat"), max_err_pos_v);
  mywrite(path+vcl_string("max_err_t_v.dat"), max_err_t_v);
  mywrite(path+vcl_string("max_err_k_v.dat"), max_err_k_v);
  mywrite(path+vcl_string("max_err_kdot_v.dat"), max_err_kdot_v);
}

// Position
void 
compute_and_print_info(
    const vcl_vector<double> &err_v,
    const vcl_vector<unsigned> &valid_idx,
    vcl_vector<double> &max_err_v,
    vcl_vector<double> &min_err_v,
    vcl_vector<double> &mean_err_v,
    vcl_vector<double> &median_err_v,
    double err_tolerance,
    const dbdif_rig &rig,
    const vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > &crv2d_gt_
    )
{

  unsigned max_idx, min_idx;
  double err;

  err = mw_util::max(err_v,max_idx); 
  max_idx = valid_idx[max_idx];
  max_err_v.push_back(err);

  err = mw_util::min(err_v,min_idx); 
  min_idx = valid_idx[min_idx];
  min_err_v.push_back(err);

  err = mw_util::mean(err_v); 
  mean_err_v.push_back(err);

  err = mw_util::median(err_v); 
  median_err_v.push_back(err);

  double n_err=0;
  double max_epi_angle=-1;
  unsigned idx_max_epi_angle;
  double n_err_epi=0;
  const double epipolar_angle_thresh = vnl_math::pi/12;
  for (unsigned i=0; i < err_v.size(); ++i) {
    assert(err_v[i] >= 0);
    if (vcl_fabs(err_v[i]) > err_tolerance ) {
      ++n_err;

      const dbdif_3rd_order_point_2d &p1 = crv2d_gt_[0][valid_idx[i]];

      double epipolar_angle = dbdif_rig::angle_with_epipolar_line(p1.t,p1.gama,rig.f12);

      if (epipolar_angle > max_epi_angle) {
        max_epi_angle = epipolar_angle;
        idx_max_epi_angle = valid_idx[i];
      }

      if (epipolar_angle < epipolar_angle_thresh)
        n_err_epi++;
    }
  }
  max_epi_angle *= 180.0/vnl_math::pi;

  vcl_cout 
    << "n_err (> " << err_tolerance << "): "<< n_err << "(" << 100.0*(double)n_err/(double)err_v.size() << "%)\t"
    << "n_err_epi: " << n_err_epi;

  if (n_err_epi > 0) {
    vcl_cout  << "(" << 100.0*(double)n_err_epi/(double)n_err << "%) max_epi: " << max_epi_angle << "deg," << idx_max_epi_angle;
  }

  vcl_cout << "\t";

  vcl_cout 
    << "max: " << max_err_v.back() << "(" << max_idx << ")\t"
    << "min: " << min_err_v.back() << "(" << min_idx << ")\t"
    << "mean: " << mean_err_v.back() << "\t"
    << "median: " << median_err_v.back() 
    << vcl_endl; 
}
