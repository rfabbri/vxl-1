// This is /contrib/biotree/proj/bioproj_proc.h

#ifndef bioproj_proc_h_
#define bioproj_proc_h_

//: 
// \file    bioproj_proc.h
// \brief   a class for the process of bioproj
// \author  H. Can Aras
// \date    2006-07-12
// 

#include "bioproj_io.h"
#include "bioproj_data.h"
#include "bioproj_nu_g_filter.h"
#include "bioproj_nu_gx_filter.h"
#include "bioproj_nu_gy_filter.h"
#include "bioproj_nu_gxx_filter.h"
#include "bioproj_nu_gxy_filter.h"
#include "bioproj_nu_gyy_filter.h"
#include "bioproj_g_filter.h"
#include "bioproj_gz_filter.h"
#include "bioproj_gzz_filter.h"

enum view_filters { NU_G, NU_G_X, NU_G_Y, NU_G_XX, NU_G_XY, NU_G_YY};
enum spatial_filtes { G, G_Z, G_ZZ };

class bioproj_proc{
public:
  //: constructor
  bioproj_proc(bioproj_io *proj_io);

  //: execute the algorithm
  void execute(int view_filter, int spatial_filter, vcl_string outfile);

  void view_space_ops_with_fixed_magnification();
  double response_at_point(vgl_point_3d<double> p);
  double interpolate_convolved_data_at_point(vgl_point_2d<double> p, int viewno);
  void apply_z_filtering();
  void write_result(vcl_string outfile);
  void print_error_message(int error_code);

  vcl_vector <xmvg_perspective_camera<double> > cameras_;

  bioproj_io *proj_io_;
  bioproj_data *proj_data_;

  int view_filter_type_;
  int spatial_filter_type_;

  // flag to notify that common structures have already been created for
  // simultaneous execute calls using the same object
  bool structures_created;

  // saving grid point coordinates not to get effected by floating point
  // inaccuracies between simultaneous execute calls using the same object
  int grid_w_, grid_h_, grid_d_;
  vbl_array_3d<double> coord_x_;
  vbl_array_3d<double> coord_y_;
  vbl_array_3d<double> coord_z_;

  int min_plane_index_, max_plane_index_;
};

#endif
