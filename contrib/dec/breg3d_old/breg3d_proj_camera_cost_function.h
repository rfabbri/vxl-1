#ifndef breg3d_proj_camera_cost_function_6dof_h_
#define breg3d_proj_camera_cost_function_6dof_h_

#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3x3.h>

#include <vil/vil_image_view.h>

#include "breg3d_voxel_grid.h"

//: camera cost function which optimizes all 6 extrinsic parameters.  Intrinsic parameters are assumed fixed and known.
class breg3d_proj_camera_cost_function_6dof : public vnl_least_squares_function
{
public:

  breg3d_proj_camera_cost_function_6dof(breg3d_voxel_grid const& voxels, vil_image_view<float> frame, vnl_double_3x3 const& K, vnl_double_3x3 const& R_init_, vnl_double_3x1 const& T_init_);
  ~breg3d_proj_camera_cost_function_6dof(){};

  //: Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: Calculate the Jacobian, given the parameter vector x.
  // not implemented yet
  // virtual void gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian);

  void pack_variables(vnl_double_3x3 const& R, vnl_double_3x1 const& T, vnl_vector_fixed<double,6> &vars);
  void unpack_variables(vnl_double_3x3& R, vnl_double_3x1& T, vnl_vector_fixed<double,6> const& vars);

private:
    //breg3d_proj_camera_cost_function_6dof(){} : vnl_least_squares_function(6,1, vnl_least_squares_function::no_gradient);

    vnl_double_3x3 K_;
    vnl_double_3x3 R_init_;
    vnl_double_3x1 T_init_;

    breg3d_voxel_grid voxels_;
    vil_image_view<float> frame_;

};


//: camera cost function which optimizes orientation parameters only
class breg3d_proj_camera_cost_function_3dof : public vnl_least_squares_function
{
public:

  breg3d_proj_camera_cost_function_3dof(breg3d_voxel_grid const& voxels, vil_image_view<float> frame, vnl_double_3x3 const& K, vnl_double_3x3 const& R_init_, vnl_double_3x1 const& T_init_);
  ~breg3d_proj_camera_cost_function_3dof(){};

  //: Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: Calculate the Jacobian, given the parameter vector x.
  // not implemented yet
  // virtual void gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian);

  void pack_variables(vnl_double_3x3 const& R, vnl_vector_fixed<double,3> &vars);
  void unpack_variables(vnl_double_3x3& R, vnl_double_3x1& T, vnl_vector_fixed<double,3> const& vars);

private:

    vnl_double_3x3 K_;
    vnl_double_3x3 R_init_;
    vnl_double_3x1 camera_center_;

    breg3d_voxel_grid voxels_;
    vil_image_view<float> frame_;

};




#endif

