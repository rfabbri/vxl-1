#if !defined(XMVG_GAUSSIAN_INTEGRATING_FILTER_3D_H_)
#define XMVG_GAUSSIAN_INTEGRATING_FILTER_3D_H_

#include "xmvg_gaussian_line_integrator.h"
#include "xmvg_gaussian_filter_descriptor.h"

/* This class serves the role of gaussian_filter_3d but has a different interface.
   It is intended to be used in combination with, e.g., the parallel_beam_filter_3d.
   It packages up a descriptor and a method for line integration.
*/

class xmvg_gaussian_integrating_filter_3d {
 protected:
 public:
  typedef xmvg_gaussian_line_integrator line_integrator_t;
  typedef xmvg_gaussian_filter_descriptor descriptor_t;
  xmvg_gaussian_integrating_filter_3d(descriptor_t descriptor) 
    : descriptor_(descriptor) {}
    descriptor_t descriptor(){return descriptor_;}
    line_integrator_t line_integrator(vgl_vector_3d<double> ray_direction){
      return line_integrator_t(ray_direction, descriptor_.sigma_r(), descriptor_.sigma_z());
    }
 protected:
    descriptor_t descriptor_;
};

inline void x_write(vcl_ostream &os, xmvg_gaussian_integrating_filter_3d pf)
{
  xmvg_gaussian_filter_descriptor gfd = pf.descriptor();
  x_write(os, gfd);
}
#endif
