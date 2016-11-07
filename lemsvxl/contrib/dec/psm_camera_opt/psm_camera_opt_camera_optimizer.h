#ifndef psm_camera_opt_camera_optimizer_h_
#define psm_camera_opt_camera_optimizer_h_

//:
// \file
// \brief // Camera optimizer using Extended Kalman Filter
//           
// \author Daniel Crispell
// \date 03/01/08
// \verbatim
// Modifications
// 03/25/08 dec  moved to contrib/dec/psm
// \endverbatim


#include <psm/psm_scene.h>
#include <psm/psm_apm_traits.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view_base.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include "psm_camera_opt_ekf_state.h"
#include "psm_camera_opt_homography_generator.h"

//: Container for algorithms related to optimizing a perspective camera's pose based a voxel grid
template <psm_apm_type APM>
class psm_camera_opt_camera_optimizer
{
  
public:
  typedef typename psm_apm_traits<APM>::obs_datatype obs_datatype;

  psm_camera_opt_camera_optimizer(double pos_var_predict, double rot_var_predict, 
    double homography_var, double homography_var_t, 
    bool use_black_background=false, bool use_proj_homography=false );


  ~psm_camera_opt_camera_optimizer();

  bool optimize(psm_scene<APM> &scene, vil_image_view<obs_datatype> const& img, vpgl_perspective_camera<double> &camera);



protected:
  psm_camera_opt_camera_optimizer() {}

psm_camera_opt_ekf_state optimize_once(vgl_plane_3d<double> plane_approx, vpgl_calibration_matrix<double> const& camera_cal,
                                       vil_image_view<obs_datatype> const& expected_img, vil_image_view<float> const& expected_img_mask,
                                       vil_image_view<obs_datatype> const& img, psm_camera_opt_ekf_state const& prev_state);

private:

  bool use_proj_homography_;
  bool use_black_background_;

  vnl_matrix<double> prediction_error_covar_;
  vnl_matrix<double> measurement_error_covar_;

  vnl_vector<double> psm_camera_opt_camera_optimizer<APM>::img_homography(vil_image_view<obs_datatype> const& base_img, vil_image_view<obs_datatype> const& img, vil_image_view<float> const& mask, vnl_matrix_fixed<double,3,3> K, bool projective);


  vnl_vector_fixed<double,6> matrix_to_coeffs_SE3(vnl_matrix_fixed<double,4,4> const& M);
  vnl_vector_fixed<double,6> matrix_to_coeffs_GA2(vnl_matrix_fixed<double,3,3> const& M);
  vnl_vector_fixed<double,8> matrix_to_coeffs_P2(vnl_matrix_fixed<double,3,3> const& M);

  vnl_matrix_fixed<double,4,4> coeffs_to_matrix_SE3(vnl_vector_fixed<double,6> const& a);
  vnl_matrix_fixed<double,3,3> coeffs_to_matrix_GA2(vnl_vector_fixed<double,6> const& a);
  vnl_matrix_fixed<double,3,3> coeffs_to_matrix_P2(vnl_vector_fixed<double,8> const &a);

  bool logm_approx(vnl_matrix<double> const& A, vnl_matrix<double> &logA, double tol = 1e-12);

  vnl_matrix<double> SE3_to_H_Jacobian(double plane_theta, double plane_phi, double plane_dz);
  vnl_matrix<double> H_to_SE3_Jacobian(double plane_theta, double plane_phi, double plane_dz);

  //psm_camera_opt_homography_generator<obs_datatype> *homography_gen_;


};





#endif
