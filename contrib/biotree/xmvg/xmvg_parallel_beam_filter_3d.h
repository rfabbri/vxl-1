#if !defined(XMVG_PARALLEL_BEAM_FILTER_3D_H_)
#define XMVG_PARALLEL_BEAM_FILTER_3D_H_

#include "xmvg_atomic_filter_3d.h"
#include <vgl/vgl_box_2d.h>
#include <vnl/vnl_double_2.h>


template <class filter_3d, class T=double>
class xmvg_parallel_beam_filter_3d {
  /*  one might think this class should extend xmvg_atomic_filter_3d<T> 
      but that class has a private descriptor_ field
  */
 public:
    xmvg_atomic_filter_2d<T> splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam);
    xmvg_parallel_beam_filter_3d(filter_3d filter);
    filter_3d filter() { return filter_; }
 protected:
    vgl_box_2d<int> get_box_2d(const xmvg_perspective_camera<double>  & cam,
                               vgl_box_3d<double> const & box);
 /*
vnl_double_3 find_ray_start(vnl_double_3 start0, vnl_double_2 projection0,
                                    vnl_double_2 projection1);
 */
 //for testing:
 double line_integral(vgl_point_3d<double> ray_start, vgl_vector_3d<double> ray_direction, double radius);

    filter_3d filter_;
};

template <class filter_3d, class T>
void x_write(vcl_ostream& os, xmvg_parallel_beam_filter_3d<filter_3d, T> f);

#endif
