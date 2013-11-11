#ifndef XMVG_POINT_FILTER_3D_H_
#define XMVG_POINT_FILTER_3D_H_
//: 
// \file  xmvg_point_filter_3d.h
// \brief  a 3d point atomic filterused to debugging the whole filtering proc
// \author    Kongbin Kang
// \date        2005-07-13
// 
#include "xmvg_atomic_filter_3d.h"
#include "xmvg_point_filter_descriptor.h"

class xmvg_point_filter_3d : public xmvg_atomic_filter_3d <double>
{
  // member functions
  public:
    //: Empty Constructor
    xmvg_point_filter_3d();

    //: Constructor from the descriptor
    xmvg_point_filter_3d(xmvg_point_filter_descriptor& descriptor);
    
    //: Destructor
    virtual ~xmvg_point_filter_3d();
    
    //: Splats the filter centered at "centre" for a single view indexed by "index"
    xmvg_atomic_filter_2d<double> splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam);

    //: get the descriptor
    xmvg_point_filter_descriptor descriptor() { return descriptor_; }

    //: scale operation has no effects on point filter
    void scale_filter(double s) { };

  protected:

  // member variables
  public:

  protected:
    xmvg_point_filter_descriptor descriptor_;
};

void x_write(vcl_ostream& os, xmvg_point_filter_3d pf);

#endif
