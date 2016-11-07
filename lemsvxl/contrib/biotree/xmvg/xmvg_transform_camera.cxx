#include "xmvg_transform_camera.h"
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_point_3d.h>

  //This is taken from xmvg_no_noise_filter_3d.cxx
xmvg_perspective_camera<double> xmvg_transform_camera(xmvg_perspective_camera<double>  cam,
                                                      vnl_double_3 rotation_axis,
                                                      double rotation_angle,
                                                      vgl_point_3d<double> centre){
  vgl_h_matrix_3d<double> transformer;
  transformer.set_identity();
  transformer.set_translation(centre.x(), centre.y(), centre.z());
  rotation_axis.normalize();
  // first minus : the angle is from [0,0,1] to the orientation of the cylinder, but here we are
  //               rotating from the orientation to [0,0,1], so we take the minus
  // second minus: the postmultiply operation is the rotation of the coordinate frame, not
  //               the camera, so we take the minus.
  transformer.set_rotation_about_axis(rotation_axis, -(-rotation_angle));
  return xmvg_perspective_camera<double>::postmultiply(cam, transformer);
}
