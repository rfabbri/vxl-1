// This is /contrib/biotree/xmvg/xmvg_gaussian_filter_3d.cxx

#include "xmvg_gaussian_filter_3d.h"

xmvg_gaussian_filter_3d::xmvg_gaussian_filter_3d()
{
}

xmvg_gaussian_filter_3d::xmvg_gaussian_filter_3d(xmvg_gaussian_filter_descriptor & descriptor)
{
  assert(descriptor.name() == vcl_string("gaussian_filter"));
  descriptor_ = descriptor;
}

xmvg_gaussian_filter_3d::~xmvg_gaussian_filter_3d()
{
}

class my_integrant : public vnl_analytic_integrant
{
public:
  my_integrant(double sr, double sz, vnl_double_3 p0) : sr_(sr), sz_(sz), p0_(p0) 
  { 
    oneoversr2_ = 1 / vcl_pow(sr_, 2); 
    oneoversz2_ = 1 / vcl_pow(sz_, 2);
    normalizer_ = -vcl_pow(sr_,2) / (sz_ * 2 * vcl_sqrt(2*vnl_math::pi));
  }

  void set_varying_params(double theta, double phi)
  { 
    theta_ = theta;
    phi_ = phi;
  }

  double f_(double rho)
  { 
    double x2 = vcl_pow( p0_.get(0) + rho * vcl_sin(theta_) * vcl_cos(phi_), 2 );
    double y2 = vcl_pow( p0_.get(1) + rho * vcl_sin(theta_) * vcl_sin(phi_), 2 );
    double z2 = vcl_pow( p0_.get(2) + rho * vcl_cos(theta_), 2 );
    double term1 = oneoversr2_ * ((x2 + y2) * oneoversr2_ - 2);
    double term2 = vcl_exp(-(x2+y2)*oneoversr2_/2) * vcl_exp(-z2*oneoversz2_/2);
    return (normalizer_ * term1 * term2);
  }
protected:
  // fixed parameters
  double sr_;
  double sz_;
  vnl_double_3 p0_;
  // varying parameters
  double theta_;
  double phi_;
  // pre-calculated values to save computing time
  double oneoversr2_;
  double oneoversz2_;
  double normalizer_;
};

class analytic_gauss_filter
{
public:
  //: constructor with the fixed parameters
  analytic_gauss_filter(double sr, double sz, vnl_double_3 p) : sr_(sr), sz_(sz), p_(p) 
  {
    C = vcl_pow(p_.get(0),2) + vcl_pow(p_.get(1),2);
    F = vcl_pow(p_.get(2),2);
    sr2_ = vcl_pow(sr_, 2);
    sz2_ = vcl_pow(sz_, 2);
  }
  //: setting the varying parameters, this is normally followed by an evaluate function call
  void set_varying_params(double theta, double phi)
  { 
    theta_ = theta;
    phi_ = phi;
  }
  //: evaluating the filter value, this is normally preceded by setting varying parameters
  double evaluate()
  {
    double sintheta = vcl_sin(theta_);
    double costheta = vcl_cos(theta_);
    double A = vcl_pow(sintheta,2);
    double B = 2*sintheta*(p_.get(0)*vcl_cos(phi_) + p_.get(1)*vcl_sin(phi_));
    double D = vcl_pow(costheta,2);
    double E = 2*p_.get(2)*costheta;
    double denominator = 2 * sr2_ * sz2_;
    double G = (A*sz2_ + D*sr2_) / denominator;
    double H = (B*sz2_ + E*sr2_) / denominator;
    double I = (C*sz2_ + F*sr2_) / denominator;

    double expo = vcl_exp((vcl_pow(H,2) / (4*G) - I));
    double all_common = 1 / (sz_ * vcl_sqrt(2*G));
    double coef1 = 1;
    double partial_common = -1 / (4*sr2_*G);
    double coef2 = A;
    double coef3 = (4*C*vcl_pow(G,2) - 2*H*B*G + A*vcl_pow(H,2)) / (2*G);

    double val = all_common * (coef1 + partial_common * (coef2 + coef3)) * expo;
    return val;
  }
protected:
  // fixed parameters
  double sr_;
  double sz_;
  vnl_double_3 p_;
  // varying parameters
  double theta_;
  double phi_;
  // pre-calculated values to save computing time
  double sr2_, sz2_;
  double C, F;
};

xmvg_atomic_filter_2d<double> xmvg_gaussian_filter_3d::splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam)
{
  // locals
  vgl_homg_line_3d_2_points<double> line;
  vgl_point_3d<double> ray_start;
  vgl_vector_3d<double> ray_direction;
  double rd_to_pa_angle;
  double theta, phi;
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

  vgl_box_2d<double> box_2d = vpgl_project::project_bounding_box(cam_trans, descriptor_.box());

  int min_x = int(vcl_floor(box_2d.min_x()));   int max_x = int(vcl_ceil(box_2d.max_x()));
  int min_y = int(vcl_floor(box_2d.min_y()));   int max_y = int(vcl_ceil(box_2d.max_y()));

  unsigned u_size = max_x - min_x + 1;
  unsigned v_size = max_y - min_y + 1;
  // the variables to create an atomic_filter_2d
  vnl_int_2 left_upper(min_x, min_y);
  vnl_int_2 size(u_size, v_size);
  vbl_array_2d<double> image(u_size, v_size);

  // initialize the ray start point here, which is always the camera center, to save computation
//  line = cam_trans.backproject(vgl_homg_point_2d<double>(double(0), double(0), 1.0));
  line = cam_trans.xmvg_backproject(vgl_homg_point_2d<double>(double(0), double(0), 1.0));
  ray_start = (line.point_finite());
  // create the analytic gaussian filter with the fixed values
  analytic_gauss_filter agf(descriptor_.sigma_r(), descriptor_.sigma_z(), vnl_double_3(ray_start.x(), ray_start.y(), ray_start.z()));
  // create then integrand with the fixed values
  my_integrant f(descriptor_.sigma_r(), descriptor_.sigma_z(), vnl_double_3(ray_start.x(), ray_start.y(), ray_start.z()));
  vnl_simpson_integral simpson_integral;
  // set lower and upper limits in a smart way such that they are picked close to the filter centre
  // compute source to filter centre distance and set the limits according to this
  vgl_vector_3d<double> src_to_filter = centre - cam_trans.get_camera_center();
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  double src_to_filter_dist = src_to_filter.length();
#endif
//  double lower_limit = src_to_filter_dist - 1;
//  double upper_limit = src_to_filter_dist + 1;

  for(int j=min_y; j<=max_y; j++)
  {
    // theta can be computed here once to save(?) computation
    for(int i=min_x; i<=max_x; i++)
    {
//      line = cam_trans.backproject(vgl_homg_point_2d<double>(double(i), double(j), 1.0));
      line = cam_trans.xmvg_backproject(vgl_homg_point_2d<double>(double(i), double(j), 1.0));
      ray_start = (line.point_finite());
      ray_direction.set( line.point_infinite().x(), line.point_infinite().y(), line.point_infinite().z() );

      // if the ray direction is given in the wrong direction, i.e. not towards the image plane, but away from it
      // then it should be corrected
      rd_to_pa_angle = vcl_acos(dot_product(ray_direction, cam_trans.principal_axis()) 
                               / (ray_direction.length() * cam_trans.principal_axis().length()));
      if(rd_to_pa_angle > vnl_math::pi_over_2)
        ray_direction = -ray_direction;
      theta = vnl_math::pi_over_2 - vcl_atan(ray_direction.z() / (vcl_sqrt(vcl_pow(ray_direction.x(),2) + vcl_pow(ray_direction.y(),2))));
      phi = vcl_atan(ray_direction.y() / ray_direction.x());
      // if x component of the ray direction is negative, then pi degrees should be added to phi since
      // atan returns value in the range of [-pi/2,pi/2].
      if(ray_direction.x() < 0)
        phi += vnl_math::pi;
 
      agf.set_varying_params(theta, phi);
      double analytic_result = agf.evaluate();
      image[i-min_x][j - min_y] = analytic_result;

//      f.set_varying_params(theta, phi);
//      integral_iterative = simpson_integral.integral(&f, lower_limit, upper_limit, 100);
//      sum += integral_iterative;
//      image[i-min_x][j - min_y] = integral_iterative;
    }
  }
  xmvg_atomic_filter_2d<double> result(left_upper, size, image);
  return result;
}

double xmvg_gaussian_filter_3d::splat_impulse(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam, vgl_point_3d<double> point)
{
  vgl_homg_line_3d_2_points<double> line;
  vgl_point_3d<double> ray_start;
  vgl_vector_3d<double> ray_direction;
  double rd_to_pa_angle;
  double theta, phi;
  // translate the system to the origin, rotate it to collide the cylinder orientation with the z-axis
  vgl_h_matrix_3d<double> transformer;
  transformer.set_identity();
  transformer.set_translation(centre.x(), centre.y(), centre.z());
  vnl_double_3 axis(descriptor_.rotation_axis()[0], descriptor_.rotation_axis()[1], descriptor_.rotation_axis()[2]);
  axis.normalize();
  // first minus : the angle is from [0,0,1] to the orientation of the cylinder, but here we are
  //               rotating from the orientation to [0,0,1], so we take the minus
  // second minus: the postmultiply operation is the rotation of the coordinate frame, not
  //               the camera, so we take the minus.
  transformer.set_rotation_about_axis(axis, -(-descriptor_.rotation_angle()));
  xmvg_perspective_camera<double> cam_trans = xmvg_perspective_camera<double>::postmultiply(cam, transformer);

  vgl_homg_point_3d<double> homg_point(transformer* vgl_homg_point_3d<double> (point));
  vgl_homg_point_2d<double> homg_projected_point = cam_trans.project(homg_point);

  //  vgl_point_2d<double> projected_point(homg_projected_point);
  //  line = cam_trans.xmvg_backproject(vgl_homg_point_2d<double>(projected_point.x(), projected_point.y(), 1.0));
  line = cam_trans.xmvg_backproject(homg_projected_point);
  ray_start = (line.point_finite());
  ray_direction.set( line.point_infinite().x(), line.point_infinite().y(), line.point_infinite().z() );
  // if the ray direction is given in the wrong direction, i.e. not towards the image plane, but away from it, it should be corrected
  rd_to_pa_angle = vcl_acos(dot_product(ray_direction, cam_trans.principal_axis()) 
    / (ray_direction.length() * cam_trans.principal_axis().length()));
  if(rd_to_pa_angle > vnl_math::pi_over_2)
    ray_direction = -ray_direction;
  theta = vnl_math::pi_over_2 - vcl_atan(ray_direction.z() / (vcl_sqrt(vcl_pow(ray_direction.x(),2) + vcl_pow(ray_direction.y(),2))));
  phi = vcl_atan(ray_direction.y() / ray_direction.x());
  // if x component of the ray direction is negative, then pi degrees should be added to phi since
  // atan returns value in the range of [-pi/2,pi/2].
  if(ray_direction.x() < 0)
    phi += vnl_math::pi;

  // create the analytic gaussian filter
  analytic_gauss_filter agf(descriptor_.sigma_r(), descriptor_.sigma_z(), vnl_double_3(ray_start.x(), ray_start.y(), ray_start.z()));
  agf.set_varying_params(theta, phi);
  double analytic_result = agf.evaluate();

  return analytic_result;
}

void x_write(vcl_ostream& os, xmvg_gaussian_filter_3d f)
{
  xmvg_gaussian_filter_descriptor fd = f.descriptor();
  x_write(os, fd);
}
