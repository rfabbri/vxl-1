#if !defined(XMVG_PIPE_FILTER_3D_H_)
#define XMVG_PIPE_FILTER_3D_H_

#include "xmvg_no_noise_filter_descriptor.h"
#include "xmvg_cylinder_line_integrator.h"
#include "xmvg_difference_line_integrator.h"
#include "xmvg_scalar_multiplier_line_integrator.h"
#include "xmvg_pipe_line_integrator.h"

/* This class serves the role of no_noise_filter_3d but has a different interface.
   It is intended to be used in combination with, e.g., the parallel_beam_filter_3d.
   It packages up a descriptor and a method for line integration.
*/

class xmvg_pipe_filter_3d {
 protected:
 public:
  typedef xmvg_pipe_line_integrator line_integrator_t;
  typedef xmvg_no_noise_filter_descriptor descriptor_t;
  xmvg_pipe_filter_3d(descriptor_t descriptor) 
    : descriptor_(descriptor) {}
    descriptor_t descriptor(){return descriptor_;}
    line_integrator_t line_integrator(vgl_vector_3d<double> ray_direction){
      return xmvg_pipe_line_integrator(ray_direction, descriptor_.inner_radius(),
                                descriptor_.outer_radius(), descriptor_.length());
    }
 protected:
    descriptor_t descriptor_;
};

inline void x_write(vcl_ostream &os, xmvg_pipe_filter_3d pf)
{
  xmvg_no_noise_filter_descriptor pfd = pf.descriptor();
  xmvg_no_noise_filter_descriptor &pfd_ref = pfd;
  x_write(os, static_cast<xmvg_filter_descriptor&>(pfd_ref));
}

#endif
