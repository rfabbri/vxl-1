// This is /contrib/biotree/xmvg/xmvg_gaussian_filter_3d.h

#ifndef xmvg_gaussian_filter_3d_h
#define xmvg_gaussian_filter_3d_h

//:
// \file   xmvg_gaussian_filter.h
// \brief  3D gaussian filter.
// \author H. Can Aras
// \date   2005-03-21
//
// \verbatim
// \modifications
//     05/11/2006 Kongbin Kang
//           add seter functions for the parameters of the filter
//

#include "xmvg_atomic_filter_3d.h"
#include "xmvg_gaussian_filter_descriptor.h"
#include "xmvg_perspective_camera.h"
#include <vpgl/algo/vpgl_project.h>

#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_math.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_analytic_integrant.h>
#include <vnl/algo/vnl_simpson_integral.h>
#include <vnl/algo/vnl_adaptsimpson_integral.h>

class xmvg_gaussian_filter_3d : public xmvg_atomic_filter_3d <double>
{
  // member functions
  public:
    //: Empty Constructor
    xmvg_gaussian_filter_3d();
    //: Constructor from the descriptor
    xmvg_gaussian_filter_3d(xmvg_gaussian_filter_descriptor& descriptor);
    //: Destructor
    virtual ~xmvg_gaussian_filter_3d();
    
    //: Splats the filter centered at "centre" for a single view indexed by "index"
    xmvg_atomic_filter_2d<double> splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam);
    //: Finds the line integral along the line passing through the point and the camera
    double splat_impulse(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam, vgl_point_3d<double> point);

    //: get the descriptor
    xmvg_gaussian_filter_descriptor descriptor() { return descriptor_; }

    //: scale the filter up with factor s
    void scale_filter(double s)
    {
      descriptor_.set_sigma_r(s * descriptor_.sigma_r());
      descriptor_.set_sigma_z(s * descriptor_.sigma_z());
    }
   
  protected:

  // member variables
  public:

  protected:
    xmvg_gaussian_filter_descriptor descriptor_;
};

// XML write
void x_write(vcl_ostream& os, xmvg_gaussian_filter_3d f);

#endif
