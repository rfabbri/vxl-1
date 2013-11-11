#if !defined(XMVG_TRANSFORM_CAMERA_H_)
#define XMVG_TRANSFORM_CAMERA_H_

#include "xmvg_perspective_camera.h"

//: translate the system to the origin and rotate it so filter orientation coincides with z axis
//used in simplifying the application of a filter_3d
xmvg_perspective_camera<double> xmvg_transform_camera(xmvg_perspective_camera<double>  cam,
                                                      vnl_double_3 rotation_axis,
                                                      double rotation_angle,
                                                      vgl_point_3d<double> centre);

#endif
