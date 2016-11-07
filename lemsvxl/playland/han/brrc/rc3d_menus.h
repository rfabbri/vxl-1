#ifndef rc3d_menus_h_
#define rc3d_menus_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief This file is adopted from Joe's work.
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//  han
//
//-----------------------------------------------------------------------------

#include <vvid/vvid_file_manager.h>
#include <vvid/bin/vidfpl_menus.h>


#include <vgui/vgui_menu.h>
class rc3d_menus
{
public:
//  static void test();
  static void klth();
    static void klth_epi();
   static void load_status();
   static void save_status();
   static void create_line();
   //static void show_epipole();
   static void load_image();
   // static void show_back_projection();
   //static void remove_debug_info();
   //static void show_next_observes();
   //static void show_predicted_curves();
   static void init_kalman_callback();
   //static void kalman_go_callback();
   static void quit_callback();
   static void vs();
   static void vs_subtraction();
   static void auto_vs();
   //static void load_video_callback();
   static void osl_line_fit();
   static void vd_edges_map();
   static void vd_edges_map_3();
   static void back_ground();
   
   static void probe();
   static void probe_design();
   static void probe_show();
   static void d3_probe_design();
   static void d3_search();
   static void d3_search_trip();
   static void d3_probe_show();
   static void d3_probe_show_tripplet();
   
   static void d3_probe_read();
   static void d3_probe_write();
   
   static void find_epipole();
   static void show_epi_line();
   static void cs();
   static void recon3d();
   static void manual_recon();
   static void manual_reconstruction_number();
         static void mouse_N();
         static void cad_N();
   static void manual_recon_recog_test();
   static void manual_recon_tri_opt();
   static void manual_recon_on_epi();
   static void manual_recon_on_epi_debug_temp();
   static void manual_recon_on_epi_BB_Front();
   static void remove_curve3d();
   static void clear_display();
         static void clear_display3();
   static void select();
   static void select_bb();
   
   static void read_model();
   static void model_statistics();
   static void model_statistics_min_max();
   static void model_statistics_distance();
   static void model_probe_dependency_test();
   static void model_probe_dependency_test_2();
   static void model_probe_dependency_test_3();
   static void read_mean_variance();
   static void select_probe_pair();
    static void read_distance();
   static void read_distance_one_shot();
   static void select_probe_tripplet();
   static void select_probe_tripplet_2view();
    static void select_probe_tripplet_angle();
        static void select_probe_tripplet_angle4();
         static void draw_probe_tripplet();
   static void select_probe_tripplet_p_e();
   static void select_manual();
   static void IR();
   static void IR_mxl();
   static void IR_mxl_monte();
    static void IR_mxl_monte_carlo();
   static void IR_mxl_monte_sum();
   static void read_detected_points();
         static void read_detected_points_vorl_out();


   static void read_models();
   static void clear_models();
   static void show_model();
   static void show_selected_model();
   static void show_model_and_recon();
   
   
   static void save_knn_model();
   static void knn();
   static void prob_gauss();
   static void prob_class_gauss();
   static void prob_class_gauss_online();
   static void prob_gauss_scale();
   
   static void write_points();
   static void BB();
   static void write_best_points();
   static void best_pts_delete();
   static void add_best_point();
   
   // feature geometry
   static void feat_add();
   static void feat_delete();
   static void front_windshield_constraint();
   static void front_windshield_constraint_tube();
   static void feat_load();
   static void feat_save();
   
   //debug
   
   static void auto_debug();
   static void init_3_camera();
   static void init_3_camera_matt();
   static void init_3_camera_string_scan();
         static void init_N_camera_string_scan();
   static void init_3_camera_debug_temp();
   static void dots_in_cube();
   static void draw_feats_dots();
         static void read_iv();
         static void read_vrml();
         static void test();
   
   // automatic recognition
   static void Auto_BB();
   static void Detector();
         static void Detector_WC();
static void Detector_WC_infile();
static void Detector_WC_infile_2view();
static void Detector_WC_infile_monte();
   static void Detector_in_Box();//Full Auto
   static void Auto_BB_1();
   static void monte();
   static void Front_Plane();
   static void shift_and_rotate();


         //batch
         static void batch();
         static void batch_front_windshield();

   static vgui_menu get_menu();
   

   // 2view
   static void model_probe_dependency_test_2_2view();
   static void read_models_2view();
   static void read_distance_one_shot_2view();
   static void IR_mxl_monte_carlo_2view();
   
private:
   rc3d_menus() {}
};

#endif // rc3d_menus_h_

