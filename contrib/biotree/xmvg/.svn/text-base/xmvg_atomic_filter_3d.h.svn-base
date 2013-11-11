// This is /contrib/biotree/xmvg/xmvg_atomic_filter_3d.h

#ifndef xmvg_filter_3d_h_
#define xmvg_filter_3d_h_

//: 
// \file   xmvg_atomic_filter_3d.h
// \brief  The base class of 3D filter. It contains a splating interface for all the filter3d
// \author Kongbin Kang and H. Can Aras
// \date   2005-01-26
// 

#include "xmvg_atomic_filter_2d.h"
#include "xmvg_filter_descriptor.h"
#include "xmvg_perspective_camera.h"

template< class T>
class xmvg_atomic_filter_3d
{
  public:
   
    //: constructor
    xmvg_atomic_filter_3d() {}

    //: destructor
    virtual ~xmvg_atomic_filter_3d() { }

    //: interface for splat
    virtual xmvg_atomic_filter_2d<T> splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam)=0;

    //: scale the current filter up or down
    virtual void scale_filter(double s)=0;

  private:

    xmvg_filter_descriptor descriptor_;
};

//template <class T>
//void x_write(vcl_ostream& os, xmvg_atomic_filter_3d<T>& af, vcl_string name);

#endif

