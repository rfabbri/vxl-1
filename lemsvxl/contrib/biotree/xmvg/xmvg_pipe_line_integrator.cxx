#include "xmvg_pipe_line_integrator.h"

xmvg_pipe_line_integrator::xmvg_pipe_line_integrator(vgl_vector_3d<double> ray_direction,
                                                             double radius_inner, double radius_outer, double length){
  ray_direction_ = normalize(ray_direction);
  //r_ = radius;
  h_ = length/2;
  x1_ = ray_direction.x();
  y1_ = ray_direction.y();
  z1_ = ray_direction.z();
  A_ = x1_ * x1_ + y1_ * y1_;
  minus_r_squared_inner_ = - radius_inner*radius_inner;
  minus_r_squared_outer_ = - radius_outer*radius_outer;
    line_parallel_to_cylinder_ = x1_ == 0 && y1_ ==0;
}

void xmvg_pipe_line_integrator::init_ray_start(vgl_point_3d<double> ray_start, 
                                                   vgl_vector_3d<double> ray_start_increment){
  ray_start_ = ray_start;
  x0_ = ray_start.x();
  y0_ = ray_start.y();
  z0_ = ray_start.z();
  delta_x0_ = ray_start_increment.x();
  delta_y0_ = ray_start_increment.y();
  delta_z0_ = ray_start_increment.z();
  minus_delta_z0_over_z1_ = -delta_z0_/z1_;
  twice_x1_delta_x0_plus_y1_delta_y0_ = 2 * (x1_ * delta_x0_ + y1_ * delta_y0_);
  B_ = 2 * (x0_ * x1_ + y0_ * y1_);
  minus_4_A_ = -4 * A_;
  one_over_2_A_ = 1/(2 * A_);
  u_tap_1_ = (h_ - z0_)/z1_;
  u_tap_2_ = (-h_ - z0_)/z1_;
  if (u_tap_1_ > u_tap_2_){
    double temp = u_tap_1_;
    u_tap_1_ = u_tap_2_;
    u_tap_2_ = temp;
  }
}


double xmvg_pipe_line_integrator::increment_ray_start_and_integrate(){
  x0_ += delta_x0_;
  y0_ += delta_y0_;
  z0_ += delta_z0_;
  u_tap_1_ += minus_delta_z0_over_z1_;
  u_tap_2_ += minus_delta_z0_over_z1_;

  //  ++ t_;
  //  t_squared_ += t_ + t_ - 1;

  /*  THIS CASE SHOULD BE DONE SPECIALLY, I.E. WITHOUT CALLING THE XMVG_PIPE
  // First, we check the special case when the ray is parallel to the cylinder orientation
  if (line_parallel_to_cylinder_)
  {
    // The ray may be passing through the cylinder
    // If the ray start is over the top tap and the ray direction is downwards OR
    // is below the bottom tap and the ray direction is upwards, then it intersects the cylinder
    double squared_radius_of_start_point = x0_ * x0_  + y0_ * y0_;
    if (squared_radius_of_start_point <= -minus_r_squared_outer_ 
        && squared_radius_of_start_point <= -minus_r_squared_inner_
        && z0_ * z1_ < 0){
      return 2*h_;
    }
    else {
      return 0;
    }
  }
  */
  // Coefficients of the equation Au^2 + Bu + C=0
  /* A_ unchanged */
  B_ += twice_x1_delta_x0_plus_y1_delta_y0_;
  double x0_squared_plus_y0_squared = x0_*x0_+y0_*y0_;
  double C_inner = x0_squared_plus_y0_squared + minus_r_squared_inner_;
  double C_outer = x0_squared_plus_y0_squared + minus_r_squared_outer_;
  double B_squared = B_ * B_;
  double val_inner, val_outer;

 {  //first compute inner
  double delta = B_squared + minus_4_A_ * C_inner;
  if(delta <= 0)
    val_inner = 0;
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
        val_inner = 0;
      else
      {
        // Intersection points
        vgl_point_3d<double> p1 = ray_start_ + u_1 * ray_direction_;
        vgl_point_3d<double> p2 = ray_start_ + u_2 * ray_direction_;
        vgl_vector_3d<double> p = p1 - p2;
        val_inner = p.length();
      }
    }
    else {
      if (u_2 < u_tap_1_ || u_1 > u_tap_2_){
        val_inner = 0;
      }
      else {
        u_1 = u_1 > u_tap_1_ ? u_1 : u_tap_1_;
        u_2 = u_2 < u_tap_2_ ? u_2 : u_tap_2_;
        // Intersection points
        vgl_point_3d<double> p1 = ray_start_ + u_1 * ray_direction_;
        vgl_point_3d<double> p2 = ray_start_ + u_2 * ray_direction_;
        vgl_vector_3d<double> p = p1 - p2;
        val_inner = p.length();
      }
    }
  }
 }
 {//Now do outer
  double delta = B_squared + minus_4_A_ * C_outer;
  if(delta <= 0)
    val_outer = 0;
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
        val_outer = 0;
      else
      {
        // Intersection points
        vgl_point_3d<double> p1 = ray_start_ + u_1 * ray_direction_;
        vgl_point_3d<double> p2 = ray_start_ + u_2 * ray_direction_;
        vgl_vector_3d<double> p = p1 - p2;
        val_outer = p.length();
      }
    }
    else {
      if (u_2 < u_tap_1_ || u_1 > u_tap_2_){
        val_outer = 0;
      }
      else {
        u_1 = u_1 > u_tap_1_ ? u_1 : u_tap_1_;
        u_2 = u_2 < u_tap_2_ ? u_2 : u_tap_2_;
        // Intersection points
        vgl_point_3d<double> p1 = ray_start_ + u_1 * ray_direction_;
        vgl_point_3d<double> p2 = ray_start_ + u_2 * ray_direction_;
        vgl_vector_3d<double> p = p1 - p2;
        val_outer = p.length();
      }
    }
  }
 }
 //finally return result
 return 2*val_inner - val_outer;
}

