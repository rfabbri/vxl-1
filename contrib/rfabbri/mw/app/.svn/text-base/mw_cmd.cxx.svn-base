#include <mw/mw_util.h>
#include <vul/vul_arg.h>
#include <dbdif/dbdif_rig.h>
#include <dbdif/dbdif_analytic.h>
#include <dbdif/algo/dbdif_data.h>

static void 
calib_perturb_minimizing_reprojection_errors(
    const vcl_vector<dbdif_camera> &cam_gt_,  //:< ideal cams
    const vpgl_calibration_matrix<double> &K,
    const vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > &crv3d);

//: This program accomodates any experimentation I wanna do.
int
main(int /*argc*/, char ** /*argv*/)
{
  vcl_vector<dbdif_camera> cam_gt_;
  unsigned nviews_=3;
  cam_gt_.resize(nviews_);

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

  vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > crv3d;
  dbdif_data::space_curves_olympus_turntable( crv3d );

  calib_perturb_minimizing_reprojection_errors(cam_gt_, K, crv3d);

  return 0;
}

//: An experiment just to measure how much a perturbation of cameras affect the projected
// measurements of tangent, curvature, and curvature derivative.
void 
calib_perturb_minimizing_reprojection_errors(
    const vcl_vector<dbdif_camera> &cam_gt_,  //:< ideal cams
    const vpgl_calibration_matrix<double> &K,
    const vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > &crv3d)
{
  vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > crv2d_gt_;
  dbdif_data::project_into_cams(crv3d, cam_gt_, crv2d_gt_);

  vcl_vector<dbdif_camera> cam_;
  cam_.resize(3);

  double err_pos, err_t, err_k, err_kdot;
  unsigned i_pos, i_t, i_k, i_kdot;
  unsigned  nvalid;

  double min_err_k, min_angle_err_k,
         min_err_t, min_angle_err_t,
         min_err_kdot, min_angle_err_kdot,
         min_err_pos, min_angle_err_pos;

  min_err_pos = min_err_k = min_err_t = min_err_kdot = vcl_numeric_limits<double>::infinity();

  for (double dtheta=0.0001; dtheta <30; dtheta += 0.01) {
    const double angle1_perturb = 0 +dtheta;
    const double angle2_perturb = 5 -dtheta;
    const double angle3_perturb = 60+dtheta;

    vpgl_perspective_camera<double> *P;
    // Using simple perturb criteria for now
    P = dbdif_turntable::camera_olympus(angle1_perturb, K);
    cam_[0].set_p(*P); delete P;
    P = dbdif_turntable::camera_olympus(angle2_perturb, K);
    cam_[1].set_p(*P); delete P;
    P = dbdif_turntable::camera_olympus(angle3_perturb, K);
    cam_[2].set_p(*P); delete P;

    //: second, distance to reprojection using perturbed cameras 
    //    vcl_cout << "\nErrors of reprojection using PERTURBED cams\n";
    dbdif_rig rig(cam_[0].Pr_, cam_[1].Pr_);
    
    dbdif_data::max_err_reproj_perturb(crv2d_gt_, cam_, rig, err_pos,err_t,err_k,
        err_kdot, i_pos, i_t, i_k, i_kdot, nvalid);

    if (err_pos < min_err_pos) {
      min_err_pos = err_pos;
      min_angle_err_pos = dtheta;
    }

    if (err_t < min_err_t) {
      min_err_t = err_t;
      min_angle_err_t = dtheta;
    }

    if (err_k < min_err_k) {
      min_err_k = err_k;
      min_angle_err_k = dtheta;
    }

    if (err_kdot < min_err_kdot) {
      min_err_kdot = err_kdot;
      min_angle_err_kdot = dtheta;
    }
    if ( (unsigned)(dtheta/0.025) % 10 == 0 || vcl_fabs(dtheta-30) < 2*dtheta ) {
      vcl_cout << "\nperturbation angle (deg):" << dtheta << vcl_endl;
      vcl_cout << "min_err_pos: " << min_err_pos <<  "\tangle: " << min_angle_err_pos << vcl_endl
      << "min_err_t: " << min_err_t <<  "\tangle: " << min_angle_err_t << vcl_endl
      << "min_err_k: " << min_err_k <<  "\tangle: " << min_angle_err_k << vcl_endl
      << "min_err_kdot: " << min_err_kdot <<  "\tangle: " << min_angle_err_kdot << vcl_endl;
    }
  }

  vcl_cout << "-----------------------  FINAL RESULTS  ------------------------------------\n"
    << "min_err_pos: " << min_err_pos <<  "\tangle: " << min_angle_err_pos << vcl_endl
    << "min_err_t: " << min_err_t <<  "\tangle: " << min_angle_err_t << vcl_endl
    << "min_err_k: " << min_err_k <<  "\tangle: " << min_angle_err_k << vcl_endl
    << "min_err_kdot: " << min_err_kdot <<  "\tangle: " << min_angle_err_kdot << vcl_endl;
}
