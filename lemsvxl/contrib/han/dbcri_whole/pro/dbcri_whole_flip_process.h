// This is brcv/mvg/dbecl/pro/dbcri_whole_flip_process.h
#ifndef dbcri_whole_flip_process_h_
#define dbcri_whole_flip_process_h_

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


#include <vector>
#include <string>
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
//#include "../tube.h"
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
  std::vector <unsigned int> triple[4];// last is for flag;
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

struct good_ratios_edge {
  int fm1,fm2,fm3;
  //double x1,x2,x3,y1,y2,y3;
  //double angle;
  std::vector < std::vector <double> > horizon; //x1,y1, x2,y2, x3,y3,angle, ratio, flag(good/bad)
  
        //double ratio;

  //int angle_index;

};

typedef struct good_ratios_edge good_ratios_edge;
class node_edge
{
  node_edge();
  ~node_edge();

  node_edge(int i,int j,int k, vgl_point_2d <double> x,vgl_point_2d <double> y,vgl_point_2d <double> z) {
    std::vector <vgl_point_2d<double> >p;

    std::vector <int> ijk;

    p.clear();//y_.clear();z_.clear(); 
    ijk_.clear();
    p.push_back(x);   p.push_back(y);   p.push_back(z);
    ijk.push_back(i); ijk.push_back(j); ijk.push_back(k);
    pt_.push_back(p);
    usage_=true;
  }

private:

  std::vector <std::vector < int > >ijk_;
  std::vector <std::vector < vgl_point_2d <double> > >pt_;
  bool usage_;

public:
  bool compare_and_add(node_edge *);


};


struct cluster {
 int index;
 // int fm1,fm2,fm3;
  //double x1,x2,x3,y1,y2,y3;
  std::vector <vgl_point_3d <double> >pts;
  vgl_point_3d <double> mean_pt;
  bool label;
  double error;//variance
  //vgl_point_3d<double> pt3d;
  int size;

};

typedef struct cluster cluster;


class dbcri_whole_flip_process : public bpro_process
{
private:
  float MARGIN_;
  int combination_;
  double epi_x_,epi_y_;
  std::vector <vsol_digital_curve_2d_sptr> dcl_;
  std::vector <std::vector <vsol_digital_curve_2d_sptr> >dcl_edge_;

  std::vector <vgl_point_2d <double> > left_pts_;
  std::vector <vgl_point_2d <double> > right_pts_;

  vgl_line_2d <double> line_upper_;
  vgl_line_2d <double> line_lower_;
  double theta_pos_,theta_neg_;
  std::vector <ratios> display_sample_front_;
  std::vector <ratios> display_sample_rear_;
  //std::vector <ratios> display_sample_fs_front_;
  //std::vector <ratios> display_sample_fs_rear_;

  double first_minus_second_;
  bool display_0_frame_, display_n_frame_;

  std::vector < std::vector <dbecl_episeg_sptr> >episeglist;
  std::vector < std::vector <dbecl_episeg_sptr> >episeglist_edge_;
  std::vector <bool> angle_f_flag_,angle_r_flag_;

  double scale_;
  std::vector <vgl_point_3d <double> > pts_3d_;
  std::vector <vgl_point_3d <double> > pts_3d_front_,pts_3d_rear_;
  vnl_double_3 min_point_,max_point_;
  vnl_double_3x3 rot_ ;
  vnl_double_3x3 inv_rot_ ;
  vnl_double_4x4 bb_xform_;

  std::vector <pts_3d_attribute> pts_3d_a_;
  std::vector <pts_3d_attribute> pts_3d_a_clustered_;
  std::vector <vsol_polygon_2d_sptr > polys_;
  std::vector <vgl_polygon<double> > veh_cons_;
  std::vector <vgl_point_3d<double> > probe_list_;
  std::vector <ratios> good_3d_curves_;
  std::vector <good_ratios_edge> network_;
  vnl_double_3 p0_,p7_;
  bool cluster_flag_;
  int ni_;
protected:
  int nframes_; 
  //: Camera intrinsic parameters
  vnl_double_3x3 K_;
   vnl_double_3x4 C_;
   vnl_double_3x4 WC_;
  //: Map from frame numbers to cameras
  //std::map<int,vnl_double_3x4> cameras_;
  std::vector<vnl_double_3x4> cameras_;
        
  vnl_vector <double> tnew_;
   sdet_detector_params dp;
public:

  dbcri_whole_flip_process();
  ~dbcri_whole_flip_process();

  dbcri_whole_flip_process(const dbcri_whole_flip_process& other);

  //: Clone the process
  virtual bpro_process* clone() const;

  std::string name();

  int input_frames();
  int output_frames();

  //pts clustering
  cluster cluster_add(cluster A,cluster B) ;
  double cluster_cmp(cluster A,cluster B, cluster &C);
  double cluster_cmp_distance(cluster A,cluster B, cluster &C);
  void cluster_copy(cluster A,cluster &B);

  double histogram_method_for_finding_cross_ratio( std::vector <ratios  > & ratios_front_list, std::vector <ratios> &ratios_rear_list);
  double histogram_method_for_finding_cross_ratio_finer( std::vector <ratios  > & );
  double cameras( int,int,int,std::vector <vsol_digital_curve_2d_sptr> &dcl,
    std::vector< std::vector <dbecl_episeg_sptr> >&episegl, dbecl_epipole_sptr &epipole);
  double cameras_accu( int,int,int,std::vector<std::vector <vsol_digital_curve_2d_sptr> >&dcl,
    std::vector< std::vector <dbecl_episeg_sptr> >&episegl, dbecl_epipole_sptr &epipole,double,double tiny_number=0.1);

  double cross_ratio(dbecl_epipole_sptr &epipole,
    dbecl_episeg_sptr & epi_seg1,dbecl_episeg_sptr & epi_seg2, dbecl_episeg_sptr & epi_seg3);

  void write_vrml(const std::string& filename,
    //std::vector<vpgl_perspective_camera<double> >& cameras,
    std::vector<vgl_point_3d<double> > world_points);

  void write_vrml_2(std::ofstream& str,//const std::string& filename,
    //std::vector<vpgl_perspective_camera<double> >& cameras,
    std::vector<vgl_point_3d<double> > world_points);


  void write_vrml_3(std::ofstream& str);


  void write_vrml_bbox( std::ofstream& str,
    const vnl_double_4x4& bbox_xform );


  void init_cameras(vnl_vector <double>,double scale=1.0);
  void d3_build(std::vector <vsol_digital_curve_2d_sptr> &dcl,std::vector< std::vector <dbecl_episeg_sptr> >&episegl);
  void d3_build();
  bool is_inwedge(vgl_point_2d<double> p, int frame) ;

  void d3_build_points( double err, std::vector <vgl_point_3d<double> > & ,bool clear=true) ;
  void d3_build_points_edge_con(  double err, std::vector <vgl_point_3d<double> > &, bool clear=true) ;

  int angle_check(double,bool);

  void d3_build_points_edge( double err, std::vector <vgl_point_3d<double> > &) ;
  void d3_build_network( double err, std::vector <vgl_point_3d<double> > &) ;
  cluster outliers(std::vector <vgl_point_3d<double> > point_3d_list,double th,cluster &);

  void get_BB_and_scale(std::vector <vgl_point_3d<double> >,std::vector <vgl_point_3d<double> > );
  void left_right_line_bb(std::vector <vgl_point_3d<double> > &point_3d_left_list,std::vector <vgl_point_3d<double> > &point_3d_right_list);

  void contour();
  void delete_contour(double thresh=0.0);
  void out_removal(std::vector <vgl_point_3d <double> > & pts);

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  void print_bb_and_camera(std::string);
  void read_WC();
  void Estimation_BB_shift_using_convex_hull_in_2D();
  vnl_double_4x4 bb_reform(double,double,double,double,double,double);
  int points_inside_of_the_box(int,vnl_double_4x4);
  void statitical_filtering(double &, std::vector <vgl_point_3d<double> >);
  void spacial_filtering(double &, std::vector <vgl_point_3d<double> >);
  void spacial_filtering_x(double &, double &, std::vector <vgl_point_3d<double> >);

  double bb_contour();
  void read_bb_and_probe(float scale, double &cube_x, double &cube_y, double &cube_z, vgl_point_3d<double> &X,int  probe_id, int class_id);
  void read_bb_and_probe_test(float scale, double &cube_x, double &cube_y, double &cube_z, vgl_point_3d<double> &X,int  probe_id, int class_id);
  void rotate_bb( vnl_double_3x3 R_in, vnl_double_3x3 & R_out,double theta_x,double theta_y,double theta_z ) ;

  double Estimation_BB_and_bb_using_convex_hull_in2D(int);
  double val_list_check(std::vector <double> );
 
  double bb_box_contour_measure(std::vector<vgl_point_3d<double> >);
  double bb_box_contour_measure(std::vector<std::vector<vgl_point_3d<double> > >);
   double bb_box_contour_measure_clip(std::vector<vgl_point_3d<double> >);

  void draw_vsol(std::vector <vnl_matrix <double> >p8_max_list);
  double generate_mean_contour_model(int);

  void epi_search(vnl_vector <double>&,float,float,float);
  void epi_search_lite(vnl_vector <double>&,float,float,float);
  vnl_matrix <int>genarate_table(int N); 
  vnl_matrix <int>read_table(int N); 

  bool execute();
  bool finish();

};

#endif // dbcri_whole_flip_process_h_
