// This is brcv/mvg/dbecl/pro/dbcri_accu_process.h
#ifndef dbcri_accu_process_h_
#define dbcri_accu_process_h_

#define BIN_SIZE_ 100

//:
// \file
// \brief A process that converts vtol edges to dbecl episegs
// \author Dongjin Han (han@lems.brown.edu)
// \date 12/25/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_inverse.h>

#include <dbecl/dbecl_episeg.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vgl/vgl_point_3d.h>

#include <bmrf/bmrf_network.h>
#include <bmrf/bmrf_curve_3d_builder.h>
#include <bmrf/bmrf_curvel_3d.h>
#include <bmrf/bmrf_curve_3d.h>
#include <sdet/sdet_detector_params.h>
#include "../../dbcri/pro/dbcri_process.h"
#include "../tube.h"
//#include <dbecl/dbecl_episeg.h>
/*
struct ratios {
  double angle;
  int fm1,fm2,fm3;
  //double x1,x2,x3,y1,y2,y3;
  vgl_point_2d <double> pt1,pt2,pt3;
        double ratio;
  bool label;
  int bin_number;
  vgl_point_3d<double> pt3d;
  int angle_index;

};

typedef struct ratios ratios;



struct angle_flag {
  vcl_vector <unsigned int> triple[4];// last is for flag;
  //bool angle_flag;
};

typedef struct angle_flag angle_flag;

struct pts_3d_attribute {
  vgl_point_3d<double> pt3d;//
  bool position; //left false right true;
  double angle;
  double weight;
  bool usage;
  double error;
  int count;
};

typedef struct pts_3d_attribute pts_3d_attribute;
*/


/*
struct ratios {
  double angle;
  int fm1,fm2,fm3;
  //double x1,x2,x3,y1,y2,y3;
  vgl_point_2d <double> pt1,pt2,pt3;
        double ratio;

  int angle_index;

};

typedef struct ratios ratios;
*/

struct good_ratios {
  int fm1,fm2,fm3;
  //double x1,x2,x3,y1,y2,y3;
  //double angle;
  vcl_vector < vcl_vector <double> > horizon; //x1,y1, x2,y2, x3,y3,angle, ratio, flag(good/bad)
  
        //double ratio;

  //int angle_index;

};

typedef struct good_ratios good_ratios;
class node
{
  node();
  ~node();

  node(int i,int j,int k, vgl_point_2d <double> x,vgl_point_2d <double> y,vgl_point_2d <double> z) {
    vcl_vector <vgl_point_2d<double> >p;

    vcl_vector <int> ijk;

    p.clear();//y_.clear();z_.clear(); 
    ijk_.clear();
    p.push_back(x);   p.push_back(y);   p.push_back(z);
    ijk.push_back(i); ijk.push_back(j); ijk.push_back(k);
    pt_.push_back(p);
    usage_=true;
  }

private:

  vcl_vector <vcl_vector < int > >ijk_;
  vcl_vector <vcl_vector < vgl_point_2d <double> > >pt_;
  bool usage_;

public:
  bool compare_and_add(node *);


};
class dbcri_accu_process : public bpro_process
{
private:
  int combination_;
  double epi_x_,epi_y_;
  vcl_vector <vsol_digital_curve_2d_sptr> dcl_;
  vcl_vector <vcl_vector <vsol_digital_curve_2d_sptr> >dcl_edge_;

  vcl_vector <vgl_point_2d <double> > left_pts_;
  vcl_vector <vgl_point_2d <double> > right_pts_;

  vgl_line_2d <double> line_upper_;
  vgl_line_2d <double> line_lower_;
  double theta_pos_,theta_neg_;
  vcl_vector <ratios> display_sample_front_;
  vcl_vector <ratios> display_sample_rear_;
  //vcl_vector <ratios> display_sample_fs_front_;
  //vcl_vector <ratios> display_sample_fs_rear_;


  bool display_0_frame_, display_n_frame_;

  vcl_vector < vcl_vector <dbecl_episeg_sptr> >episeglist;
  vcl_vector < vcl_vector <dbecl_episeg_sptr> >episeglist_edge_;
  vcl_vector <bool> angle_f_flag_,angle_r_flag_;

  double scale_;
  vcl_vector <vgl_point_3d <double> > pts_3d_;
  vcl_vector <vgl_point_3d <double> > pts_3d_front_,pts_3d_rear_;
  vnl_double_3 min_point_,max_point_;
  vnl_double_3x3 rot_ ;
  vnl_double_3x3 inv_rot_ ;
  vnl_double_4x4 bb_xform_;

  vcl_vector <pts_3d_attribute> pts_3d_a_;
  vcl_vector <vsol_polygon_2d_sptr > polys_;
  vcl_vector <vgl_polygon<double> > veh_cons_;
  vcl_vector <vgl_point_3d<double> > probe_list_;
  vcl_vector <ratios> good_3d_curves_;
  vcl_vector <good_ratios> network_;
  vnl_double_3 p0_,p7_;
protected:
  int nframes_; 
  //: Camera intrinsic parameters
  vnl_double_3x3 K_;
   vnl_double_3x4 C_;
   vnl_double_3x4 WC_;
  //: Map from frame numbers to cameras
  //vcl_map<int,vnl_double_3x4> cameras_;
  vcl_vector<vnl_double_3x4> cameras_;
        
  vnl_vector <double> tnew_;
   sdet_detector_params dp;
public:

  dbcri_accu_process();
  ~dbcri_accu_process();

  dbcri_accu_process(const dbcri_accu_process& other);

  //: Clone the process
  virtual bpro_process* clone() const;
  
  vcl_string name();

  int input_frames();
  int output_frames();

  double histogram_method_for_finding_cross_ratio( vcl_vector <ratios  > & ratios_front_list, vcl_vector <ratios> &ratios_rear_list);
   double histogram_method_for_finding_cross_ratio_finer( vcl_vector <ratios  > & );
 double cameras( int,int,int,vcl_vector <vsol_digital_curve_2d_sptr> &dcl,
    vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl, dbecl_epipole_sptr &epipole);
 double cameras_accu( int,int,int,vcl_vector<vcl_vector <vsol_digital_curve_2d_sptr> >&dcl,
    vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl, dbecl_epipole_sptr &epipole,double,double tiny_number=0.1);

  double cross_ratio(dbecl_epipole_sptr &epipole,
   dbecl_episeg_sptr & epi_seg1,dbecl_episeg_sptr & epi_seg2, dbecl_episeg_sptr & epi_seg3);

  void write_vrml(const vcl_string& filename,
                                //vcl_vector<vpgl_perspective_camera<double> >& cameras,
                                vcl_vector<vgl_point_3d<double> > world_points);

  void write_vrml_2(vcl_ofstream& str,//const vcl_string& filename,
                                //vcl_vector<vpgl_perspective_camera<double> >& cameras,
                                vcl_vector<vgl_point_3d<double> > world_points);


  void write_vrml_bbox( vcl_ofstream& str,
                      const vnl_double_4x4& bbox_xform );


  void init_cameras(vnl_vector <double>,double scale=1.0);
  void d3_build(vcl_vector <vsol_digital_curve_2d_sptr> &dcl,vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl);
  void d3_build();
  bool is_inwedge(vgl_point_2d<double> p, int frame) ;

  void d3_build_points( double err, vcl_vector <vgl_point_3d<double> > &) ;
  void d3_build_points_edge( double err, vcl_vector <vgl_point_3d<double> > &) ;
  void d3_build_network( double err, vcl_vector <vgl_point_3d<double> > &) ;
  void outliers(vcl_vector <vgl_point_3d<double> > point_3d_list);

  void get_BB_and_scale(vcl_vector <vgl_point_3d<double> >,vcl_vector <vgl_point_3d<double> > );
  void left_right_line_bb(vcl_vector <vgl_point_3d<double> > &point_3d_left_list,vcl_vector <vgl_point_3d<double> > &point_3d_right_list);

  void contour();
  void delete_contour(double thresh=0.0);

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  void print_bb_and_camera();
  void read_WC();
  void Estimation_BB_shift_using_convex_hull_in_2D();
  vnl_double_4x4 bb_reform(double,double,double,double,double,double);
  int points_inside_of_the_box(int,vnl_double_4x4);
  void statitical_filtering(double &, vcl_vector <vgl_point_3d<double> >);
  void spacial_filtering(double &, vcl_vector <vgl_point_3d<double> >);
  void spacial_filtering_x(double &, double &, vcl_vector <vgl_point_3d<double> >);

  double bb_contour();
  void read_bb_and_probe(float scale, double &cube_x, double &cube_y, double &cube_z, vgl_point_3d<double> &X,int  probe_id, int class_id);
  void read_bb_and_probe_test(float scale, double &cube_x, double &cube_y, double &cube_z, vgl_point_3d<double> &X,int  probe_id, int class_id);
  void rotate_bb( vnl_double_3x3 R_in, vnl_double_3x3 & R_out,double theta_x,double theta_y,double theta_z ) ;

  double Estimation_BB_and_bb_using_convex_hull_in2D(int);
  double val_list_check(vcl_vector <double> );
 
  double bb_box_contour_measure(vcl_vector<vgl_point_3d<double> >);
  double bb_box_contour_measure(vcl_vector<vcl_vector<vgl_point_3d<double> > >);
   double bb_box_contour_measure_clip(vcl_vector<vgl_point_3d<double> >);

  void draw_vsol(vcl_vector <vnl_matrix <double> >p8_max_list);
  double generate_mean_contour_model(int);

  bool execute();
  bool finish();

};

#endif // dbcri_accu_process_h_
