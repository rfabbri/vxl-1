// This is /contrib/biotree/xmvg/xmvg_no_noise_filter_descriptor.h

#ifndef xmvg_no_noise_filter_descriptor_h_
#define xmvg_no_noise_filter_descriptor_h_

//: 
// \file    xmvg_no_noise_filter_descriptor.h
// \brief   No noise filter descriptor
// \author  H. Can Aras
// \date    2005-02-28
// \modifications
//
//     05/11/2006 Kongbin Kang
//           add seter functions for the parameters of the filter
//
//     12/09/2005 Gamze Tunali
//           centre, orientation, rotation axis and rotation angle are 
//           carried up to the parent class (xvmg_filter_descriptor)
//           x_axis and y_axis attributes are removed.
//
// -The no noise cylinder is composed of two concentric cylinders.
//
// -The cylinder inside/outside has positive/negative values.
//
// -The radius of the outer cylinder is sqrt(2) times the radius of
//  the inner cylinder.
//
// -Later, this will be changed so that the user can specify an outer 
//  diameter different than the specified value. The weights of the 
//  outer cylinder will be set according to the radii of inner and 
//  outer cylinders.
//
// -We have a base cylinder aligned with z-axis. Before some operations
//  (splatting) of the filter class that uses this class, we will 
//  transform our system vectors accordingly so that the given filter 
//  orientation collides with positive z-axis. RIght hand rule coordinate
//  system is used.
//
//         z
//        |     y
//            /
//        | /
//         _ _ _x
//

#include <vcl_string.h>
#include "xmvg_filter_descriptor.h"
#include <vnl/vnl_double_3.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

class xmvg_no_noise_filter_descriptor : public xmvg_filter_descriptor
{
  // member functions
public:
  //: empty constructor
  xmvg_no_noise_filter_descriptor ();
  //: constructor from radius, centre and orientation
  xmvg_no_noise_filter_descriptor (double radius, double length,
                                   vgl_point_3d<double> centre, 
                                   vgl_vector_3d<double> orientation);
  ~xmvg_no_noise_filter_descriptor ();

  // getters
  //: returns inner radius of the cylinder
  double inner_radius() { return inner_radius_; }
  //: returns outer radius of the cylinder
  double outer_radius() { return outer_radius_; }

  //: set radius
  void set_inner_radius(double r) { inner_radius_ = r; }

  void set_outer_radius(double r) { outer_radius_ = r; }
  
  //: returns length of the cylinder
  double length() { return length_; }

  void set_length(double l) { length_ = l;}

private:
  //: calculates bounding box, called inside the constructor
  void calculate_bounding_box(void);

  // member variables
protected:
  double inner_radius_;
  double outer_radius_;
  double length_;

};

//: XML write
void x_write(vcl_ostream & os, xmvg_no_noise_filter_descriptor f);

#endif
