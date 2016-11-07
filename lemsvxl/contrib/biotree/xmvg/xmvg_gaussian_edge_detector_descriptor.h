// This is /contrib/biotree/xmvg/xmvg_gaussian_edge_detector_descriptor.h

#ifndef xmvg_gaussian_edge_detector_descriptor_h_
#define xmvg_gaussian_edge_detector_descriptor_h_

//: 
// \file    xmvg_gaussian_edge_detector_descriptor.h
// \brief   Gaussian edge detector descriptor
// \author  H. Can Aras
// \date    2005-10-02
// 

#include <vcl_string.h>
#include "xmvg_filter_descriptor.h"
#include <vnl/vnl_double_3.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

class xmvg_gaussian_edge_detector_descriptor : public xmvg_filter_descriptor
{
  // member functions
public:
  //: empty constructor
  xmvg_gaussian_edge_detector_descriptor ();
  //: constructor from radius, centre and orientation
  xmvg_gaussian_edge_detector_descriptor (double sigma, vgl_point_3d<double> centre);
  ~xmvg_gaussian_edge_detector_descriptor ();

  // getters
  //: returns sigma
  double sigma() { return sigma_; }
  //: returns centre of the cylinder
  vgl_point_3d<double> centre() { return centre_; };

  vgl_box_3d<double> bounding_box() { return box_; }
  // there will be no setter functions

protected:

private:
  //: calculates bounding box, called inside the constructor
  void calculate_bounding_box(void);

  // member variables
public:

protected:
  double sigma_;

  vgl_point_3d<double> centre_;
  
private:
};

void x_write(vcl_ostream& os, xmvg_gaussian_edge_detector_descriptor d, vcl_string name);

#endif
