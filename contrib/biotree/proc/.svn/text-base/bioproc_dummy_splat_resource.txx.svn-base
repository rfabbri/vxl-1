#ifndef BIOPROC_DUMMY_SPLAT_RESOURCE_TXX_
#define BIOPROC_DUMMY_SPLAT_RESOURCE_TXX_

#include "bioproc_dummy_splat_resource.h"
template<class T, class F>
bioproc_dummy_splat_resource<T, F>::bioproc_dummy_splat_resource(
    xscan_scan const& scan, xmvg_composite_filter_3d<T, F> const & filter)
  : scan_(scan), filter_(filter)
{
   
}
  
template<class T, class F>
xmvg_composite_filter_2d<T> 
bioproc_dummy_splat_resource<T, F>::splat(orbit_index t,  vgl_point_3d<double> pt)
{
  xmvg_perspective_camera<double> cam = scan_(t); 
  return  filter_.splat(cam, pt);
}



// Code for easy instantiation.
#undef BIOPROC_DUMMY_SPLAT_RESOURCE_INSTANTIATE
#define BIOPROC_DUMMY_SPLAT_RESOURCE_INSTANTIATE(T, F) \
template class bioproc_dummy_splat_resource<T, F>;

#endif
