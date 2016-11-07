#include "vpgl_rat_proj_camera.h"

#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_box_3d.h>

vpgl_rat_proj_camera::vpgl_rat_proj_camera(vpgl_rational_camera<double>& rat_cam)
: rat_cam_(rat_cam)
{}

void vpgl_rat_proj_camera::compute()
{
  vpgl_scale_offset<double> sox =
    rat_cam_.scl_off(vpgl_rational_camera<double>::X_INDX);
  vpgl_scale_offset<double> soy =
    rat_cam_.scl_off(vpgl_rational_camera<double>::Y_INDX);
  vpgl_scale_offset<double> soz =
    rat_cam_.scl_off(vpgl_rational_camera<double>::Z_INDX);
  vpgl_scale_offset<double> sou =
    rat_cam_.scl_off(vpgl_rational_camera<double>::U_INDX);
  vpgl_scale_offset<double> sov =
    rat_cam_.scl_off(vpgl_rational_camera<double>::V_INDX);
  vgl_point_3d<double> pmin(sox.offset()-sox.scale(),
                            soy.offset()-soy.scale(), 0);
  vgl_point_3d<double> pmax(sox.offset()+sox.scale(), 
                            soy.offset()+soy.scale(),
                            soz.scale());
  /*double xoff = rat_cam_.offset(vpgl_rational_camera<double>::X_INDX);
  double yoff = rat_cam_.offset(vpgl_rational_camera<double>::Y_INDX);
  double zoff = rat_cam_.offset(vpgl_rational_camera<double>::Z_INDX);
  if (zoff < 0)
    zoff = 0;

  double xs = rat_cam_.scale(vpgl_rational_camera<double>::X_INDX);
  double ys = rat_cam_.scale(vpgl_rational_camera<double>::Y_INDX);
  double zs = rat_cam_.offset(vpgl_rational_camera<double>::Z_INDX);
*/
  //vgl_point_3d<double> center(xoff, yoff, zoff);
  vgl_box_3d<double> vol(pmin, pmax);
  //pcc_ = vpgl_perspective_camera_compute();
  vpgl_perspective_camera_compute::compute(rat_cam_, vol, persp_cam_, t_);
  vcl_cout << persp_cam_;
}

//: Project a point in world coordinates onto the image plane.
/*vgl_homg_point_2d<double> vpgl_rat_proj_camera::project( const vgl_homg_point_3d<double>& world_point ) const
{
  //vpgl_proj_camera_compute pcc(rat_cam_);
  vgl_h_matrix_3d<double> T = pcc_.norm_trans();

  vgl_homg_point_2d<double> p = proj_cam_->project(T(world_point));
  return p;
}*/

void vpgl_rat_proj_camera::shift_camera(vgl_point_3d<double> center)
{
  
 // pcc_.compute(center, proj_cam_);
 
}
