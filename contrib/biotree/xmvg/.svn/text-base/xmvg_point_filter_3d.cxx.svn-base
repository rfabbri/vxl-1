
#include "xmvg_point_filter_3d.h"

xmvg_point_filter_3d::xmvg_point_filter_3d()
{
}

xmvg_point_filter_3d::xmvg_point_filter_3d(xmvg_point_filter_descriptor & descriptor)
{
  assert(descriptor.name() == vcl_string("point_filter"));
  descriptor_ = descriptor;
}

xmvg_point_filter_3d::~xmvg_point_filter_3d()
{
}

xmvg_atomic_filter_2d<double> xmvg_point_filter_3d::splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam)
{

  vgl_homg_point_3d<double> c3d(centre);

  vgl_point_2d<double> c2d = cam.project(c3d);

  int u = static_cast<int>(vcl_floor(c2d.x()));
  int v = static_cast<int>(vcl_floor(c2d.y()));
  vnl_int_2 left_upper(u, v);

  vnl_int_2 size(1,1);

  vbl_array_2d<double> image(1,1);

  image[0][0] = 1.0;

  xmvg_atomic_filter_2d<double> result(left_upper, size, image);
  return result;
}

void x_write(vcl_ostream& os, xmvg_point_filter_3d pf)
{
}
