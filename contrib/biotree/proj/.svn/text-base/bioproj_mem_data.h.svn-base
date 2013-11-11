// This is /contrib/biotree/proj/bioproj_mem_data.h

#ifndef bioproj_mem_data_h_
#define bioproj_mem_data_h_

//: 
// \file    bioproj_mem_data.h
// \brief   a memory efficient class for the data of bioproj
// \author  H. Can Aras
// \date    2006-09-08
// 

#include <vcl_string.h>
#include <vil3d/vil3d_image_view.h>
#include <vbl/vbl_array_3d.h>
#include <vcl_vector.h>

class bioproj_mem_data{
public:
  //: constructor
  bioproj_mem_data(vcl_string file_path, int nviews, 
                   int sensor_width, int sensor_height,
                   int grid_w, int grid_h, int grid_d);

  int nviews_, sensor_width_, sensor_height_;
  //: to hold the response values at grid points
  int grid_w_, grid_h_, grid_d_;
  vbl_array_3d<float> grid_;
  //: parsed filenames
  vcl_vector<vcl_string> filenames_;
};

#endif
