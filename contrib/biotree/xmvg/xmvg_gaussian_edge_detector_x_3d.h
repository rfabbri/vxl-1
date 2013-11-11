// This is /contrib/biotree/xmvg/xmvg_gaussian_edge_detector_x_3d.h

#ifndef xmvg_gaussian_edge_detector_x_3d_h
#define xmvg_gaussian_edge_detector_x_3d_h

//:
// \file   xmvg_gaussian_edge_detector_x_3d.h
// \brief  3D gaussian edge detector in x direction.
// \author H. Can Aras
// \date   2005-10-03
//

#include "xmvg_atomic_filter_3d.h"
#include "xmvg_gaussian_edge_detector_descriptor.h"
#include "xmvg_perspective_camera.h"
#include <vpgl/algo/vpgl_project.h>

#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_math.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_analytic_integrant.h>
#include <vnl/algo/vnl_simpson_integral.h>
#include <vnl/algo/vnl_adaptsimpson_integral.h>

class xmvg_gaussian_edge_detector_x_3d : public xmvg_atomic_filter_3d <double>
{
  // member functions
  public:
    //: Empty Constructor
    xmvg_gaussian_edge_detector_x_3d();
    //: Constructor from the descriptor
    xmvg_gaussian_edge_detector_x_3d(xmvg_gaussian_edge_detector_descriptor& descriptor);
    //: Destructor
    virtual ~xmvg_gaussian_edge_detector_x_3d();
    
    //: Splats the filter centered at "centre" for a single view indexed by "index"
    xmvg_atomic_filter_2d<double> splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam);

    //: get the descriptor
    xmvg_gaussian_edge_detector_descriptor descriptor() { return descriptor_; }

    //: TODO implement a real scale filter
    void scale_filter(double s) {}

  protected:

  // member variables
  public:

  protected:
    xmvg_gaussian_edge_detector_descriptor descriptor_;
};

void x_write(vcl_ostream& os, xmvg_gaussian_edge_detector_x_3d detector);

#endif
