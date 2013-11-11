// This is brcv/mvg/dbecl/pro/dbcri_whole_process.h
#ifndef dbcri_whole_process_h_
#define dbcri_whole_process_h_

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
#include <bpro/bpro_parameters.h>


#include <vidpro/process/vidpro_VD_edge_process.h>

#include <vul/vul_timer.h>
#include <vul/vul_get_timestamp.h>


#include <bpro/bpro_parameters.h>

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vidpro/storage/vidpro_vtol_storage.h>
#include <vidpro/storage/vidpro_vtol_storage_sptr.h>


#include <vidpro/storage/vidpro_vtol_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/algo/vil_gauss_filter.h>


#include <vil/vil_convert.h>

//required because sdet still uses vil1
#include <vil1/vil1_vil.h>
#include <vil1/vil1_memory_image_of.h>


#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vsol/vsol_point_2d.h>

#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h> 
#include <vsol/vsol_polygon_2d.h>
#include <brip/brip_vil_float_ops.h>

#include <dbecl/dbecl_episeg_from_curve_converter.h>
#include <vsol/vsol_digital_curve_2d.h>

//#include <vgl/algo/vgl_fit_lines_2d.h> //line segment
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>


#include <dbecl/dbecl_episeg.h>
#include <dbecl/dbecl_episeg_point.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_identity_3x3.h>
#include <vnl/algo/vnl_svd.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <vcl_limits.h>

#include <vcl_cstring.h>
#include <vcl_string.h>
//#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>



#include <bmvl/brct/brct_algos.h>


#include <vnl/vnl_math.h> // for vnl_math_isnan()


#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_closest_point.h>

#include <bsol/bsol_algs.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_detector_params.h>



#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_float_3.h>
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

struct good_ratios_edge {
  int fm1,fm2,fm3;
  //double x1,x2,x3,y1,y2,y3;
  //double angle;
  vcl_vector < vcl_vector <double> > horizon; //x1,y1, x2,y2, x3,y3,angle, ratio, flag(good/bad)
  
        //double ratio;

  //int angle_index;

};

typedef struct good_ratios_edge good_ratios_edge;
class node_edge
{
  node_edge();
  ~node_edge();

  node_edge(int i,int j,int k, vgl_point_2d <double> x,vgl_point_2d <double> y,vgl_point_2d <double> z) {
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
  bool compare_and_add(node_edge *);


};


struct cluster {
 int index;
 // int fm1,fm2,fm3;
  //double x1,x2,x3,y1,y2,y3;
  vcl_vector <vgl_point_3d <double> >pts;
  vgl_point_3d <double> mean_pt;
  bool label;
  double error;//variance
  //vgl_point_3d<double> pt3d;
  int size;

};

typedef struct cluster cluster;


class dbcri_whole_process : public bpro_process
{
private:
  float MARGIN_;
  int combination_;
  double epi_x_,epi_y_;
  double epi_x_WC_,epi_y_WC_;
  vcl_vector <vsol_digital_curve_2d_sptr> dcl_;
  vcl_vector <vcl_vector <vsol_digital_curve_2d_sptr> >dcl_edge_;
  vcl_vector <vcl_vector <vsol_digital_curve_2d_sptr> >veh_con_edge_;

  vcl_vector <vgl_point_2d <double> > left_pts_;
  vcl_vector <vgl_point_2d <double> > right_pts_;

  vgl_line_2d <double> line_upper_;
  vgl_line_2d <double> line_lower_;
  double theta_pos_,theta_neg_;
  vcl_vector <ratios> display_sample_front_;
  vcl_vector <ratios> display_sample_rear_;
  //vcl_vector <ratios> display_sample_fs_front_;
  //vcl_vector <ratios> display_sample_fs_rear_;

  double first_minus_second_;
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
  vcl_vector <pts_3d_attribute> pts_3d_a_clustered_;
  vcl_vector <vsol_polygon_2d_sptr > polys_;
  vcl_vector <vgl_polygon<double> > veh_cons_;
  vcl_vector <vgl_point_3d<double> > probe_list_;
  vcl_vector <ratios> good_3d_curves_;
  vcl_vector <good_ratios_edge> network_;

  vnl_double_3 p0_,p7_,p8_,p2_,p_cabin_length_; //p2 cabin end
  vnl_double_3 p0_con_,p7_con_;
  
  bool cluster_flag_;
  double mean_y_1_,mean_y_2_;
  vcl_vector <vgl_point_2d <double> >pts_upper_line_;
  vcl_vector <vgl_point_2d <double> >pts_lower_line_;
  double ang_;
  vnl_vector <double> solve_z_;
  bool world_color_flag_;
  float r_world_,g_world_,b_world_;
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

  dbcri_whole_process();
  ~dbcri_whole_process();

  dbcri_whole_process(const dbcri_whole_process& other);

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  //pts clustering
  cluster cluster_add(cluster A,cluster B) ;
  double cluster_cmp(cluster A,cluster B, cluster &C);
  double cluster_cmp_distance(cluster A,cluster B, cluster &C);
  void cluster_copy(cluster A,cluster &B);

  double histogram_method_for_finding_cross_ratio( vcl_vector <ratios  > & ratios_front_list, vcl_vector <ratios> &ratios_rear_list);
  double histogram_method_for_finding_cross_ratio_finer( vcl_vector <ratios  > & );
  double cameras( int,int,int,vcl_vector <vsol_digital_curve_2d_sptr> &dcl,
    vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl, dbecl_epipole_sptr &epipole);
  double cameras_accu( int,int,int,vcl_vector<vcl_vector <vsol_digital_curve_2d_sptr> >&dcl,
    vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl, dbecl_epipole_sptr &epipole,double,double tiny_number=0.1);

  double cameras_fast( int,int,int,vcl_vector <vsol_digital_curve_2d_sptr> &dcl,
    vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl, dbecl_epipole_sptr &epipole);

  double cameras_accu_fast( const vcl_vector < vcl_vector <vcl_vector <vnl_float_3> > >& ,int,int,int,vcl_vector<vcl_vector <vsol_digital_curve_2d_sptr> >&dcl,
    vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl, dbecl_epipole_sptr &epipole,double,double tiny_number=0.1);
  double episeg_simplify( vcl_vector <vcl_vector <vcl_vector <vnl_float_3> > >&);


  double cross_ratio(dbecl_epipole_sptr &epipole,
    dbecl_episeg_sptr & epi_seg1,dbecl_episeg_sptr & epi_seg2, dbecl_episeg_sptr & epi_seg3);

  void write_vrml(const vcl_string& filename,
    //vcl_vector<vpgl_perspective_camera<double> >& cameras,
    vcl_vector<vgl_point_3d<double> > world_points);

  void write_vrml_2(vcl_ofstream& str,//const vcl_string& filename,
    //vcl_vector<vpgl_perspective_camera<double> >& cameras,
    vcl_vector<vgl_point_3d<double> > world_points);


  void write_vrml_3(vcl_ofstream& str);


  void write_vrml_bbox( vcl_ofstream& str,
    const vnl_double_4x4& bbox_xform );


  void init_cameras(vnl_vector <double>,double scale=1.0);
  void d3_build(vcl_vector <vsol_digital_curve_2d_sptr> &dcl,vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl);
  void d3_build();
  bool is_inwedge(vgl_point_2d<double> p, int frame) ;

  void d3_build_points( double err, vcl_vector <vgl_point_3d<double> > & ,bool clear=true) ;
  void d3_build_points_edge_con(  double err, vcl_vector <vgl_point_3d<double> > &, bool clear=true) ;
  void d3_build_points_edge_con_inside(  double err, vcl_vector <vgl_point_3d<double> > &, bool clear=true) ;

  int angle_check(double,bool);

  void d3_build_points_edge( double err, vcl_vector <vgl_point_3d<double> > &) ;
  void d3_build_network( double err, vcl_vector <vgl_point_3d<double> > &) ;
  cluster outliers(vcl_vector <vgl_point_3d<double> > point_3d_list,double th,cluster &);

  void get_BB_and_scale(vcl_vector <vgl_point_3d<double> >,vcl_vector <vgl_point_3d<double> > );
  void left_right_line_bb(vcl_vector <vgl_point_3d<double> > &point_3d_left_list,vcl_vector <vgl_point_3d<double> > &point_3d_right_list);

  void contour();
  void delete_contour(double thresh=0.0);
  void delete_contour_05(double thresh=0.0);
  void out_removal(vcl_vector <vgl_point_3d <double> > & pts);
  void out_removal_1(vcl_vector <vgl_point_3d <double> > & pts);

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  void print_bb_and_camera(vcl_string);
  void print_con_probe(vcl_string);

  void read_WC();
  void Estimation_BB_shift_using_convex_hull_in_2D();
  vnl_double_4x4 bb_reform(double,double,double,double,double,double);
  int points_inside_of_the_box(int,vnl_double_4x4);
  void statitical_filtering(double &, vcl_vector <vgl_point_3d<double> >);
  void spacial_filtering(double &, vcl_vector <vgl_point_3d<double> >);
  void spacial_filtering_x(double &, double &, vcl_vector <vgl_point_3d<double> >);
  void spacial_filtering_x_with_z(double &, double &, vcl_vector <vgl_point_3d<double> >);

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

  double epi_search(vnl_vector <double>&,float,float,float);
  double epi_search_lite(vnl_vector <double>&,float,float,float);
  double epi_search_fast(vnl_vector <double>&,float,float,float);
  double epi_search_fast_lite(vnl_vector <double>&,float,float,float);
  bool read_epi_search();
  vnl_vector <double>  epi_constraint_on_the_plane(
  double epi_x, double epi_y, double search_range, double search_step_size);

  vnl_matrix <int>genarate_table(int N); 
  vnl_matrix <int>read_table(int N); 

  float solve_z(vcl_vector <vgl_point_2d <double> >,vcl_vector <vgl_point_2d <double> >);
  float solve_front_windshield(vcl_vector <vgl_point_2d <double> >,vcl_vector <vgl_point_2d <double> >);
  float solve_front_windshield();
  float solve_front_windshield1();
  float solve_pickup();

  void angle_modify( );

  bool execute();
  bool finish();


  //in drcri_whole_3D file
    double cameras_fast1( int,int,int,vcl_vector <vsol_digital_curve_2d_sptr> &dcl,
    vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl, dbecl_epipole_sptr &epipole);
    void read_edge_map();

};

#endif // dbcri_whole_process_h_
