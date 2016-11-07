// This is /contrib/biotree/proj/bioproj_mem_proc.h

#ifndef bioproj_mem_proc_h_
#define bioproj_mem_proc_h_

//: 
// \file    bioproj_mem_proc.h
// \brief   a memory efficient class for the process of bioproj
// \author  H. Can Aras
// \date    2006-09-08
// 

#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>
#include <xmvg/xmvg_perspective_camera.h>

class bioproj_mem_io;
class bioproj_filter;

enum view_filters { NU_G, NU_G_X, NU_G_Y, NU_G_XX, NU_G_XY, NU_G_YY};
enum spatial_filtes { G, G_Z, G_ZZ };


class bioproj_mem_proc{
public:
  //: constructor
  bioproj_mem_proc(bioproj_mem_io *proj_mem_io);

  //: execute the algorithm
  bool execute(int view_filter, int spatial_filter, int resize_factor = 1);

  //: convolve the current view and write in convolved current view
  void convolve_current_view(bioproj_filter* f, const int& viewno);

  void accumulate_responses_from_current_convolved_view(int viewno);

  inline double interpolate_convolved_data_at_point(vgl_point_2d<double> p);

  void apply_z_filtering();

  void write_result(vcl_string outfile);

  vcl_vector <xmvg_perspective_camera<double> > cameras_;

  bioproj_mem_io *proj_mem_io_;

  int view_filter_type_;
  int spatial_filter_type_;

  vil_image_view<double> temp_view_double_;
  vil_image_view<double> current_view_;
  vil_image_view<double> current_view_convolved_;

  //: resolution along three directions, in millimeters
  double res_x_, res_y_, res_z_;
  //: resize factor
  int resize_factor_;
};

#endif
