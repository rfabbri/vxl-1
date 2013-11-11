// This is /contrib/biotree/xmvg/xmvg_no_noise_filter_3d.cxx

#include "xmvg_no_noise_filter_3d.h"

xmvg_no_noise_filter_3d::xmvg_no_noise_filter_3d()
{
}

xmvg_no_noise_filter_3d::xmvg_no_noise_filter_3d(xmvg_no_noise_filter_descriptor & descriptor)
{
  assert(descriptor.name() == vcl_string("no_noise_filter"));
  descriptor_ = descriptor;
}

xmvg_no_noise_filter_3d::~xmvg_no_noise_filter_3d()
{
}

vgl_box_2d<int> xmvg_no_noise_filter_3d::get_box_2d(xmvg_perspective_camera<double>  cam,
                                                    vgl_box_3d<double> const & box){
    vgl_box_2d<double> box_2d = vpgl_project::project_bounding_box(cam, box);
    int min_x = int(vcl_floor(box_2d.min_x()));   int max_x = int(vcl_ceil(box_2d.max_x()));
    int min_y = int(vcl_floor(box_2d.min_y()));   int max_y = int(vcl_ceil(box_2d.max_y()));
    return vgl_box_2d<int>(min_x, max_x, min_y, max_y);
}

xmvg_atomic_filter_2d<double> xmvg_no_noise_filter_3d::splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam)
{
  // translate the system to the origin, rotate it to collide the cylinder orientation with the z-axis
  vgl_h_matrix_3d<double> transformer;
  transformer.set_identity();
  transformer.set_translation(centre.x(), centre.y(), centre.z());
  vnl_double_3 axis(descriptor_.rotation_axis()[0], 
                    descriptor_.rotation_axis()[1], 
                    descriptor_.rotation_axis()[2]);
  axis.normalize();
  // first minus : the angle is from [0,0,1] to the orientation of the cylinder, but here we are
  //               rotating from the orientation to [0,0,1], so we take the minus
  // second minus: the postmultiply operation is the rotation of the coordinate frame, not
  //               the camera, so we take the minus.
  transformer.set_rotation_about_axis(axis, -(-descriptor_.rotation_angle()));
  xmvg_perspective_camera<double> cam_trans = xmvg_perspective_camera<double>::postmultiply(cam, transformer);

  vgl_box_2d<int> int_box_2d = get_box_2d(cam_trans, descriptor_.box());

  int min_x = int_box_2d.min_x();   int max_x = int_box_2d.max_x();
  int min_y = int_box_2d.min_y();   int max_y = int_box_2d.max_y();

  unsigned u_size = max_x - min_x + 1;
  unsigned v_size = max_y - min_y + 1;
  // the variables to create an atomic_filter_2d
  vnl_int_2 left_upper(min_x, min_y);
  vnl_int_2 size(u_size, v_size);
  vbl_array_2d<double> image(u_size, v_size);
  for(int j=min_y; j<=max_y; j++)
  {
    for(int i=min_x; i<=max_x; i++)
    {
//      vgl_homg_line_3d_2_points<double> line = cam_trans.backproject(vgl_homg_point_2d<double>(double(i), double(j), 1.0));
      vgl_homg_line_3d_2_points<double> line = cam_trans.xmvg_backproject(vgl_homg_point_2d<double>(double(i), double(j), 1.0));
      vgl_point_3d<double> ray_start(line.point_finite());
      vgl_vector_3d<double> ray_direction(line.point_infinite().x(), line.point_infinite().y(), line.point_infinite().z());
      double inner = line_integral(ray_start, ray_direction, descriptor_.inner_radius());
      double outer = line_integral(ray_start, ray_direction, descriptor_.outer_radius());
      double val = 2 * inner - outer;
      image[i-min_x][j - min_y] = val;
    }
  }
  xmvg_atomic_filter_2d<double> result(left_upper, size, image);
  return result;
}

double xmvg_no_noise_filter_3d::line_integral(vgl_point_3d<double> ray_start, vgl_vector_3d<double> ray_direction, double radius)
{
  ray_direction = normalize(ray_direction);
  double r = radius;
  double H = descriptor_.length();
  double x0 = ray_start.x();
  double y0 = ray_start.y();
  double z0 = ray_start.z();
  double x1 = ray_direction.x();
  double y1 = ray_direction.y();
  double z1 = ray_direction.z();

  double h = H/2;
  // First, we check the special case when the ray is parallel to the cylinder orientation
  if(x1 == 0 && y1 == 0)
  {
    // The ray may be passing through the cylinder
    // If the ray start is over the top tap and the ray direction is downwards OR
    // is below the bottom tap and the ray direction is upwards, then it intersects the cylinder
    if((x0*x0 + y0*y0 <= r*r) && (z0*z1 < 0))
      return H;
    else
      return 0;
  }
  // Coefficients of the equation Au^2 + Bu + C=0
  double A = x1*x1 + y1*y1;
  double B = 2*(x0*x1 + y0*y1);
  double C = x0*x0 + y0*y0 - r*r;
  double delta = B*B - 4*A*C;
  if(delta <= 0)
    return 0;
  else
  {
    double u_1 = (-B + sqrt(delta)) / (2*A);
    double u_2 = (-B - sqrt(delta)) / (2*A);
    // Sort u values
    if(u_1 > u_2)
    {
      double temp = u_1;
      u_1 = u_2;
      u_2 = temp;
    }
    // Handle another special case here where the ray direction is parallel to the cylinder taps
    if(z1 == 0)
    {
      if(vcl_abs(z0) > h)
        return 0;
      else
      {
        // Intersection points
        vgl_point_3d<double> p1 = ray_start + u_1 * ray_direction;
        vgl_point_3d<double> p2 = ray_start + u_2 * ray_direction;
        vgl_vector_3d<double> p = p1 - p2;
        return p.length();
      }
    }
    // Find the u values of taps and sort them, note that z1=0 case already handled above
    double u_tap_1 = (h - z0) / z1;
    double u_tap_2 = (-h - z0) / z1;
    if (u_tap_1 > u_tap_2)
    {
      double temp = u_tap_1;
      u_tap_1 = u_tap_2;
      u_tap_2 = temp;
    }
    // Check for different cases of intersection
    if(u_1 < u_tap_1)
    {
      if(u_2 < u_tap_1) // No intersection
        return 0;
      else if(u_2 <= u_tap_2) // Update small u
        u_1 = u_tap_1;
      else //Update both
      {
        u_1 = u_tap_1;
        u_2 = u_tap_2;
      }
    }
    else if(u_1 <= u_tap_2)
    {
      // u_2 < u_tap_1 is not a case
      if(u_2 <= u_tap_2) // Change nothing
        assert (1);
      else //Update big u
        u_2 = u_tap_2;
    }
    else // No intersection, i.e. u_1 > u_tap_2
    {
      // u_2 < u_tap_1 is not a case
      // u_2 <= u_tap_2 is not a case
      return 0;
    }
    // Intersection points
    vgl_point_3d<double> p1 = ray_start + u_1 * ray_direction;
    vgl_point_3d<double> p2 = ray_start + u_2 * ray_direction;
    vgl_vector_3d<double> p = p1 - p2;
    return p.length();
  }
}

void x_write(vcl_ostream& os, xmvg_no_noise_filter_3d f)
{
  xmvg_no_noise_filter_descriptor fd = f.descriptor();
  x_write(os, static_cast<xmvg_filter_descriptor&>(fd));
}
