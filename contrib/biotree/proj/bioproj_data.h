// This is /contrib/biotree/proj/bioproj_data.h

#ifndef bioproj_data_h_
#define bioproj_data_h_

//: 
// \file    bioproj_data.h
// \brief   a class for the data of bioproj
// \author  H. Can Aras
// \date    2006-07-12
// 

#include <vcl_string.h>
#include <vil3d/vil3d_image_view.h>
#include <vbl/vbl_array_3d.h>

class bioproj_data{
public:
  //: constructor
  //: read view slices one-by-one and extract the necessary (xi,theta) planes given by
  //  min_plane and max_plane parameters
  bioproj_data(vcl_string file_path, int min_plane, int max_plane, 
    int nviews, int sensor_width, int sensor_height,
    int grid_w, int grid_h, int grid_d);

  int nviews_, sensor_width_, sensor_height_;
  int min_plane_, max_plane_;
  // to hold the (theta-xi)planes
  vil3d_image_view<unsigned short> planes_;
  int planes_width_, planes_height_, planes_depth_;
  // to hold the convolved planes
  vil3d_image_view<double> convolved_;
  // to hold the response values at grid points
  int grid_w_, grid_h_, grid_d_;
  vbl_array_3d<double> grid_;
};

#endif
