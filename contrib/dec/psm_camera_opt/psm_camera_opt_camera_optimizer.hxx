#ifndef psm_camera_opt_camera_optimizer_txx_
#define psm_camera_opt_camera_optimizer_txx_

#include "psm_camera_opt_camera_optimizer.h" 

#include <psm/psm_apm_traits.h>
#include <psm/psm_scene.h>
#include <psm/algo/psm_render_expected.h>
#include <psm/algo/psm_compute_expected_depth.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_exp.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

// for computing homography
#include <vimt/vimt_transform_2d.h>
#include <dbvrl/dbvrl_world_roi.h>
#include <dbvrl/dbvrl_minimizer.h>
#include <brip/brip_vil_float_ops.h>


#include "psm_camera_opt_ekf_state.h"
#include "psm_camera_opt_homography_generator.h"
#include "psm_camera_opt_lm_direct_homography_generator.h"
#include "psm_camera_opt_gdbicp_homography_generator.h"


template<psm_apm_type APM>
psm_camera_opt_camera_optimizer<APM>::psm_camera_opt_camera_optimizer(double pos_var_predict, double rot_var_predict, 
                                                                      double homography_var, double homography_var_t, 
                                                                      bool use_black_background, bool use_proj_homography ) 
                                                                      : use_proj_homography_(use_proj_homography), use_black_background_(use_black_background)
{
  // fill in prediction error covariance matrix
  prediction_error_covar_ = vnl_matrix<double>(6,6,0.0);
  for (unsigned i=0; i<3; ++i)
    prediction_error_covar_(i,i) = pos_var_predict;
  for (unsigned i=3; i<6; ++i)
    prediction_error_covar_(i,i) = rot_var_predict;

  // fill in measurement error covariance matrix
  unsigned nh = 6;
  if (use_proj_homography) 
    nh =8;

  unsigned matrix_size = nh;

  measurement_error_covar_ = vnl_matrix<double>(matrix_size,matrix_size,0.0);
  for (unsigned i=0; i<2; ++i) 
    measurement_error_covar_(i,i) = homography_var_t;
  for (unsigned i=2; i<nh; ++i) 
    measurement_error_covar_(i,i) = homography_var;


  //homography_gen_ = new psm_camera_opt_lm_direct_homography_generator<obs_datatype>();
  //homography_gen_ = new psm_camera_opt_gdbicp_homography_generator<obs_datatype>();
}


template<psm_apm_type APM>
psm_camera_opt_camera_optimizer<APM>::~psm_camera_opt_camera_optimizer()
{
  //if (homography_gen_)
  //  delete homography_gen_;
}


template<psm_apm_type APM>
bool psm_camera_opt_camera_optimizer<APM>::optimize(psm_scene<APM> &scene, vil_image_view<obs_datatype> const& img, vpgl_perspective_camera<double> &camera)
{
 
  // create mask of all ones for image
  vil_image_view<float> mask(img.ni(), img.nj(),1);
  mask.fill(1.0f);

  // debug
  vcl_vector<vpgl_perspective_camera<double> > step_vec;
  step_vec.push_back(camera);

  vcl_cout << "computing plane estimate. " << vcl_endl;

  // estimate a plane using the expected depth image
  vil_image_view<float> expected_depth(img.ni(), img.nj());
  vil_image_view<float> vis_inf_prob(img.ni(), img.nj());
  psm_compute_expected_depth(scene, &camera, expected_depth, vis_inf_prob);
  // for each pixel compute the point location and fit a plane to collection.
  vgl_point_3d<double> cam_center(camera.get_camera_center());
  vcl_vector<vgl_homg_point_3d<double> > scene_points;

  for (unsigned int j=0; j<expected_depth.nj(); ++j) {
    for (unsigned int i=0; i<expected_depth.ni(); ++i) {
      // probably should be weighting based on prob, but a simple threshold will work for now.
      if (vis_inf_prob(i,j) < 0.5) {
        vgl_line_3d_2_points<double> cam_ray = camera.backproject(vgl_homg_point_2d<double>((double)i,(double)j));
        vgl_vector_3d<double> unit_ray = normalize(cam_ray.direction());
        vgl_homg_point_3d<double> pt(cam_center + unit_ray * expected_depth(i,j));
        scene_points.push_back(pt);
      }
    }
  }
  vcl_cout << "estimating plane using " << scene_points.size() << " points. " << vcl_endl;
  vgl_fit_plane_3d<double> plane_fit(scene_points);
  const double plane_fit_error_marg = 1e6; // allow a big error - points are probably not actually co-planar
  plane_fit.fit(plane_fit_error_marg);
  vgl_plane_3d<double> scene_plane(plane_fit.get_plane());

  vcl_cout << "estimated scene plane = " << scene_plane << vcl_endl;


  // iteratively update estimate, using expected images at intermediate steps as observations
  //psm_camera_opt_camera_optimizer substep_optimizer(1.0,1.0,0.0,0.0,0.0,0,false,false,false);

  const double t_scale = 0.1;
  psm_camera_opt_ekf_state ekf_state(0, t_scale, camera.camera_center(), camera.get_rotation(), vnl_vector_fixed<double,6>(0.0), prediction_error_covar_);

  unsigned max_iterations = 10;
  const double min_step_length = 0.00075;
  unsigned nits = 0;
  bool iterate_again = true;
  while(iterate_again) {
    if (nits++ > max_iterations)
      break;
    // generate expected image to use as observation
    vpgl_perspective_camera<double> step_cam(camera.get_calibration(), ekf_state.get_point(), ekf_state.get_rotation());
    
    vil_image_view<obs_datatype> expected_img(img.ni(), img.nj());
    vil_image_view<float> expected_img_mask(img.ni(), img.nj());

    // generate expected image
    psm_render_expected(scene, &step_cam, expected_img, expected_img_mask, use_black_background_);

    // we want to use pixels with low vis_inf probability only
    vil_image_view<float>::iterator mask_it = expected_img_mask.begin();
    for (; mask_it != expected_img_mask.end(); ++mask_it) {
      if (*mask_it > 0.5) {
        *mask_it = 0.0f;
      }
      else {
        *mask_it = 1.0f;
      }
    }

    // debug
    vil_save(expected_img,"c:/research/psm/output/step_expected.tiff");
    vil_save(expected_img_mask,"c:/research/psm/output/step_expected_mask.tiff");

    ekf_state = this->optimize_once(scene_plane, camera.get_calibration(), expected_img, expected_img_mask, img, ekf_state);

    step_vec.push_back(vpgl_perspective_camera<double>(camera.get_calibration(), ekf_state.get_point(), ekf_state.get_rotation()));

    const double step_length = ekf_state.get_state().magnitude();

    vcl_cout << " step length = " << step_length << vcl_endl;
    vcl_cout << "Pk = " << vcl_endl << ekf_state.get_error_covariance() << vcl_endl;
    if (step_length < min_step_length)
      iterate_again = false;
  }
#if 0
    // compute total step distance
    vnl_matrix_fixed<double,3,3> dR = 
      camera.get_rotation().inverse().as_matrix()*ekf_state.get_rotation().as_matrix();

    vnl_matrix_fixed<double,3,1> dC;
    dC(0,0) = camera.get_camera_center().x() - ekf_state.get_point().x();
    dC(1,0) = camera.get_camera_center().y() - ekf_state.get_point().y();
    dC(2,0) = camera.get_camera_center().z() - ekf_state.get_point().z();

    vnl_matrix_fixed<double,3,1> dT = ekf_state.get_rotation().as_matrix()*(dC);
    vnl_matrix_fixed<double,4,4> dE(0.0);
    dE.update(dR,0,0);
    dE.update(dT,0,3);
    dE(3,3) = 1.0;

    vnl_vector_fixed<double,6> total_step = matrix_to_coeffs_SE3(dE);
    // scale down translation coefficients
    total_step[0] *= step_state.t_scale();
    total_step[1] *= step_state.t_scale();
    total_step[2] *= step_state.t_scale();

    step_state.set_state(total_step);
    //step_state.set_error_covariance(substep_state.get_error_covariance());
    step_state.set_base_point(substep_state.get_point());
    step_state.set_base_rotation(substep_state.get_rotation());

  }
  // debug
  for (unsigned i=0; i<step_vec.size(); ++i) {
    vcl_cout << "step " << i << vcl_endl;
    vcl_cout << "center = " << step_vec[i].get_camera_center() << vcl_endl;
    vcl_cout << "rot = " << step_vec[i].get_rotation().as_rodrigues() << vcl_endl;
  }
#endif

  camera.set_camera_center(ekf_state.get_point());
  camera.set_rotation(ekf_state.get_rotation());

  return true;

}



template<psm_apm_type APM>
psm_camera_opt_ekf_state psm_camera_opt_camera_optimizer<APM>::optimize_once(vgl_plane_3d<double> plane_approx, vpgl_calibration_matrix<double> const& calibration_matrix,
                                                                                     vil_image_view<obs_datatype> const& expected_img, vil_image_view<float> const& expected_img_mask,
                                                                                     vil_image_view<obs_datatype> const& img, psm_camera_opt_ekf_state const& prev_state)
{
  // get the transformation from world coordinates to the previous camera coordinates
  vnl_matrix_fixed<double,3,3> R_prev = prev_state.get_rotation().as_matrix();
  vgl_point_3d<double> center_prev = prev_state.get_point();
  double t_scale = prev_state.t_scale();

  // Project the state ahead
  // assume constant velocity
  vnl_vector_fixed<double,6> x_pred = prev_state.get_state();
  //vnl_vector_fixed<double,6> x_pred(0.0);

  // Project the error covariance ahead
  vnl_matrix_fixed<double,6,6> P_pred = prev_state.get_error_covariance() + prediction_error_covar_;
  // vnl_matrix_fixed<double,6,6> P_pred = prediction_error_covar_;

  // transform the plane to camera coordinates
  vnl_vector_fixed<double,3> plane_normal(plane_approx.normal().x(), plane_approx.normal().y(),plane_approx.normal().z());
  vnl_vector_fixed<double,3> center_prev_v(center_prev.x(),center_prev.y(),center_prev.z());
  vnl_vector_fixed<double,3> plane_normal_cam = R_prev * plane_normal;
  double dist_cam = dot_product(center_prev_v,plane_normal) + plane_approx.d();
  vgl_plane_3d<double> plane_approx_cam(plane_normal_cam(0),plane_normal_cam(1),plane_normal_cam(2),dist_cam);
  // compute plane parameters theta,phi,and dz
  double dz = -plane_approx_cam.d() / plane_approx_cam.c();
  //double theta = acos(-plane_approx_cam.nz()/sqrt(plane_approx_cam.nx()*plane_approx_cam.nx() + plane_approx_cam.nz()*plane_approx_cam.nz()));
  double theta = atan2(plane_approx_cam.nx(),-plane_approx_cam.nz());
  //double phi = acos(-plane_approx_cam.nz()/sqrt(plane_approx_cam.ny()*plane_approx_cam.ny() + plane_approx_cam.nz()*plane_approx_cam.nz()));
  double phi = atan2(plane_approx_cam.ny(),-plane_approx_cam.nz());

  vcl_cout << "dz = " << dz << vcl_endl;
  vcl_cout << "theta = " << theta << vcl_endl;
  vcl_cout << "phi = " << phi << vcl_endl << vcl_endl;

  // construct the measurement Jacobian
  unsigned nhomography = 6 + (use_proj_homography_? 2:0);
  unsigned nmeasurements = nhomography;
  vnl_matrix<double> H(nmeasurements,6,0.0);

  vnl_matrix<double> J = SE3_to_H_Jacobian(theta,phi,dz*t_scale);
  H.update(J,0,0);

  // Compute the Kalman Gain
  vnl_matrix<double> H_trans = H.transpose();
  vnl_matrix<double> K = P_pred*H_trans*vnl_matrix_inverse<double>(H*P_pred*H_trans + measurement_error_covar_);

  vcl_cout << "H = " << H << vcl_endl;
  vcl_cout << "measurement_error_covar = " << measurement_error_covar_ << vcl_endl;

  // predict measurement vector z
  vnl_vector<double> z_pred(nmeasurements);

  // use J to generate predicted homography
  vnl_vector<double> zh_pred = J*x_pred.as_vector();
  z_pred.update(zh_pred,0);


  vnl_vector<double> zh = this->img_homography(img, expected_img, expected_img_mask, calibration_matrix.get_matrix(), use_proj_homography_);

  vnl_vector<double> z(nmeasurements);
  z.update(zh,0);

  // Update estimate with measurement zk
  vnl_vector_fixed<double,6> x_post = x_pred + K*(z - z_pred);

  vcl_cout << "K = " << K << vcl_endl;

  vcl_cout << "z_pred = " << z_pred << vcl_endl;
  vcl_cout << "z      = " << z << vcl_endl;
  vcl_cout << "x_pred = " << x_pred << vcl_endl;
  vcl_cout << "x_post = " << x_post << vcl_endl;


  // Update error covariance
  vnl_matrix<double> I6(6,6);
  I6.set_identity();
  vnl_matrix_fixed<double,6,6> P_post = (I6 - K*H)*P_pred;

  vcl_cout << "P_pred = " << P_pred << vcl_endl;
  vcl_cout << "P_post = " << P_post << vcl_endl;

  // update camera
  vnl_vector_fixed<double,6> x_post_unscaled;
  x_post_unscaled.update(x_post.extract(3,0)/t_scale, 0);
  x_post_unscaled.update(x_post.extract(3,3), 3);
  vnl_matrix_fixed<double,4,4> dRT = coeffs_to_matrix_SE3(x_post_unscaled);
  vnl_matrix_fixed<double,3,1> C_prev;
  C_prev(0,0) = center_prev.x();
  C_prev(1,0) = center_prev.y();
  C_prev(2,0) = center_prev.z();
  vnl_matrix_fixed<double,3,1> T_prev = -R_prev*C_prev;
  vnl_matrix_fixed<double,3,3> Rnew = dRT.extract(3,3)*R_prev;
  vnl_matrix_fixed<double,3,1> Tnew = dRT.extract(3,3,0,0)*T_prev + dRT.extract(3,1,0,3);
  vnl_matrix_fixed<double,3,1> Cnew = -Rnew.transpose()*Tnew;

  vgl_point_3d<double> curr_center(Cnew(0,0),Cnew(1,0),Cnew(2,0));
  vgl_rotation_3d<double> curr_rot(Rnew);

  //cam.set_camera_center(curr_center);
  //cam.set_rotation(curr_rot);

  // create new state
  psm_camera_opt_ekf_state curr_state(prev_state.k()+1, prev_state.t_scale(),
    curr_center, curr_rot, x_post, P_post);

  return curr_state;
}


template<psm_apm_type APM>
vnl_vector<double> psm_camera_opt_camera_optimizer<APM>::img_homography(vil_image_view<obs_datatype> const& base_img, vil_image_view<obs_datatype> const& img, vil_image_view<float> const& img_mask, vnl_matrix_fixed<double,3,3> K, bool projective)
{
  vil_image_view<float> base_img_view, img_view;


  // computed homography maps pixels in current image to pixels in base image
  psm_camera_opt_lm_direct_homography_generator<obs_datatype> homography_gen(base_img,img,img_mask);
  //homography_gen_->set_image0(base_img);
  //homography_gen_->set_image1(img);
  //homography_gen_->set_mask1(img_mask);
  //homography_gen_->set_projective(use_proj_homography_);
  vimt_transform_2d xform = homography_gen.compute_homography();

  // convert to normalized camera matrix 
  vnl_matrix<double> invK = vnl_matrix_inverse<double>(K);
  vnl_matrix<double> H = invK * xform.inverse().matrix() * K;

  // now extract Lie Generator coefficients
  vnl_vector<double> lie_vector;
  if (projective)
    lie_vector = matrix_to_coeffs_P2(H);
  else
    lie_vector = matrix_to_coeffs_GA2(H);

  vcl_cout << "optimized homography = " << vcl_endl << xform.inverse().matrix() << vcl_endl;
  vcl_cout << "normalized homography = "<< vcl_endl << H << vcl_endl;
  vcl_cout << "homography lie coeffs = " << lie_vector << vcl_endl << vcl_endl;

  return lie_vector;
}


template<psm_apm_type APM>
vnl_vector_fixed<double,6> psm_camera_opt_camera_optimizer<APM>::matrix_to_coeffs_SE3(vnl_matrix_fixed<double,4,4> const& M)
{
  vnl_matrix<double> logM(4,4);
  vnl_vector_fixed<double,6> coeffs;

  if(!logm_approx(M,logM)) {
    vcl_cout << "error converting matrix to lie coefficents.  matrix could be too far from Identity." << vcl_endl;
    coeffs.fill(0.0);
    return coeffs;
  }

  coeffs(0) = logM(0,3);
  coeffs(1) = logM(1,3);
  coeffs(2) = logM(2,3);
  coeffs(3) = logM(2,1);
  coeffs(4) = logM(0,2);
  coeffs(5) = logM(1,0);

  return coeffs;

}

template<psm_apm_type APM>
vnl_vector_fixed<double,6> psm_camera_opt_camera_optimizer<APM>::matrix_to_coeffs_GA2(vnl_matrix_fixed<double,3,3> const& M)
{
  vnl_matrix<double> logM(3,3);
  vnl_vector_fixed<double,6> coeffs;

  if(!logm_approx(M,logM)) {
    vcl_cout << "error converting matrix to lie coefficents.  matrix could be too far from Identity." << vcl_endl;
    coeffs.fill(0.0);
    return coeffs;
  }

  vcl_cout << "M = " << M << vcl_endl;
  vcl_cout << "logM = " << logM << vcl_endl;

  coeffs(0) = logM(0,2);
  coeffs(1) = logM(1,2);
  coeffs(2) = (-logM(0,1) + logM(1,0))/2.0;
  coeffs(3) = ( logM(0,0) + logM(1,1))/2.0;
  coeffs(4) = ( logM(0,0) - logM(1,1))/2.0;
  coeffs(5) = ( logM(0,1) + logM(1,0))/2.0;


  return coeffs;
}

template<psm_apm_type APM>
vnl_vector_fixed<double,8> psm_camera_opt_camera_optimizer<APM>::matrix_to_coeffs_P2(vnl_matrix_fixed<double,3,3> const& M)
{
  vnl_matrix<double> logM(3,3);
  vnl_vector_fixed<double,8> coeffs;

  if(!logm_approx(M,logM)) {
    vcl_cout << "error converting matrix to lie coefficents.  matrix could be too far from Identity." << vcl_endl;
    coeffs.fill(0.0);
    return coeffs;
  }

  coeffs(0) = logM(0,2);
  coeffs(1) = logM(1,2);
  coeffs(2) = (-logM(0,1) + logM(1,0))/2.0;
  coeffs(3) = ( logM(0,0) + logM(1,1))/2.0;
  coeffs(4) = ( logM(0,0) - logM(1,1))/2.0;
  coeffs(5) = ( logM(0,1) + logM(1,0))/2.0;

  // not sure if these are right -DC
  coeffs(6) = logM(2,0);
  coeffs(7) = logM(2,1);

  return coeffs;

}

template<psm_apm_type APM>
vnl_matrix_fixed<double,4,4> psm_camera_opt_camera_optimizer<APM>::coeffs_to_matrix_SE3(vnl_vector_fixed<double,6> const& a)
{
  vnl_matrix_fixed<double,4,4> A(0.0);
  // create Lie Algebra element A = |r t| where r is 3x3 skew-symmetric and t is a 3x1 vector.
  //                                |0 0|

  A(0,3) = a(0);
  A(1,3) = a(1);
  A(2,3) = a(2);
  A(1,2) = -a(3); A(2,1) =  a(3);
  A(0,2) =  a(4); A(2,0) = -a(4);
  A(0,1) = -a(5); A(1,0) =  a(5);

  vnl_matrix_fixed<double,4,4> M = vnl_matrix_exp< vnl_matrix_fixed<double,4,4> >(A);

  return M;
}

template<psm_apm_type APM>
vnl_matrix_fixed<double,3,3> psm_camera_opt_camera_optimizer<APM>::coeffs_to_matrix_GA2(vnl_vector_fixed<double,6> const& a)
{
  vcl_cout << "not implemented yet" << vcl_endl;
  return vnl_matrix_fixed<double,3,3>(0.0);

}

template<psm_apm_type APM>
vnl_matrix_fixed<double,3,3> psm_camera_opt_camera_optimizer<APM>::coeffs_to_matrix_P2(vnl_vector_fixed<double,8> const &a)
{
  vcl_cout << "not implemented yet" << vcl_endl;
  return vnl_matrix_fixed<double,3,3>(0.0);
}

template<psm_apm_type APM>
bool psm_camera_opt_camera_optimizer<APM>::logm_approx(vnl_matrix<double> const& A, vnl_matrix<double> &logA, double tol)
{
  unsigned max_iterations = 1000;

  unsigned nr = A.rows();
  unsigned nc = A.cols();
  if (nr != nc) {
    vcl_cout << "error: logm_approx called with non-square matrix." << vcl_endl;
    return false;
  }
  logA.set_size(nr,nr);
  logA.fill(0.0);

  vnl_matrix<double> I(nr,nr);
  I.set_identity();

  vnl_matrix<double> W = I - A;
  double term_norm = tol + 1.0;

  unsigned i = 1;
  vnl_matrix<double> Wpow = I;
  while(term_norm > tol) {
    if (i >= max_iterations) {
      vcl_cerr << vcl_endl;
      vcl_cerr << "*************************************************************" << vcl_endl;
      vcl_cerr << "ERROR: logm_approx did not converge." << vcl_endl;
      vcl_cerr << "*************************************************************" << vcl_endl << vcl_endl;
      return false;
    }
    Wpow = Wpow*W;
    vnl_matrix<double> term = -Wpow/i;
    term_norm = term.frobenius_norm();
    logA += term;
    //vcl_cout << "iteration " << i <<": W = " << W << vcl_endl << "Wpow = " << Wpow << vcl_endl << "term = " << term << vcl_endl;
    //vcl_cout << "logA = " << logA << vcl_endl;
    ++i;
  }
  vcl_cout << "logM converged in " << i << " iterations. " << vcl_endl;

  return true;
}

template<psm_apm_type APM>
vnl_matrix<double> psm_camera_opt_camera_optimizer<APM>::SE3_to_H_Jacobian(double plane_theta, double plane_phi, double plane_dz)
{
  vnl_matrix<double> J;

  if (use_proj_homography_) 
    J.set_size(8,6);
  else
    J.set_size(6,6);

  J.fill(0.0);

  double inv_dz = 1.0/plane_dz;
  double t_phi = tan(plane_phi) / (2*plane_dz);
  double t_theta = tan(plane_theta) / (2*plane_dz);

  J(0,0) = inv_dz;                                                       J(0,4) = 1.0;
  J(1,1) = inv_dz;                     J(1,3) = -1.0;
  J(2,0) = t_phi;    J(2,1) = -t_theta;                                                J(2,5) = 1.0;
  J(3,0) = -t_theta; J(3,1) = -t_phi;    J(3,2) = -inv_dz;
  J(4,0) = -t_theta; J(4,1) = t_phi;
  J(5,0) = -t_phi;   J(5,1) = -t_theta;
  if (use_proj_homography_) {
    J(6,2) = t_phi*2;    J(6,3) = -1.0;
    J(7,2) = t_theta*2;  J(7,4) = 1.0;
  }

  return J;

}

template<psm_apm_type APM>
vnl_matrix<double> psm_camera_opt_camera_optimizer<APM>::H_to_SE3_Jacobian(double plane_theta, double plane_phi, double plane_dz)
{

  vnl_matrix<double> J = SE3_to_H_Jacobian(plane_theta,plane_phi,plane_dz);
  vnl_matrix<double> invJ = vnl_matrix_inverse<double>(J);

  return invJ;

}

#define PSM_CAMERA_OPT_CAMERA_OPTIMIZER_INSTANTIATE(T) \
template class psm_camera_opt_camera_optimizer<T >


#endif

