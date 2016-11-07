// This is /contrib/biotree/xmvg/xmvg_gaussian_filter_descriptor.h

#ifndef xmvg_gaussian_filter_descriptor_h_
#define xmvg_gaussian_filter_descriptor_h_

//: 
// \file    xmvg_gaussian_filter_descriptor.h
// \brief   Gaussian filter descriptor
// \author  H. Can Aras
// \date    2005-03-21
// \modifications
//     12/09/2005 Gamze Tunali
//           centre, orientation, rotation axis and rotation angle are 
//           carried up to the parent class (xvmg_filter_descriptor)
//           x_axis and y_axis attributes are removed.
//          
// 
// -The gaussian filter is realised by the following (assuming the filter
//  is aligned with positive z-axis):
//  1. The profile in a direction parallel to the x-axis is the second
//     derivative if a 1-D Gaussian.
//  2. The profile in a direction parallel to the y-axis is the second
//     derivative if a 1-D Gaussian.
//  3. The profile in a direction parallel to the z-axis is a 1-D Gaussian.
//
// -We have a base filter aligned with z-axis. Before some operations
//  (splatting) of the filter class that uses this class, we will 
//  transform our system vectors accordingly so that the given filter 
//  orientation collides with positive z-axis. Right hand rule coordinate
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

class xmvg_gaussian_filter_descriptor : public xmvg_filter_descriptor
{
  // member functions
public:
  //: empty constructor
  xmvg_gaussian_filter_descriptor ();
  //: constructor from radius, length(twice sigma_z), centre and orientation
  xmvg_gaussian_filter_descriptor (double sigma_r, double length,
                                   vgl_point_3d<double> centre, 
                                   vgl_vector_3d<double> orientation);
  ~xmvg_gaussian_filter_descriptor ();

  // getters
  //: returns sigma_r
  double sigma_r() { return sigma_r_; }

  void set_sigma_r(double r) { sigma_r_ = r; }
  
  //: returns sigma_z
  double sigma_z() { return sigma_z_; }

  void set_sigma_z(double l) { sigma_z_ = l; }


private:
  //: calculates bounding box, called inside the constructor
  void calculate_bounding_box(void);

protected:
  double sigma_r_;
  double sigma_z_;

};

//: XML 
void x_write(vcl_ostream & os, xmvg_gaussian_filter_descriptor f);
#endif
