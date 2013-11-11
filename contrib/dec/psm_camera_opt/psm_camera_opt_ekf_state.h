#ifndef psm_camera_opt_ekf_state_h_
#define psm_camera_opt_ekf_state_h_

//:
// \file
// \brief // Representation of camera optimization Extended Kalman Filter state at step k
//           
// \author Daniel Crispell
// \date 03/01/08
// \verbatim
// Modifications
// 03/25/08 dec  moved to contrib/dec/breg3d
// \endverbatim

#include <vcl_iostream.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>


class psm_camera_opt_ekf_state
{
public:
  // init constructor
  psm_camera_opt_ekf_state(double t_scale, 
    vgl_point_3d<double> base_point, vgl_rotation_3d<double> base_rot, 
    double init_translation_var, double init_rotation_var);

  // full constructor
  psm_camera_opt_ekf_state(unsigned time_index, double t_scale, 
    vgl_point_3d<double> base_point, vgl_rotation_3d<double> base_rot, 
    vnl_vector_fixed<double,6> xk, vnl_matrix_fixed<double,6,6> Pk)
    : k_(time_index), base_point_(base_point), base_rotation_(base_rot), xk_(xk), Pk_(Pk), t_scale_(t_scale) {};

  // default constructor
  psm_camera_opt_ekf_state(){};
  ~psm_camera_opt_ekf_state(){};

  // setters and getters
  unsigned k() const {return k_;}
  vnl_vector_fixed<double,6> get_state() const {return xk_;}
  vnl_matrix_fixed<double,6,6> get_error_covariance() const {return Pk_;}
  void set_state(vnl_vector_fixed<double,6> const& xk){xk_ = xk;}
  void set_error_covariance(vnl_matrix_fixed<double,6,6> const& Pk){Pk_ = Pk;}
  // set error covariance assuming independant variables
  void set_error_covariance(double center_var, double rot_var);
  void set_base_point(vgl_point_3d<double> const& point) { base_point_ = point; }
  void set_base_rotation(vgl_rotation_3d<double> const& rot) { base_rotation_ = rot; }

  double t_scale() const {return t_scale_;}

  vgl_rotation_3d<double> get_rotation() const;
  vgl_point_3d<double> get_point() const;



private:
  unsigned k_;
  vnl_matrix_fixed<double,6,6> Pk_;
  vnl_vector_fixed<double,6> xk_;

  vgl_point_3d<double> base_point_;
  vgl_rotation_3d<double> base_rotation_;

  // scale factor for translations. 
  double t_scale_;

  // IO functions
  friend vcl_ostream&  operator<<(vcl_ostream& s, psm_camera_opt_ekf_state const& ekf_state);

};

//: output description of state to stream.
vcl_ostream&  operator<<(vcl_ostream& s, psm_camera_opt_ekf_state const& ekf_state);


#endif
