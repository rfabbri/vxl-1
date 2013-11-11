// This is /contrib/biotree/xmvg/xmvg_filter_descriptor.h

#ifndef xmvg_filter_descriptor_h_
#define xmvg_filter_descriptor_h_

//: 
// \file    xmvg_filter_descriptor.h
// \brief   a class containing the mapping sequence order of atomic filters to a meaningful arrangement.
// \author  Kongbin Kang and H. Can Aras
// \date    2005-02-08
// 
#include <vcl_string.h>
#include <vgl/vgl_box_3d.h> 
#include <vgl/vgl_point_3d.h> 
#include <vnl/vnl_double_3.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <vnl/xio/vnl_xio_vector_fixed.h>

class xmvg_filter_descriptor{
  public:
    //: constructor
    xmvg_filter_descriptor(const vgl_box_3d<double> & box, 
                           const vcl_string& name)
      : box_(box), name_(name){ }  

    //: empty constructor
    xmvg_filter_descriptor() {}

    //: empty destructor
    virtual ~xmvg_filter_descriptor() {}

    //: get name string
    vcl_string name() { return name_; }

    //: get the box containing the filter
    vgl_box_3d<double> box() { return box_; }

    //: returns centre of the cylinder
    vgl_point_3d<double> centre() { return centre_; }

    //: returns orientation of the cylinder
    vgl_vector_3d<double> orientation() const { return orientation_; }

    //: returns rotation angle to bring the cylinder orientation from z-axis
    //  to the orientation of the cylinder
    double rotation_angle() { return rotation_angle_; }

    //: returns rotation axis to bring the cylinder orientation from z-axis
    //  to the orientation of the cylinder
    vnl_double_3 rotation_axis() { return rotation_axis_; }

    //: set orientation
    void set_orientation(vgl_vector_3d<double> const& dir)
    {
      orientation_ = dir;
    }
      

  protected:
    vgl_box_3d<double> box_;
    vcl_string name_;

    vgl_point_3d<double> centre_;
    vgl_vector_3d<double> orientation_;

  protected: // implementation related variable 
    //: The angle from (0,0,1) vector to the orientation of the cylinder (counter-clockwise)
    double rotation_angle_;

    //: The axis to rotate around for bringing the cylinder orientation from (0,0,1) 
    //  to its actual value
    vnl_vector_fixed<double,3> rotation_axis_;

private:
    virtual void calculate_bounding_box(void) {}
};

void x_write(vcl_ostream & os, xmvg_filter_descriptor f);

#endif
