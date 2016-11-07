// This is /contrib/biotree/xmvg/xmvg_no_noise_filter_3d.h

#ifndef xmvg_no_noise_filter_3d_h
#define xmvg_no_noise_filter_3d_h

//:
// \file   xmvg_no_noise_filter.h
// \brief  3D no noise filter.
// \author H. Can Aras
// \date   2005-03-08
//
// \modifications
//     05/11/2006 Kongbin Kang
//           add function for scale up the filter size
//

#include "xmvg_atomic_filter_3d.h"
#include "xmvg_no_noise_filter_descriptor.h"
#include "xmvg_perspective_camera.h"
#include <vpgl/algo/vpgl_project.h>

#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_math.h>
#include <vcl_cstdio.h>

class xmvg_no_noise_filter_3d : public xmvg_atomic_filter_3d <double>
{
  // member functions
  public:
    //: Empty Constructor
    xmvg_no_noise_filter_3d();
    //: Constructor from the descriptor
    xmvg_no_noise_filter_3d(xmvg_no_noise_filter_descriptor& descriptor);
    //: Destructor
    virtual ~xmvg_no_noise_filter_3d();
    
    //: Splats the filter centered at "centre" for a single view indexed by "index"
    xmvg_atomic_filter_2d<double> splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam);

    //: Calculates the line integral along a ray specified with a start position (source position)
    //  and a direction (source-to-sensor point direction) for the given cylinder descriptor.
    //  It assumes that the cylinder orientation is aligned with the positive z-axis.
    //  Hence, other parameters should be pre-processed properly before calling this function.
    //  The function finds the intersection points of the ray and the cylinder, and computes
    //  the distance between these points, finally returning it as the line integral value.
    //  Note that the ray direction vector is limited to an angle variation of (-pi/2,pi/2) since
    //  the sensor is always in front of the source, not at the back and the image sample is
    //  in between the source and the sensor
    double line_integral(vgl_point_3d<double> ray_start, vgl_vector_3d<double> ray_direction, double radius);

    //: get the descriptor
    xmvg_no_noise_filter_descriptor descriptor() { return descriptor_; }

    void scale_filter(double s)
    {
      descriptor_.set_inner_radius(s * descriptor_.inner_radius());
      descriptor_.set_outer_radius(s * descriptor_.outer_radius());
      descriptor_.set_length(s * descriptor_.length());
    }
 
  protected:

  // member variables
  public:

  protected:
    xmvg_no_noise_filter_descriptor descriptor_;
    //: Used in splat() to find the bounding box for the 2d splat; for testing purposes, need to be able to have alternative ways of doing this.

   virtual vgl_box_2d<int> get_box_2d(xmvg_perspective_camera<double>  cam,
                                      vgl_box_3d<double> const & box);
};

void x_write(vcl_ostream& os, xmvg_no_noise_filter_3d f);
#endif
