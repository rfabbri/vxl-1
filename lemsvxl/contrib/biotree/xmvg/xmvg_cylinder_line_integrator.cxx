#include "xmvg_cylinder_line_integrator.h"

xmvg_cylinder_line_integrator::xmvg_cylinder_line_integrator(vgl_vector_3d<double> ray_direction,
                                                double radius, double length){
  ray_direction_ = normalize(ray_direction);
  //r_ = radius;
  h_ = length/2;
  x1_ = ray_direction.x();
  y1_ = ray_direction.y();
  z1_ = ray_direction.z();
  A_ = x1_ * x1_ + y1_ * y1_;
  minus_r_squared_ = - radius*radius;
  one_over_z1_ = 1/z1_;
  line_parallel_to_cylinder_ = x1_ == 0 && y1_ ==0;
}

void xmvg_cylinder_line_integrator::init_ray_start(vgl_point_3d<double> ray_start, 
                                                   vgl_vector_3d<double> ray_start_increment){
  ray_start_ = ray_start;
  x0_ = ray_start.x();
  y0_ = ray_start.y();
  z0_ = ray_start.z();
  delta_x0_ = ray_start_increment.x();
  delta_y0_ = ray_start_increment.y();
  delta_z0_ = ray_start_increment.z();
  twice_x1_delta_x0_plus_y1_delta_y0_ = 2 * (x1_ * delta_x0_ + y1_ * delta_y0_);
  B_ = 2 * (x0_ * x1_ + y0_ * y1_);
  minus_4_A_ = -4 * A_;
  one_over_2_A_ = 1/(2 * A_);
}


double xmvg_cylinder_line_integrator::increment_ray_start_and_integrate(){
  x0_ += delta_x0_;
  y0_ += delta_y0_;
  z0_ += delta_z0_;
  //  ++ t_;
  //  t_squared_ += t_ + t_ - 1;

  // First, we check the special case when the ray is parallel to the cylinder orientation
  if (line_parallel_to_cylinder_)
  {
    // The ray may be passing through the cylinder
    // If the ray start is over the top tap and the ray direction is downwards OR
    // is below the bottom tap and the ray direction is upwards, then it intersects the cylinder
    if((x0_ * x0_  + y0_ * y0_  <= - minus_r_squared_) && (z0_ * z1_ < 0))  //***could simplify this
      return 2*h_;
    else
      return 0;
  }
  // Coefficients of the equation Au^2 + Bu + C=0
  /* A_ unchanged */
  B_ += twice_x1_delta_x0_plus_y1_delta_y0_;
  double C = x0_*x0_+y0_*y0_+minus_r_squared_;
  double delta = B_ * B_ + minus_4_A_ * C;
  if(delta <= 0)
    return 0;
  else
  {
    double sqrt_delta = sqrt(delta);
    double u_1 = (-B_ + sqrt_delta) * one_over_2_A_;
    double u_2 = (-B_ - sqrt_delta) * one_over_2_A_;
    // Sort u values
    if(u_1 > u_2)
    {
      double temp = u_1;
      u_1 = u_2;
      u_2 = temp;
    }
    // Handle another special case here where the ray direction is parallel to the cylinder taps
    if(z1_ == 0)
    {
      if(vcl_abs(z0_) > h_)
        return 0;
      else
      {
        // Intersection points
        vgl_point_3d<double> p1 = ray_start_ + u_1 * ray_direction_;
        vgl_point_3d<double> p2 = ray_start_ + u_2 * ray_direction_;
        vgl_vector_3d<double> p = p1 - p2;
        return p.length();
      }
    }
    // Find the u values of taps and sort them, note that z1=0 case already handled above
    double u_tap_1 = (h_ - z0_) * one_over_z1_;
    double u_tap_2 = (-h_ - z0_) * one_over_z1_;
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
    vgl_point_3d<double> p1 = ray_start_ + u_1 * ray_direction_;
    vgl_point_3d<double> p2 = ray_start_ + u_2 * ray_direction_;
    vgl_vector_3d<double> p = p1 - p2;
    return p.length();
  }
}
