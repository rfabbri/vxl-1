// This is /contrib/biotree/xmvg/xmvg_gaussian_edge_detector_y_3d.cxx

#include "xmvg_gaussian_edge_detector_y_3d.h"

xmvg_gaussian_edge_detector_y_3d::xmvg_gaussian_edge_detector_y_3d()
{
}

xmvg_gaussian_edge_detector_y_3d::xmvg_gaussian_edge_detector_y_3d(xmvg_gaussian_edge_detector_descriptor & descriptor)
{
  assert(descriptor.name() == vcl_string("gaussian_edge_detector"));
  descriptor_ = descriptor;
}

xmvg_gaussian_edge_detector_y_3d::~xmvg_gaussian_edge_detector_y_3d()
{
}

// using first derivative of Gaussian
class analytic_gauss_edge_detector_y_1
{
public:
  //: constructor with the fixed parameters
  analytic_gauss_edge_detector_y_1(double sigma, vnl_double_3 p) : sigma_(sigma), p_(p) 
  {
    C = vcl_pow(p_.get(0),2) + vcl_pow(p_.get(1),2) + vcl_pow(p_.get(2),2);
    sigmasq_ = vcl_pow(sigma_, 2);
    G = -p_.get(1) / sigmasq_;
    common_coeff_ = 1 / (2*(vnl_math::pi)*sigmasq_);
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
    double B = sintheta*(p_.get(0)*vcl_cos(phi_) + p_.get(1)*vcl_sin(phi_)) + costheta*(p_.get(2));
    double F = -sintheta * sin(phi_) / sigmasq_;

    double expo = vcl_exp((vcl_pow(B,2.0) - C) / (2*sigmasq_));
    double val = common_coeff_ * expo * (B*F - G);
    return val;
  }
protected:
  // fixed parameters
  double sigma_;
  vnl_double_3 p_;
  // varying parameters
  double theta_;
  double phi_;
  // pre-calculated values to save computing time
  double sigmasq_;
  double common_coeff_;
  double C, G;
};

// using third derivative of Gaussian
class analytic_gauss_edge_detector_y_3
{
public:
  //: constructor with the fixed parameters
  analytic_gauss_edge_detector_y_3(double sigma, vnl_double_3 p) : sigma_(sigma), p_(p) 
  {
    x0 = p_.get(0);
    y0 = p_.get(1);
    z0 = p_.get(2);
    C = vcl_pow(x0,2) + vcl_pow(y0,2) + vcl_pow(z0,2);
    sigmatwo_ = vcl_pow(sigma_, 2);
    sigmasix_ = vcl_pow(sigma_, 6);
    G = (3*y0*sigmatwo_ - vcl_pow(y0,3)) / sigmasix_;
    common_coeff_ = 1 / (12*(vnl_math::pi)*sigmatwo_);
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
    double sinphi = vcl_sin(phi_);
    double cosphi = vcl_cos(phi_);
    double B = sintheta*(x0*cosphi + y0*sinphi) + costheta*z0;
    double D = -vcl_pow(sintheta*sinphi,3) / sigmasix_;
    double E = -3*vcl_pow(sintheta*sinphi,2)*y0 / sigmasix_;
    double F = 3*sintheta*sinphi*(sigmatwo_-vcl_pow(y0,2)) / sigmasix_;

    double expo = vcl_exp((vcl_pow(B,2.0) - C) / (2*sigmatwo_));
    double val = common_coeff_ * expo * (G - B*F + vcl_pow(B,2)*E - D*vcl_pow(B,3) + sigmatwo_*E - 3*sigmatwo_*D*B);
    return val;
  }
protected:
  // fixed parameters
  double sigma_;
  vnl_double_3 p_;
  // varying parameters
  double theta_;
  double phi_;
  // pre-calculated values to save computing time
  double sigmatwo_;
  double sigmasix_;
  double common_coeff_;
  double C, G;
  double x0,y0,z0;
};

xmvg_atomic_filter_2d<double> xmvg_gaussian_edge_detector_y_3d::splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam)
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
  analytic_gauss_edge_detector_y_1 aged_y(descriptor_.sigma(), vnl_double_3(ray_start.x(), ray_start.y(), ray_start.z()));

  for(int j=min_y; j<=max_y; j++)
  {
    for(int i=min_x; i<=max_x; i++)
    {
      line = cam_trans.xmvg_backproject(vgl_homg_point_2d<double>(double(i), double(j), 1.0));
      ray_start = (line.point_finite());
      ray_direction.set( line.point_infinite().x(), line.point_infinite().y(), line.point_infinite().z() );

      // if the ray direction is given in the wrong direction, i.e. not towards the image plane, but away from it
      // then it should be corrected, note that acos returns a value between [0, pi].
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
 
      aged_y.set_varying_params(theta, phi);
      double analytic_result = aged_y.evaluate();
      image[i-min_x][j - min_y] = analytic_result;
    }
  }
  xmvg_atomic_filter_2d<double> result(left_upper, size, image);
  return result;
}

void x_write(vcl_ostream& os, xmvg_gaussian_edge_detector_y_3d detector)
{
  x_write(os, detector.descriptor());
}
