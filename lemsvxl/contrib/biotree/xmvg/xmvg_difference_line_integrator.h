#if !defined(XMVG_DIFFERENCE_LINE_INTEGRATOR_H_)
#define XMVG_DIFFERENCE_LINE_INTEGRATOR_H_

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>

template <class A, class B>
  class xmvg_difference_line_integrator {
 public:
  xmvg_difference_line_integrator(A a, B b): a_(a), b_(b) {}

  void init_ray_start(vgl_point_3d<double> ray_start, 
                      vgl_vector_3d<double> ray_start_increment){
    a_.init_ray_start(ray_start, ray_start_increment);
    b_.init_ray_start(ray_start, ray_start_increment);
  }

 double increment_ray_start_and_integrate(){
   return a_.increment_ray_start_and_integrate()
     - b_.increment_ray_start_and_integrate();
 }
 protected:
 A a_;
 B b_;
};

#endif
