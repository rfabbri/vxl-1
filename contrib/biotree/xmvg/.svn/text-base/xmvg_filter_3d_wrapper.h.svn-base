#if !defined(XMVG_FILTER_3D_WRAPPER_H_)
#define XMVG_FILTER_3D_WRAPPER_H_

template <class filter_3d, class T=double>
class xmvg_filter_3d_wrapper {
 public:
  filter_3d filter_;
  xmvg_filter_3d_wrapper(const filter_3d & filter): filter_(filter) {}
  xmvg_atomic_filter_2d<T> splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam){
    return filter_.splat(centre, cam);
  }
};

template <class filter_3d>
void x_write(vcl_ostream& os, xmvg_filter_3d_wrapper<filter_3d> f){
  x_write(os, f.filter_);
}

#endif
