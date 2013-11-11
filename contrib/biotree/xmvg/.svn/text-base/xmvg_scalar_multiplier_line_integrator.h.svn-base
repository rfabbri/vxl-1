#if !defined(XMVG_SCALAR_MULTIPLIER_LINE_INTEGRATOR_H_)
#define XMVG_SCALAR_MULTIPLIER_LINE_INTEGRATOR_H_

template <int c, class A>
  class xmvg_scalar_multiplier_line_integrator {
 public:
  xmvg_scalar_multiplier_line_integrator(A a)
    : a_(a) {}

  void init_ray_start(vgl_point_3d<double> ray_start, 
                      vgl_vector_3d<double> ray_start_increment){
    a_.init_ray_start(ray_start, ray_start_increment);
  }

  double increment_ray_start_and_integrate(){
    return c*a_.increment_ray_start_and_integrate();
  }
 protected:
  A a_;
};

#endif
