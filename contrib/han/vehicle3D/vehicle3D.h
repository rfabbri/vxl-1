// This is contrib/han/vehicle3D.h
#ifndef vehicle3D_h_
#define vehicle3D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  defines 3D vehicle class constructed from video cameras
// \author Dongjin Han (han@lems.brown.edu)
// \date   4/26/2007
//
// \verbatim
//  Modifications
//   april 2007 created
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_iosfwd.h>


#include <vgl/algo/vgl_line_2d_regression.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>

#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_line_3d_2_points.h>

#include <vgl/vgl_convex.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_3x4.h>

#include <vnl/vnl_double_3.h>
#include <vul/vul_file.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>



#include <vcl_iosfwd.h>
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vgl/algo/vgl_convex_hull_2d.h>

#include <vcl_cstring.h>
#include <vcl_string.h>
//#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>

#include <mbl/mbl_stats_1d.h> 


class vehicle3D
{
  vcl_vector<vgl_point_3d<double> > pts_;
  vcl_vector<vgl_point_3d<double> > pts_front_,pts_rear_;
  
  vgl_point_3d<double> location_,center_,center1_,center2_,center3_,center4_;
  
  double radius_,radius1_,radius2_,radius3_,radius4_;
  vnl_vector_fixed<double,3> color_;
  vnl_double_3x3 R_;

 public:
  //: Default constructor
  vehicle3D(void): color_(0.0, 0.0, 0.0), radius_(1.0) {}


  
  ~vehicle3D(void){}
  vehicle3D(vcl_vector<vgl_point_3d<double> >, int pts_flag=0);

  

  void set(vcl_vector<vgl_point_3d<double> >, int pts_flag=0);
  bool contains(vgl_point_3d<double>);
  int  countIN(vcl_vector <vgl_point_3d<double> >);
  void init();
  vnl_vector <int> search(vcl_vector <vgl_point_3d<double> >, vgl_point_3d <double> );

  int read_wrl_file(vcl_vector <vgl_point_3d<double> > & ptl,vcl_string argv,bool rotation=false);
  int read_bb_box_file(vcl_string);
 
  //int read_data_file(vcl_vector <p > & ptl,vcl_string argv);
  double curvature(double=1.0,int=0);

  vcl_vector <vgl_point_3d<double> > copy_out(int flag=0);

  //void ranger(vcl_vector<vgl_point_3d<double> > pts);

  double curvature();
  double local_curvature(vgl_point_3d<double> p0,vgl_point_3d<double> p1,vgl_point_3d<double> p2)
  {
    vgl_vector_3d <double> a1(p0-p1);
    vgl_vector_3d <double> a2(p2-p1);
    double ang=angle(a1,a2);

    return ang;
  };

  vcl_vector <double> distance(int flag=0);
  double mean_length();
  double max_length();
  vgl_polygon<double> get_XY_con(int flag=0);
  void get_YZ_con();
  void get_XZ_con();
  void pts_from_pts_f_and_r();
};



#endif // vehicle3D_h_

