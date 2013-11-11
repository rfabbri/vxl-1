#if !defined(XMVG_CYLINDER_LINE_INTEGRATOR_H_)
#define XMVG_CYLINDER_LINE_INTEGRATOR_H_

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_math.h>
#include <vcl_cstdio.h>

class xmvg_cylinder_line_integrator
 {
 protected:
  // Cylinder parameters
   double  h_; // half-length of cylinder
   double minus_r_squared_ ; //- radius*radius
 // parameters of line of integration
 // orientation
    vgl_vector_3d<double> ray_direction_;
    double x1_, y1_, z1_;
 // ray start
   vgl_point_3d<double> ray_start_;
   double x0_, y0_, z0_;
 //Values that depend only on orientation:
   int line_parallel_to_cylinder_;
   double one_over_z1_;
   double A_;
   double minus_4_A_, one_over_2_A_;
 //values that depend on delta in ray start
   double delta_x0_, delta_y0_, delta_z0_,
     delta_x0_squared_plus_delta_y0_squared,
     twice_x1_delta_x0_plus_y1_delta_y0_;
 //values that change as ray start changes
   double B_;
 public:
 xmvg_cylinder_line_integrator(vgl_vector_3d<double> ray_direction,
                double radius, double length);
void init_ray_start(vgl_point_3d<double> ray_start, 
                    vgl_vector_3d<double> ray_start_increment);
 double increment_ray_start_and_integrate();
};

#endif
