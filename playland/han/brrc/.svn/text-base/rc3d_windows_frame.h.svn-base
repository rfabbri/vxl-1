#ifndef rc3d_windows_frame_h_
#define rc3d_windows_frame_h_
#define PN_ 14
//:
// \file
// \brief Manager for multiview 3d reconstruction gui applications
// \author Dongjin Han 09-19-03

#include <vcl_string.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_easy3D_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_composite_tableau_sptr.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h>
#include <vgui/internals/vgui_accelerate.h>

#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_picker_tableau.h>
#include <bgui/bgui_picker_tableau_sptr.h>

#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vsol/vsol_curve_2d.h>
// pilou's line
//#include <bdgl/bdgl_curve_description.h>
//#include <bdgl/bdgl_curve_tracker.h>
//#include <bdgl/bdgl_curve_tracker_primitive.h>
//#include <bdgl/bdgl_curve_matcher.h>
#include <bbas/bdgl/bdgl_curve_algs.h>
#include <bbas/bdgl/bdgl_curve_region.h>
#include <bbas/bdgl/dll.h>
#include <bbas/bsol/bsol_intrinsic_curve_2d.h>
#include <bbas/bsol/bsol_intrinsic_curve_2d_sptr.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgui/vgui_soview2D.h>
#include <bmvl/brct/kalman_filter.h>

#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

#include <vnl/vnl_double_2.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>


#include <vnl/vnl_double_4x4.h>

//#include "c:/klt/pnmio.h"
//#include "c:/klt/klt.h"
//#include <stdio.h>  // printf
//#include "c:/klt/KltExamples.h"

//#include "c:/klt/klt_util.h"
//#include  "c:/klt/base.h" 

//#include "c:/klt/pyramid.h"

//#include "c:/klt/StdAfx.h"

//#include "c:/klt/convolve.h"
//#include "c:/klt/error.h"


struct tripplet_cov_set
{   
   bool use;
   int id0,id1,id2;
   //vcl_vector <vnl_matrix <float> > S3;
   vcl_vector <vnl_matrix <float> > Sl_;
   vcl_vector <vnl_matrix <float> >  ml_;
   vcl_vector <int> countl_;
   vcl_vector <float> detl_;
   vcl_vector <vnl_matrix <float> > S_invl_;
   float mi;
   //int j;
   //less_cost_knn(int newj) {j=newj;}
   //bool operator()(vnl_matrix<float> x, vnl_matrix<float> y)
   //{ return x[j][3] < y[j][3]; }
};

struct tripplet_selection_list 
{
   int id0,id1,id2;
   float error0,error1,error2;
   float p0,p1,p2;
   vcl_vector <int> set;
};
typedef struct tripplet_selection_list tripplet_selection_list;
struct p_list_set 
{
   int probe_number;
   vcl_vector <vnl_matrix <float>  >p_list;
   int class_id;
   float scale;
};

struct feat_ 
{   
   float theta_;
   float length_;
   float ratio_;
   float slope_;
   int id_;
   int type_; //single=1 double=2 tripple=3..
   int whos_[3];
   bool flag_;
   vcl_string explantaion_;
   //less_cost_knn(int newj) {j=newj;}
   //bool operator()(vnl_matrix<float> x, vnl_matrix<float> y)
   //{ return x[j][3] < y[j][3]; }
};
typedef struct feat_ feat_;
class vgui_window;

class BB1{
public:
  
  BB1(){}
  BB1(vnl_double_4x4 RT){RT_=RT;}
  ~BB1(){}
  
  void read(vnl_double_4x4 RT);
  
  void read(double mar_x,double mar_y,double mar_z);
  
  double x_p_,x_n_;
  double y_p_,y_n_;
  double z_p_,z_n_;

  vnl_vector_fixed <double,4 > b0_; //front
  vnl_vector_fixed <double,4 > b1_; //rear
  vnl_vector_fixed <double,4 > b2_; //near
  vnl_vector_fixed <double,4 > b3_; //far
  vnl_vector_fixed <double,4 > b4_; //bottom
  vnl_vector_fixed <double,4 > b5_;

  double Rx,Ry,Rz,Mx,My,Mz;
private:
  
  vnl_double_4x4 RT_;
  //double margin_x_,margin_y_,margin_z_;
  
};

struct singletons {
        int id0,id1;
        float mean_class0,mean_class1,mean_class2;
        float var_class0,var_class1,var_class2;
};
typedef struct singletons singletons;


struct batch {
 vcl_string file_name;
 int file_mode;
 float x,y,z;
};


//: A manager for displaying segmentation results.
class rc3d_windows_frame : public vgui_wrapper_tableau
{
 public: 

   bool abandon_configuration(int,int,int);
//   void test();
   vgl_point_3d <double> cad_2_world(vgl_point_3d <double>X) ;
   
   vgl_point_3d <double> world_2_cad(vgl_point_3d <double>X) ;
   
   
   void save_images();
   void add_vertex(int,float,float,int,int);
         void batch();
         void batch_front_windshield();

   
   float dtr_fws_geometric_constraint(vnl_vector_fixed <float,3>,vnl_vector_fixed <float,3>,
     vnl_vector_fixed <float,3>,vnl_vector_fixed <float,3>);
   vnl_matrix <double> detector_front_windshield(double , double , double, double,double , int );
   vnl_matrix <double> detector_front_windshield_adjust(double , double , double, double,double , int );
   vnl_matrix <double> detector_hood(double , double , double, double,double , int );


         void slope_detection(vgl_point_3d<double>, vgl_point_3d<double>,float &);
   void monte();
   void cam_copy();
   void klth();
   void klth_epi();

         vcl_vector <vnl_matrix <float> > p_con_test(vcl_vector <vnl_matrix <float> >);//,vcl_vector <bsol_intrinsic_curve_2d_sptr>);
         vcl_vector <vgl_point_3d<double> > point_;
         singletons sing_[PN_][PN_];

         int singleton_usage_[PN_][PN_];

   void singleton_recount();

         void marginal(double,bool);
         void marginal(double,double,double);
         void test();

   void singleton(vnl_matrix <float> pts1,
                           vnl_matrix <float> pts2,
                           vnl_matrix <float> pts3,
                           unsigned id0,
                           unsigned id1,
                           unsigned id2,
                           bool good_distribution,bool debug, singletons &);
   void Auto_BB_1();
   void BB_base(int probe_index, vgl_point_3d<double> &X,
              double &cube_x,double &cube_y,double &cube_z,
                                 double &cube_x_pos,double &cube_x_neg,
                                double & cube_y_pos,double &cube_y_neg,
                                 double &cube_z_pos,double &cube_z_neg,
                                 double &cube_x_times);
   void cut(double &cube_x_pos,
                             double &cube_x_neg,
                            double &cube_y_pos,
                             double &cube_y_neg,
                             double &cube_z_pos,
                             double &cube_z_neg,
                             vgl_point_3d<double> X
                             ,BB1 &b,float);
   bool BB_inside(vgl_point_3d<double>);
   void rotate_bb( vnl_double_3x3 , vnl_double_3x3 &,double,double,double);
     void Front_Plane();
     void rotate_and_shift();
 void read_iv();
 void read_vrml();

    vnl_matrix <float> p_error(vnl_matrix<float>,vnl_matrix<float>,vnl_matrix<float>,int,int,int);
    void rotate_back();
    
    float d3_point_distance(vnl_matrix <float>p1, vnl_matrix <float> p2);
    float back_ground_clustering(vcl_vector <float>);
    void back_ground();
    
    vnl_vector <float> han(vnl_matrix <float>,vnl_matrix <float>,
       vnl_matrix <float>,vnl_matrix <float>,vnl_matrix <float>,
       vnl_matrix <float>,vnl_matrix <float>,vnl_matrix <float>,
       vnl_matrix <float>,vnl_matrix <float>,float, bool);
    vnl_vector <float> hanptr(vnl_matrix <float>*,vnl_matrix <float>*,
      vnl_matrix <float>*,vnl_matrix <float>*,vnl_matrix <float>*,
      vnl_matrix <float>*,vnl_matrix <float>*,vnl_matrix <float> *,
      vnl_matrix <float>*,vnl_matrix <float>*,float, bool);
    vnl_vector <float> hanptr_fast(vnl_matrix <float>*,vnl_matrix <float>*,
      vnl_matrix <float>*,vnl_matrix <float>*,vnl_matrix <float>*,
      vnl_matrix <float>*,vnl_matrix <float>*,vnl_matrix <float> *,
      vnl_matrix <float>*,vnl_matrix <float>*,float, bool);
    
    void IR_mxl();
    void IR_mxl_monte();
                void IR_mxl_monte_carlo();      void IR_mxl_monte_carlo_2view();
    void IR_mxl_monte_sum();

    int IR_mxl(float scale,
                         int survival_number);
    double geometric_constraint(vcl_vector <vnl_matrix <float> > p,float scale);
    void print_p_list(vcl_vector <vnl_matrix <float> > p_list); 
    void manual_reconstruction_on_epiline_debug_temp();
    void binary_save();
    void select_var123(vnl_matrix <float > *var1,
                              vnl_matrix <float > *var2,
                              vnl_matrix <float > *var3,
                              unsigned i,unsigned j, unsigned m);

    vnl_vector <float> pair_sum(float,  unsigned, unsigned);
    vnl_vector <float> tripplet_sum(float, float, float,unsigned, unsigned, unsigned);
    vnl_vector <float> tripplet_sum_fast(float, float, float,unsigned, unsigned, unsigned);
    vnl_vector <float> IR_on_line(vcl_vector <vnl_matrix <float> >,float);
    vnl_vector <float> IR_on_line_ptr(vcl_vector <vnl_matrix <float> *>,float);
        vnl_vector <float> IR_on_line_ptr_2view(vcl_vector <vnl_matrix <float> *>,float);
        vnl_vector <float> IR_on_line_ptr_fast(vcl_vector <vnl_matrix <float> *>,float);
    void IR();
    void covariance(vnl_matrix <float> pts1,
                           vnl_matrix <float> pts2,
                           vnl_matrix <float> pts3,
                           unsigned,unsigned,unsigned,bool,bool);

         void covariance_angle(vnl_matrix <float>& pts1,
                           vnl_matrix <float>& pts2,
                           vnl_matrix <float>& pts3,
                           unsigned,unsigned,unsigned,bool,bool,bool,int);

    void write_vrml_points(vnl_matrix <float>,vnl_matrix <float>,
       vnl_matrix <float>, int, vcl_string);
                void write_vrml_points(float,float,
       float, int, vcl_string, float ,float, float, float);
                void write_vrml_probes(vnl_vector <unsigned> ,int , vcl_string);


    vcl_vector <float> distribution_distance(vnl_matrix <float>);
    void select_probe_tripplet();
    void select_probe_tripplet_2view();
                void draw_probe_tripplet();
    void select_tripplet_manual();
    void select_probe_tripplet_p_e();
    vnl_matrix <float> pe_sum(vnl_matrix <float> &) ;

    void find_tripplet_edges(vnl_matrix <float> &,vnl_matrix <float> &,
                    vnl_matrix <float> &,int,int,int);
    bool already_in_list(int,int,int,vcl_vector <tripplet_selection_list>);
         
               
                void read_bb_and_probe(float,double &cube_x, double &cube_y, double &cube_z,
                        vgl_point_3d<double> &X,int,int);
                
                void read_bb_and_probe(float,double &cube_x, double &cube_y, double &cube_z,
                        vgl_point_3d<double> &X,int,int,bool );
    void select_probe_tripplet_angle();
                void select_probe_tripplet_angle4();
    void select_probe_pair();
    float mutual_information(vnl_matrix <float>,
                    vnl_matrix <float>,vnl_matrix <float>,float);
          float mutual_information_for_2(vnl_matrix <float>,
                     vnl_matrix <float>,vnl_matrix <float>,float,int);
              void mi_coupling();
    void read_mean_variance();
    void read_distance();
    void read_distance_one_shot();void read_distance_one_shot_2view();
    void read_detected_points();
                    void read_detected_points_vorl_out();
     void read_detected_points(vcl_string);


    void variance(vcl_vector <float>,double *var, double *mean);
    double measure();
    void model_probe_dependency_test();
    void model_probe_dependency_test_2();
      void model_probe_dependency_test_2_2view();
    void model_probe_dependency_test_3();
     void model_probe_dependency_test_4();
    void vd_back_ground();
   //: method for file menu callback
 void save_status();
  void load_status();

  // methods for debug menu callbacks
  //void show_epipole();
  void load_image();
  void load_image_file(vcl_string, bool, unsigned , unsigned);
 void load_image_file(vcl_string, bool, unsigned );

  rc3d_windows_frame();
  ~rc3d_windows_frame();
  static rc3d_windows_frame *instance();
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  // methods for run menu callbacks
  void creat_line();
  void quit();
 // void add_curve2d(vcl_vector<vgl_point_2d<double> > &pts);
 // void remove_curve2d();
  void remove_debug_info();

  void add_curve3d(vcl_vector<vgl_point_3d<double> > &pts);
  void remove_curve3d();

  void add_next_observes(vcl_vector<vgl_point_2d<double> > &pts);
  
  void init();
  void vd_edges();
  void vd_subtract();
  void fit_line();
  void vd_edges_map();
  void vd_edges_map_all_in();
  void vd_edges_map_all_in_Vorl();
  bool map_check(double,double,double);

  void auto_vd_edges(unsigned, unsigned);
  void auto_vd_edges();
  void select_curve_corres();
  void select_curve_corres_bb();
  void clear_selected();
  void find_epipole();
  // void show_epipole();
  void show_epipolar_line();
  
  void clear_display();
          void clear_display3();
  void reconstruct3d();
   void manual_reconstruction_number(); 
  vgl_point_3d<double> manual_reconstruction();
  void manual_recon_rec_test(); 
  void manual_recon_tripplet_optimization();
  vcl_vector <vgl_point_3d<double> > manual_reconstruction_on_epiline();
  vgl_point_3d<double> manual_reconstruction_on_epiline_BB_Front();
  void draw_point(vcl_vector <vnl_double_3x4> PL,vgl_point_3d<double> X);
  void draw_point(vcl_vector <vnl_double_3x4> PL,vgl_point_3d<double> X,
     float r,float g,float b, float size);
        void draw_point();
  void mouse_point_selection_3_frame(float *xm,float *ym);
  void mouse_point_selection_N_frame();
        void cad_back_N_frame();

  void write_d3_points();
  
  ///
  void add_best_point();
  void best_pts_delete();
  void write_best_points();
  
  
  
  void probe();
  void recursive_probe(double, double, double , double,
    bgui_vtol2D_tableau_sptr );
  
  
  vnl_matrix<double> probe_prob_smart_refine(vcl_vector<vdgl_edgel_chain_sptr>  ec_inR,
    double , double );
  vnl_matrix <double> recursive_probe_ret(double,double,double, double, double, double , double,
    bgui_vtol2D_tableau_sptr,int );
  vnl_matrix <double> recursive_probe_ret_map(double,double,double, double, double, double , double,
    bgui_vtol2D_tableau_sptr,int );
    vnl_matrix <double> recursive_probe_ret_map(double,double,double, vnl_matrix<double>, double , double,
    bgui_vtol2D_tableau_sptr,int );
  vnl_matrix <double> recursive_probe_ret_map_slope(double,double,double,double,double,double, double, double, double , double,
    bgui_vtol2D_tableau_sptr,int );
  vnl_matrix <double> wheel_detector(double,double,double, double, double,int );
  vnl_matrix <double> well_detector(double,double,double, double, double,int );
  
  
  
  vnl_matrix <double> recursive_probe_ret_map_special(double,double,double, double, double, double , double,
    bgui_vtol2D_tableau_sptr,int, double ,double, double);
  vnl_matrix <double> recursive_probe_ret_simple(double,double,double, double, double,vcl_vector<vdgl_edgel_chain_sptr> ,
    bgui_vtol2D_tableau_sptr );
  
  
  vcl_vector <vnl_matrix<float> > d3_refinement(vcl_vector <vnl_matrix<float> >p_some_list) ;
  
  
  vnl_matrix <double> recursive_probe_ret_refine(double, double, double , double,
    bgui_vtol2D_tableau_sptr );
  vcl_vector<vdgl_edgel_chain_sptr> find_sovs_in_BOX(bgui_vtol2D_tableau_sptr,
    double,double,double);
  void probe_design();
  void generate_probe_shape( double, double );
  void initialize_probe();
  void probe_show();
  void probe_show(unsigned int,float,float);
  void d3_probe_show();
  void d3_probe_show_tripplet();
  
  void draw_cube(float [][2]);
  
  void draw_cube_3(float [][2], int );
  bool c_test(vnl_matrix<double>);
  bool c_test(vnl_matrix<double>,float,float,float);
        
  vnl_matrix<double> probe_prob(double,double,double,double,double);
    vnl_matrix<double> probe_prob_wheel(int,double,double,double,double,double);
        vnl_matrix<double> probe_con_prob(double,double,double,double,double);
  vnl_matrix<double> probe_back_or_fore(double,double,double,double,double,int);
  vnl_matrix<double> probe_prob_well(double,double,double,double,double,int,
           double, double, double);
  
  // 3d probe parameter.
  
  void initialize_camera();
  void initialize_camera_3();
  void initialize_camera_3_matt();
  void initialize_camera_3_matt_Vorl(int frame1,
                                          int frame2,
                                          int frame3,
                                          double Scale,
                                          vcl_string filename);

  void initialize_camera_3_debug_temp();
  void initialize_camera_3_string_scan();
        void initialize_camera_N_string_scan();

  void d3_probe_design();
  void initialize_d3_probe();
  void d3_search();
  void d3_search_trip();
  vcl_vector<vnl_matrix <float> > d3_search_trip(vgl_point_3d<double> point_3d,
     double cube_x,double cube_y, double cube_z);
  vcl_vector<vnl_matrix <float> > d3_search_trip_2(vgl_point_3d<double> point_3d,
     double cube_x_pos,double cube_x_neg,double cube_y_pos,double cube_y_neg, double cube_z_pos,double cube_z_neg);

  vnl_matrix<double> back_project(int i_frame, double d3_x0_, double d3_y0_,double d3_z0_);
  vnl_matrix<double> back_project_direct(int i_frame, double d3_x0_, double d3_y0_,double d3_z0_);
  vnl_matrix<double> back_project_for_d_(int i_frame, double d3_x0_, double d3_y0_,double d3_z0_);
  double vec2theta( double, double, double, double );
  void probe_adjust(vnl_matrix<double>);
  vcl_vector<vnl_matrix <float> > p_sort(vcl_vector <vnl_matrix<float> >);
  vcl_vector <vnl_matrix<float> >  p_reduce(vcl_vector <vnl_matrix<float> >);
  float measure_distance( vnl_matrix<float> , vnl_matrix<float> );
  float measure_distance_3d( vnl_matrix<float> , vnl_matrix<float> );
  void quickSort(int *,int);
  void q_sort(int *, int, int );
  float compare( float *a,  float *b);
  
  //knn
  void read_model();
  void read_models();void read_models_2view();
  void clear_models();
  void d3_show_model();
  void d3_show_selected_model();
  void knn();
  void probability_gauss();
  double probability_gauss(int);
  double probability_gauss_online(int,float);
  void probability_class_gauss();
  void probability_class_gauss_online();
  
  void probability_gauss_scale();
  
  void knn_ftr_sl();
  vcl_vector<vnl_matrix <float> > k_sort(vcl_vector <vnl_matrix<float> >,int);
  void save_knn_model();
  void BB();
  void Auto_BB();
  void Detector();
        void Detector_WC();
        void Detector_WC_infile();
        void Detector_WC_infile_2view();
  void Detector_WC_infile_monte();


  void Detector_in_Box();
  vnl_matrix<double> reverse_mapping(vcl_vector<vnl_matrix <float> >);

  void model_statistics();
  void model_statistics_min_max();
  void model_statistics_svd();
  void model_statistics_distance();
  void feature_extraction(vcl_vector<vnl_matrix<float> >, vcl_vector <feat_ >, vcl_vector <feat_ >);
  
  void save_d3_probe() ;
  void d3_probe_load();
  bool d3_probe_load(vcl_string);
  void d3_probe_load_all(vcl_string);
  void d3_probe_model(); 
  void d3_probe_load_direct(vcl_string);
    void d3_probe_load_direct_mute(vcl_string);
  void d3_probe_load_wheel(int,float);
    void d3_probe_load_front_windshield(int,float,vgl_point_3d<double>,vgl_point_3d<double>);
  // drawing ..
  void d3_show_selected_model_and_recon();
  void ball_plot(float x,float y,float z,
     float r,float g,float b, float R,
     vcl_ofstream out );
  
  // global features..
  void feat_add();
  void feat_delete();
  void front_windshield_constraint();
  void front_windshield_constraint_tube();
  float parallel(vnl_vector_fixed <float,3> f1,
     vnl_vector_fixed <float,3> f2,
     vnl_vector_fixed <float,3> f3,
     vnl_vector_fixed <float,3> f4,
     float ,float,float,float);
  float tube(vnl_vector_fixed <float,3> f1,
     vnl_vector_fixed <float,3> f2,
     vnl_vector_fixed <float,3> f3,
     vnl_vector_fixed <float,3> f4); 
  void feat_load();
  void feat_save();
  
  ////debug
  
  void auto_debug();
  void dots_in_cube();
  void draw_feats_dots();
  
  //: the virtual handle function
  virtual bool handle(const vgui_event&);
  
 protected:

                vcl_vector <bsol_intrinsic_curve_2d_sptr> curve_2dl_;
    //internal utility methods
    
    //: initialize the epipole from the lines modelled road
    void init_epipole();
    
    //: it clean the memory allocated by init. it should be called by quit()
    void clean_up();
    bgui_vtol2D_tableau_sptr get_vtol2D_tableau_at(unsigned col, unsigned row);
    bgui_vtol2D_tableau_sptr get_selected_vtol2D_tableau();
    bgui_picker_tableau_sptr get_picker_tableau_at(unsigned col, unsigned row);
    bgui_picker_tableau_sptr get_selected_picker_tableau();
    
    
    vil1_image get_image_at(unsigned col, unsigned row);
    
 private:


         float W_;float E_;
   vcl_ofstream out_;//(out_filename.c_str());
         //batch mode
         bool BATCH_;
         vcl_string batch_dir_, batch_cam_filename_;
         int batch_auto_cam_;
   int batch_frame_1_;
         int batch_frame_2_;
         int batch_frame_3_;
   int new_parametrization_;
         float batch_shift_x_,batch_shift_y_,batch_shift_z_,batch_Box_Add_;

         int batch_how_many_trial_;
         vcl_vector <int> batch_classification_;
         int batch_real_class_;
         int batch_index_;
         
         bool batch_point_save_;bool batch_images_save_;
         
         float batch_cube_inc_;




   int slope_class_id_;
         int wheel_count_; 
   bool wheel_detector_,slope_detector_,well_detector_on_,front_windshield_;
   bool front_windshield_fine_adjust_;
   vgl_point_3d <double> slope_detector_pivot_;

   bool con_flag_; // occludding contour..
vcl_vector <vgl_polygon<double> > veh_cons_;



   bool class0_on_;
   float margin_;


          bool yellow_cube_;
          float epsilon_small_; //(Non Maximum Suppression)
    double BB_Scale_;
    double BB_Box_Add_;
    bool special_flag_wheel_well_2_;
    bool special_flag_wheel_well_1_;
    //: get track of all the 3d points added into 3d tableau
    vcl_vector<vgui_lineseg3D* > curves_3d_;
    
    //: get predicted curves 2d
    vcl_vector<vgui_soview2D_point* > predicted_curves_2d_;
    
    //: 2d curve for the next frame, which is used for debugging
    vcl_vector<vgui_soview2D_lineseg* > debug_curves_2d_;
    
    //: 2d curve at time t
    vcl_vector<vgui_soview2D_lineseg* > curves_2d_;
    
    //: 2d curve at time 0
    vcl_vector<vgui_soview2D_lineseg* > curves_2d_0_;
    
    //: kalman filter
    kalman_filter* kalman_;
    bgui_picker_tableau_sptr tab_picker_;
    vgui_composite_tableau_sptr tab_cps_;
    
    
    //vcl_vector <vgui_image_tableau_sptr> img_2d_;
    //vcl_vector <vgui_easy2D_tableau_sptr> easy_2d_;
    vcl_vector<bgui_vtol2D_tableau_sptr> vtol_tabs_;
    vcl_vector<bgui_picker_tableau_sptr> tabs_picker_;
    
    vgui_easy3D_tableau_sptr tab_3d_;
    //vgui_rubberband_tableau_sptr tab_rubber_;
    vil1_image img_;
    vgui_grid_tableau_sptr grid_;
    //vcl_string data_file_name_;
    
    //: inital epipole
    vcl_vector<vgl_homg_line_2d<double> > lines_;
    vgl_point_2d<double> *e_;
    vcl_string status_info_;
    static rc3d_windows_frame *instance_;
    
    vnl_double_3 epi_;
    bool epi_flag;
    bool init_3_cam_flag_;
    bool dots_in_cube_flag_;
    bool how_many_dots_flag_;
    int how_many_dots_;
    
    //vcl_vector <vdgl_edgel_chain_sptr>  ecs;
    vdgl_edgel_chain_sptr  ecs[3];
    vdgl_digital_curve_sptr dcs[3];
    
    
    vcl_vector<vnl_double_3x4 > Cam_List_;
    vnl_double_3x4  Cam_List_KLT_[60];
    //Number of image frame..
    unsigned iview_;
    //probe parameters....
    
    double th1_,th2_,th3_;
    double th1_x_,th1_y_,th2_x_,th2_y_,th3_x_,th3_y_;
    double n_x1_,n_x2_,n_x3_,n_y1_,n_y2_,n_y3_;
    double d_,AT_,CT_;
    double R_,R1_,R2_,R3_;
    double s_thresh_1;
    double del_mouse_,inc_mouse_;
    bool udm_;//uniform distance measeure
    bool recur_flag_;
    bool c_test_flag_;
    float c_ratio_;
    
    float threshold_;
    double limiter_threshold_;
    double d_tube_;
    
    //3d probe parameter
    double d3_phi_[4];
    double d3_theta_[4];
    double d3_rho_;
    double d3_R_;
   
   // 8-11 direct 
   double d3_x_[4];
   double d3_y_[4];
   double d3_z_[4];

    //double robe1_,robe2_,robe3_; //robe flag
    double d3_robe_[4];
    double d3_back_fore_left_[4];
    double d3_back_fore_right_[4];

    //3d search parameter
    double cube_x_,cube_y_, cube_z_;
    double cube_inc_,cube_z_multiplier_;
    
    vgui_window* win_;
    
    //3d reconsturction  
    double xc_,yc_,zc_;
    bool centered_flag_;
    vnl_double_3x3 M_in_;
    double trans_dist;
    vnl_double_3 T, T_intpl;
    vnl_double_3x4 E1, E2, E3, E_intpl;
    
    // camera parameters..
    vcl_vector <vnl_double_3x4> PL_;

    // 5-6-04 Model Scale,Rotation,Translation parameters
    vnl_matrix <double> BB_Rot_;
    vgl_point_3d<double> BB_front_;
    vcl_vector < vgl_point_3d<double> > BB_bottom_;
     vgl_point_3d<double> BB_shift_;
    vnl_matrix <double> BB_RT_matt_;
    vgl_point_3d<double> BB_Front_shift_;
    //knn
    vcl_vector <vnl_matrix<float> > model_list_;
    vcl_vector <vnl_matrix<float> > feature_list_;
//    vnl_matrix<float>  feature_list_(15,3);
    vcl_vector <vnl_vector<float> > model_distance_list_;
    vcl_vector <vnl_matrix<float> > var_list_;
    vcl_vector <vcl_vector <vnl_matrix<float> > > p_list_;

    //selected 3d Points
    vcl_vector <vnl_vector<float> > d3_pts_list_;
    float d3_one_pt_[3];
    
    //global geomtric constraint
    vcl_vector <vcl_vector<vnl_matrix <float> > > geo_feat_;
    vcl_vector<vnl_matrix <float> > temp_geo_feat_;
    
    //vcl_vector <vil1_memory_image_of<float>> maps_; 
    vil1_memory_image_of <float> maps_[3]; //later vector
    vil1_memory_image_of <float> maps_search_[3];
    vil1_memory_image_of <float> fore_ground_[3];
    vil1_memory_image_of <float> back_ground_; 
    vil1_memory_image_of<vil1_rgb<unsigned char> > fore_ground_rgb_[3];  
    
    vcl_vector <vcl_string> fore_name;

    //
    

    vcl_vector <struct tripplet_cov_set> trio_;
    struct tripplet_cov_set trios_[14][14][14];
    vcl_vector <struct p_list_set> p_list_set_;

                //11-7-2005 recognizer combination
                vcl_vector <struct p_list_set> p_list_set_1_;
                vcl_vector <struct p_list_set> p_list_set_2_;


    struct p_list_set p_list_set_cs_[3][10];
    //vcl_vector <struct p_list_set> p_list_set_0_;
    //vcl_vector <struct p_list_set> p_list_set_1_;
    //vcl_vector <struct p_list_set> p_list_set_2_;
   vcl_vector < vcl_vector <struct p_list_set> > ppll_;
   // backgorund test
   bool back_ground_test_flag_;
   vnl_matrix <double> back_ground_test_;

   vnl_matrix <double> Probes_;
   int N_P_;
   int Current_P_;


   //turn off probes
   int toff0_,toff1_,toff2_,toff3_,toff4_,toff5_,
       toff6_,toff7_,toff8_,toff9_;
   
};


class detector{
};

//later //template<class T>
/*
class probe{
public:
probe();
probe(double th1,double th2,double th3);
probe(vcl_vector<double>);
~probe();
void set_angle(double,double,double); 

  double th1_,th2_,th3_;
  double d_,AT_,CT_;
  double R_;
  double s_thresh_1;
  double del_mouse_,inc_mouse_;
  bool udm_;//uniform distance measeure
  bool recur_flag_;
  float c_ratio_;
  
   private:
   double th1_x_,th1_y_,th2_x_,th2_y_,th3_x_,th3_y_;
   double n_x1_,n_x2_,n_x3_,n_y1_,n_y2_,n_y3_;
   };

void probe::set_angle(double th1,double th2, double th3) {
   th1_=th1;
   th2_=th2;
   th3_=th3;

   th1_x_=sin(th1_*vpi/180.0);th1_y_=cos(th1_*vpi/180.0);
   th2_x_=sin(th2_*vpi/180.0);th2_y_=cos(th2_*vpi/180.0);
   th3_x_=sin(th3_*vpi/180.0);th3_y_=cos(th3_*vpi/180.0);
   n_x1_=-th1_y_; n_y1_=th1_x_;
   n_x2_=-th2_y_; n_y2_=th2_x_;
   n_x3_=-th3_y_; n_y3_=th3_x_;
}
probe::probe(double th1,double th2, double th3) {
    th1_=th1;
   th2_=th2;
   th3_=th3;

   th1_x_=sin(th1_*vpi/180.0);th1_y_=cos(th1_*vpi/180.0);
   th2_x_=sin(th2_*vpi/180.0);th2_y_=cos(th2_*vpi/180.0);
   th3_x_=sin(th3_*vpi/180.0);th3_y_=cos(th3_*vpi/180.0);
   n_x1_=-th1_y_; n_y1_=th1_x_;
   n_x2_=-th2_y_; n_y2_=th2_x_;
   n_x3_=-th3_y_; n_y3_=th3_x_;
*/
#endif // rc3d_windows_frame_h_


