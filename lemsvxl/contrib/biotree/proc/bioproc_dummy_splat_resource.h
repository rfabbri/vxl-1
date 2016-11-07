#ifndef bioproc_dummy_splat_resource_h_
#define bioproc_dummy_splat_resource_h_

//: 
// \file  bioproc_dummy_splat_resource.h
// \brief  A dummy splat resource used for waiting the splr finish
// \author    Kongbin Kang
// \date        2005-03-27
// 
#include <bio_defs.h>
#include <xmvg/xmvg_composite_filter_2d.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xscan/xscan_scan.h>
#include <vgl/vgl_point_3d.h>

template<class T, class F>
class bioproc_dummy_splat_resource
{
  public:

    //: splating
    xmvg_composite_filter_2d<T> splat(orbit_index t, vgl_point_3d<double> pt);

    //: constructor
    bioproc_dummy_splat_resource(
        xscan_scan const &, xmvg_composite_filter_3d<T, F> const &);

  private:
    
    xscan_scan scan_;
    xmvg_composite_filter_3d<T, F> filter_;
};

#endif
