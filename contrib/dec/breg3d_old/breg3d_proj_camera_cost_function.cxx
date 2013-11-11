#include <vnl/vnl_least_squares_cost_function.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector.h>

#include <vgl/algo/vgl_rotation_3d.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>

#include <vil/vil_image_view.h>

#include "breg3d_proj_camera_cost_function.h"
#include "breg3d_voxel_grid.h"




void camera_cost_function(vpgl_camera<double> *camera, breg3d_voxel_grid &voxels, vil_image_view<float> &frame, vnl_vector<double> &fx)
{
  //#define USE_SUM_COST
#ifdef USE_SUM_COST
  double alpha = 100.0;
  vil_image_view<float> score_img = voxels.camera_score(frame, camera);

  vil_image_view<float>::iterator score_it = score_img.begin();
  vnl_vector<double>::iterator fx_it = fx.begin();
  double max_score = -1e6, min_score = 1e6;
  for (; score_it != score_img.end(); score_it++, fx_it++) {
    // need to convert score function to a cost function
    *fx_it = alpha / (*score_it + alpha);
    //if (*score_it > max_score)
    //  max_score = *score_it;
    //if(*score_it < min_score)
    //  min_score = *score_it;
  }
  //vcl_cout << vcl_endl << "max_score = " << max_score <<"    min_score = " << min_score << vcl_endl;

#else // prod cost
  vil_image_view<float> cost_img = voxels.camera_cost(frame, camera);

  vil_image_view<float>::iterator cost_it = cost_img.begin();
  vnl_vector<double>::iterator fx_it = fx.begin();
  double max_score = -1e6, min_score = 1e6;
  for (; cost_it != cost_img.end(); cost_it++, fx_it++) {
    *fx_it = *cost_it;
    //   if (*fx_it > max_score)
    //     max_score = *fx_it;
    //   if(*fx_it < min_score)
    //     min_score = *fx_it;
  }
  //vcl_cout << vcl_endl << "max_score = " << max_score <<"    min_score = " << min_score << vcl_endl;

#endif

  return;
}





breg3d_proj_camera_cost_function_6dof::breg3d_proj_camera_cost_function_6dof(breg3d_voxel_grid const& voxels, vil_image_view<float> frame, vnl_double_3x3 const& K, vnl_double_3x3 const& R_init, vnl_double_3x1 const& T_init) 
: vnl_least_squares_function(6,frame.size(), vnl_least_squares_function::no_gradient), K_(K), R_init_(R_init), T_init_(T_init), voxels_(voxels), frame_(frame) {}


void breg3d_proj_camera_cost_function_6dof::f(const vnl_vector<double> &x, vnl_vector<double> &fx)
{

  vnl_double_3x3 R;
  vnl_double_3x1 T;
  unpack_variables(R,T,x);

  // construct vpgl_camera from input parameters
  vnl_double_3x4 RT;    
  RT.set_columns(0,R);
  RT.set_columns(3,T);

  //vcl_cout << "RT = " << vcl_endl;
  //vcl_cout << RT << vcl_endl;

  vnl_matrix_fixed<double,3,4> cam_P = K_*RT;
  vpgl_proj_camera<double> camera(cam_P);

  camera_cost_function(&camera, voxels_, frame_, fx);

  return;
}



// convert rotation and translation functions into a vector of length 6.
// x0 - x2 : Rodrigues form of rotation matrix
// x3 - x5 : Translation vector
void breg3d_proj_camera_cost_function_6dof::pack_variables(vnl_double_3x3 const& R, vnl_double_3x1 const& T, vnl_vector_fixed<double,6> &vars)
{
  vnl_double_3x3 Rmod = R * R_init_.transpose();
  vnl_double_3x1 Tmod = T - Rmod * T_init_;

  vgl_rotation_3d<double> rot3d(Rmod);
  vnl_vector_fixed<double,3> rotv = rot3d.as_rodrigues();

  vars[0] = rotv[0];
  vars[1] = rotv[1];
  vars[2] = rotv[2];
  vars[3] = Tmod[0][0];
  vars[4] = Tmod[1][0];
  vars[5] = Tmod[2][0];

  return;
}


void breg3d_proj_camera_cost_function_6dof::unpack_variables(vnl_double_3x3& R, vnl_double_3x1& T, vnl_vector_fixed<double,6> const& vars)
{
  vnl_vector_fixed<double,3> rotv(vars[0],vars[1],vars[2]);
  vgl_rotation_3d<double> rot3d(rotv);
  vnl_double_3x3 Rmod = rot3d.as_matrix();
  vnl_double_3x1 Tmod;
  Tmod[0][0] = vars[3];
  Tmod[1][0] = vars[4];
  Tmod[2][0] = vars[5];

  R = Rmod*R_init_;
  T = Rmod*T_init_ + Tmod;

  return;
}


breg3d_proj_camera_cost_function_3dof::breg3d_proj_camera_cost_function_3dof(breg3d_voxel_grid const& voxels, vil_image_view<float> frame, vnl_double_3x3 const& K, vnl_double_3x3 const& R_init, vnl_double_3x1 const& T_init) 
: vnl_least_squares_function(3,frame.size(), vnl_least_squares_function::no_gradient), K_(K), R_init_(R_init),  voxels_(voxels), frame_(frame), camera_center_(-R_init.transpose()*T_init) {}


void breg3d_proj_camera_cost_function_3dof::f(const vnl_vector<double> &x, vnl_vector<double> &fx)
{

  vnl_double_3x3 R;
  vnl_double_3x1 T;
  unpack_variables(R,T,x);

  // construct vpgl_camera from input parameters
  vnl_double_3x4 RT;    
  RT.set_columns(0,R);
  RT.set_columns(3,T);

  //vcl_cout << "RT = " << vcl_endl;
  //vcl_cout << RT << vcl_endl;

  vnl_matrix_fixed<double,3,4> cam_P = K_*RT;
  vpgl_proj_camera<double> camera(cam_P);

  camera_cost_function(&camera, voxels_, frame_, fx);

  return;
}



// convert rotation and translation functions into a vector of length 3
// x0 - x2 : Rodrigues form of rotation matrix
void breg3d_proj_camera_cost_function_3dof::pack_variables(vnl_double_3x3 const& R, vnl_vector_fixed<double,3> &vars)
{
  vnl_double_3x3 Rmod = R * R_init_.transpose();

  vgl_rotation_3d<double> rot3d(Rmod);
  vnl_vector_fixed<double,3> rotv = rot3d.as_rodrigues();

  vars[0] = rotv[0];
  vars[1] = rotv[1];
  vars[2] = rotv[2];

  return;
}


void breg3d_proj_camera_cost_function_3dof::unpack_variables(vnl_double_3x3& R, vnl_double_3x1& T, vnl_vector_fixed<double,3> const& vars)
{
  vnl_vector_fixed<double,3> rotv(vars[0],vars[1],vars[2]);
  vgl_rotation_3d<double> rot3d(rotv);
  vnl_double_3x3 Rmod = rot3d.as_matrix();

  R = Rmod*R_init_;
  T = -R*camera_center_;

  return;
}

