#include "rc3d_menus.h"

#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include "rc3d_windows_frame.h"
//#include <bmvl/bmvv/bmvv_multiview_manager.h>
//Static munu callback functions

void rc3d_menus::quit_callback()
{
  rc3d_windows_frame::instance()->quit();
}

//void rc3d_menus::init_kalman_callback()
//{
//  rc3d_windows_frame::instance()->init_kalman();
//}

//bmvv_menus definition
vgui_menu rc3d_menus::get_menu()
{
   vgui_menu menu_bar;
   vgui_menu file_menu;
   vgui_menu probe_menu;
   vgui_menu debug_menu;
   vgui_menu manual_menu;
   vgui_menu epipole_menu;
   vgui_menu vd_menu;
   vgui_menu knn_menu;
   vgui_menu pts_menu;
   vgui_menu probe_IO;
   vgui_menu feat_menu;
   vgui_menu auto_menu;
   vgui_menu automatic_menu;
   vgui_menu klt_menu;
         vgui_menu two_view_menu;
   vgui_menu batch_menu;
   

 //  vgui_menu debug_menu;
   
   epipole_menu.add("save epipole", save_status, (vgui_key)'s', vgui_CTRL);
   epipole_menu.add("load epipole", load_status, (vgui_key)'o', vgui_CTRL);
   epipole_menu.add("show ep line", show_epi_line);
   epipole_menu.add("epipole business",find_epipole);
   
   file_menu.add("load image", load_image, (vgui_key)'l', vgui_CTRL);
   file_menu.add("clear image",clear_display);
            file_menu.add("clear ALL 3 image",clear_display3);
   file_menu.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
   
   //action_menu.add("create line", create_line, (vgui_key)'n', vgui_CTRL);
   //action_menu.add("init", init_kalman_callback, (vgui_key)'i', vgui_CTRL);
   //action_menu.add("go", kalman_go_callback, (vgui_key)'g', vgui_CTRL);
   probe_menu.add("**probe",probe,(vgui_key)'p');
   probe_menu.add("probe_design",probe_design);
   probe_menu.add("probe_show",probe_show);
   //   probe_menu.message("3D probe");
   probe_menu.separator();
   probe_menu.add("3d probe_design",d3_probe_design);
   probe_menu.add("3d probe_show",d3_probe_show);
   probe_menu.add("3d probe_show_tripplet",d3_probe_show_tripplet);
   probe_menu.separator();
   probe_menu.add("** 3d serach",d3_search);
   probe_menu.add("*** 3d serach (triple)",d3_search_trip);
   
   vd_menu.add("VD Edges", vs);
   vd_menu.add("auto VD Edges", auto_vs);
   vd_menu.add("VD subtraction Edges" ,vs_subtraction);
   vd_menu.add("osl line fit", osl_line_fit);
   vd_menu.add("vd + map", vd_edges_map);
   vd_menu.add("*vd + map all 3", vd_edges_map_3);
   vd_menu.add("*back ground", back_ground);
   
   manual_menu.add("select",select);
   manual_menu.add("select_for_bounding_box",select_bb);
   manual_menu.add("clear",cs);
   manual_menu.add("clear 3d",remove_curve3d);
   manual_menu.separator();
   manual_menu.add("3d reconstruction",recon3d);   
   manual_menu.add("3d point recon",manual_recon);
   manual_menu.add("recon insert #",manual_reconstruction_number);

         manual_menu.add("** mouse N",mouse_N);
         manual_menu.add("** cad N",cad_N);

   manual_menu.add("*3d point recon recog test",manual_recon_recog_test);
   manual_menu.add("** manual recon tripplet optimaiztion",manual_recon_tri_opt);
   
   manual_menu.add("3d point recon_on_epi",manual_recon_on_epi);
   
   manual_menu.add("3d point recon_on_epi_debug_temp()",manual_recon_on_epi_debug_temp);
   manual_menu.add("3d point recon_on_epi_BB_front",manual_recon_on_epi_BB_Front);
   
   knn_menu.add("read individual model",read_model);
   knn_menu.add("read multiple models",read_models);
   knn_menu.add("save knn model",save_knn_model);
   knn_menu.add("clear_models",clear_models);
   knn_menu.separator();
   knn_menu.add("show model(->writing vrml file)",show_model);
   knn_menu.add("show selected model(->writing vrml file)",show_selected_model);
   knn_menu.add("model statistics",model_statistics);
   knn_menu.add("min max of probe",model_statistics_min_max);
   knn_menu.add("model statistics(distance)",model_statistics_distance);
   knn_menu.add("show_model_and_recon()",show_model_and_recon);
   
   
   knn_menu.separator();
   knn_menu.add("knn",knn);
   knn_menu.add("probability gauss",prob_gauss,(vgui_key)'g');
   knn_menu.add("Class conditional Probaility",prob_class_gauss,(vgui_key)'x');
   knn_menu.add("probability gauss_scale",prob_gauss_scale);
   knn_menu.add("Class conditional Probaility Online",prob_class_gauss_online);
   
   knn_menu.separator();
   knn_menu.add("model probe dependency", model_probe_dependency_test);
   knn_menu.add("model probe dependency 2 (cooper)", model_probe_dependency_test_2);
   knn_menu.add("model probe dependency  3(2D)", model_probe_dependency_test_3);

    knn_menu.add("read mean and variance",read_mean_variance);
   knn_menu.add("select probe pair",select_probe_pair);
   knn_menu.separator();
   knn_menu.add("read distance",read_distance);
   knn_menu.add("*read distance_one_shot",read_distance_one_shot);
   knn_menu.add("*select probe tripplet",select_probe_tripplet);
   knn_menu.add("*select probe tripplet_angle",select_probe_tripplet_angle);
         knn_menu.add("*select probe tripplet_angle(4 probe)",select_probe_tripplet_angle4);
         knn_menu.add("*draw probe tripplet",draw_probe_tripplet);
   knn_menu.add("*select probe tripplet manual",select_manual);
    knn_menu.add("*select probe tripplet p of error",select_probe_tripplet_p_e);
   knn_menu.separator();
   knn_menu.add("invariant recognizer",IR);
   knn_menu.add("*invariant recognizer_mxl",IR_mxl);
    knn_menu.add("*invariant recognizer_mxl_monte",IR_mxl_monte);
   knn_menu.add("*invariant recognizer_mxl_monte_carlo",IR_mxl_monte_carlo);
    knn_menu.add("*invariant recognizer_mxl_monte_sum",IR_mxl_monte_sum);
   knn_menu.add("*read_detected points(probes)",read_detected_points);
         knn_menu.add("*read_detected points vorl out(probes)",read_detected_points_vorl_out);



   pts_menu.add("3d point reconstruction",manual_recon);
   pts_menu.add("write points",write_points);
   pts_menu.add("BB",BB);
   pts_menu.separator();
   pts_menu.add("write best points",write_best_points);
   pts_menu.add("add_best_point",add_best_point);
   pts_menu.add("best_pts_delete()",best_pts_delete); 
   
   probe_IO.add("probe_read",d3_probe_read);
   probe_IO.add("probe_write",d3_probe_write);
   
   feat_menu.add("feature add",feat_add);
   feat_menu.add("feature load",feat_load);
   feat_menu.add("feature save",feat_save);
   feat_menu.add("feature delete all",feat_delete);
   feat_menu.separator();
   feat_menu.add("geometry constraint", front_windshield_constraint);
   feat_menu.add("geometry constraint tube", front_windshield_constraint_tube);
   
   auto_menu.add("auto debug", auto_debug);
   auto_menu.add("init_3_camera", init_3_camera);
   auto_menu.add("*init_3_camera_matt", init_3_camera_matt);
   auto_menu.add("**init_3_camera_string_scan", init_3_camera_string_scan);
            auto_menu.add("**init_N_camera_string_scan", init_N_camera_string_scan);
   auto_menu.add("init_3_camera debug temp", init_3_camera_debug_temp);
   auto_menu.add("dots in cube", dots_in_cube);
   auto_menu.add("draw loaded feature dots",draw_feats_dots);
   
   automatic_menu.add("Auto_BB",Auto_BB);
   automatic_menu.add("*Detector",Detector);
         automatic_menu.add("*Detector_WC",Detector_WC);
         automatic_menu.add("*Detector_WC_infile",Detector_WC_infile);
   automatic_menu.add("*Detector_WC_infile_monte",Detector_WC_infile_monte);
   automatic_menu.add("*Auto_BB_in_Box(Full Auto)",Detector_in_Box);
   automatic_menu.add("*Auto_BB_1",Auto_BB_1);
    automatic_menu.add("*monte",monte);
   automatic_menu.separator();
   automatic_menu.add("Front_Plane",Front_Plane);
   automatic_menu.add("shift_and_rotate",shift_and_rotate);
         
   debug_menu.add("read_iv",read_iv);
         debug_menu.add("read_vrml",read_vrml);
         debug_menu.add("test routine",test);

   klt_menu.add("klt",klth);
   klt_menu.add("klt_epi",klth_epi);
      
   two_view_menu.add("read models 2 view",read_models_2view);

   two_view_menu.add("read distance one shot 2 view",read_distance_one_shot_2view);
   two_view_menu.add("model dependency 2 view ",model_probe_dependency_test_2_2view);
   two_view_menu.add("select triplet 2 view",select_probe_tripplet_2view);
   two_view_menu.add("IR monte carlo 2 view",IR_mxl_monte_carlo_2view);
   two_view_menu.add("Detector_WC_infile_2view",Detector_WC_infile_2view);

         batch_menu.add("batch",batch);
         batch_menu.add("batch(front windshield)",batch_front_windshield);

         menu_bar.add("file", file_menu);
         menu_bar.add("probe", probe_menu);
         menu_bar.add("Epipole", epipole_menu);
   menu_bar.add("VD",vd_menu);
   menu_bar.add("Manual",manual_menu);
   menu_bar.add("*KNN", knn_menu);
   menu_bar.add("Points",pts_menu);
   menu_bar.add("probe IO",probe_IO);
   menu_bar.add("Features",feat_menu);
   menu_bar.add("*auto debug",auto_menu);
   menu_bar.add("*Automatic", automatic_menu);
   menu_bar.add("Debug",debug_menu);
   menu_bar.add("klth",klt_menu);
   menu_bar.add("2 view",two_view_menu);
   menu_bar.add("BATCH",batch_menu);
         return menu_bar;
}

void rc3d_menus::klth()
{
  rc3d_windows_frame::instance()->klth();
}

void rc3d_menus::klth_epi()
{
  rc3d_windows_frame::instance()->klth_epi();
}

void rc3d_menus::Auto_BB_1()
{
   rc3d_windows_frame::instance()->Auto_BB_1();
}
void rc3d_menus::monte()
{
   rc3d_windows_frame::instance()->monte();
}

void rc3d_menus::Auto_BB()
{
   rc3d_windows_frame::instance()->Auto_BB();
}
void rc3d_menus::Detector()
{
   rc3d_windows_frame::instance()->Detector();
}

void rc3d_menus::Detector_WC()
{
   rc3d_windows_frame::instance()->Detector_WC();
}
void rc3d_menus::Detector_WC_infile()
{
   rc3d_windows_frame::instance()->Detector_WC_infile();
}

// 3 individual baby box
void rc3d_menus::Detector_WC_infile_monte()
{
   rc3d_windows_frame::instance()->Detector_WC_infile_monte();
}

void rc3d_menus::Detector_in_Box()
{
   rc3d_windows_frame::instance()->Detector_in_Box();
}
void rc3d_menus::Front_Plane() 
{
    rc3d_windows_frame::instance()->Front_Plane();
}
void rc3d_menus::shift_and_rotate() 
{
    rc3d_windows_frame::instance()->rotate_and_shift();
}


void rc3d_menus::d3_probe_read()
{
   rc3d_windows_frame::instance()->d3_probe_load();
}

void rc3d_menus::d3_probe_write()
{
   rc3d_windows_frame::instance()->save_d3_probe();
}

void rc3d_menus::read_model()
{
   rc3d_windows_frame::instance()->read_model();
}

void rc3d_menus::model_statistics()
{
   rc3d_windows_frame::instance()->model_statistics();
}


void rc3d_menus::model_statistics_min_max()
{
   rc3d_windows_frame::instance()->model_statistics_min_max();
}

void rc3d_menus::model_statistics_distance()
{
   rc3d_windows_frame::instance()->model_statistics_distance();
}

// 5-24-04
void rc3d_menus::model_probe_dependency_test()
{
   rc3d_windows_frame::instance()->model_probe_dependency_test();
}
//

// 5-26-04 cooper's idea
void rc3d_menus::model_probe_dependency_test_2()
{
   rc3d_windows_frame::instance()->model_probe_dependency_test_2();
}
//
void rc3d_menus::model_probe_dependency_test_3()
{
   rc3d_windows_frame::instance()->model_probe_dependency_test_3();
}

//5-27-04 
void rc3d_menus::select_probe_pair(){
   rc3d_windows_frame::instance()->select_probe_pair();
}

void rc3d_menus::select_probe_tripplet(){
   rc3d_windows_frame::instance()->select_probe_tripplet();
}
void rc3d_menus::select_probe_tripplet_angle(){
   rc3d_windows_frame::instance()->select_probe_tripplet_angle();
}
void rc3d_menus::select_probe_tripplet_angle4(){
   rc3d_windows_frame::instance()->select_probe_tripplet_angle4();
}
void rc3d_menus::draw_probe_tripplet(){
   rc3d_windows_frame::instance()->draw_probe_tripplet();
}
void rc3d_menus::select_manual(){
   rc3d_windows_frame::instance()->select_tripplet_manual();
}
void rc3d_menus::select_probe_tripplet_p_e(){
   rc3d_windows_frame::instance()->select_probe_tripplet_p_e();
}
void rc3d_menus::read_mean_variance(){
   rc3d_windows_frame::instance()->read_mean_variance();
}
void rc3d_menus::read_distance(){
   rc3d_windows_frame::instance()->read_distance();
}
//7-7-2004
void rc3d_menus::read_distance_one_shot(){
   rc3d_windows_frame::instance()->read_distance_one_shot();
}
//6-7 -04 
void rc3d_menus::IR(){
   rc3d_windows_frame::instance()->IR();
}
//6-17-04
void rc3d_menus::IR_mxl(){
   rc3d_windows_frame::instance()->IR_mxl();
}

void rc3d_menus::IR_mxl_monte(){
   rc3d_windows_frame::instance()->IR_mxl_monte();
}
void rc3d_menus::IR_mxl_monte_carlo(){
   rc3d_windows_frame::instance()->IR_mxl_monte_carlo();
}
void rc3d_menus::IR_mxl_monte_sum(){
   rc3d_windows_frame::instance()->IR_mxl_monte_sum();
}

void rc3d_menus::read_detected_points(){
   rc3d_windows_frame::instance()->read_detected_points();
}
void rc3d_menus::read_detected_points_vorl_out(){
   rc3d_windows_frame::instance()->read_detected_points_vorl_out();
}
void rc3d_menus::read_models()
{
   rc3d_windows_frame::instance()->read_models();
}
void rc3d_menus::clear_models()
{
   rc3d_windows_frame::instance()->clear_models();
}
void rc3d_menus::save_knn_model()
{
   rc3d_windows_frame::instance()->save_knn_model();
}
void rc3d_menus::show_model()
{
   rc3d_windows_frame::instance()->d3_show_model();
}
void rc3d_menus::show_selected_model()
{
   rc3d_windows_frame::instance()->d3_show_selected_model();
}
void rc3d_menus::show_model_and_recon()
{
rc3d_windows_frame::instance()->d3_show_selected_model_and_recon();
}



void rc3d_menus::knn()
{
   rc3d_windows_frame::instance()->knn();
}

void rc3d_menus::prob_gauss()
{
   rc3d_windows_frame::instance()->probability_gauss();
}
void rc3d_menus::prob_class_gauss()
{
   rc3d_windows_frame::instance()->probability_class_gauss();
}

void rc3d_menus::prob_class_gauss_online()
{
   rc3d_windows_frame::instance()->probability_class_gauss_online();
}

void rc3d_menus::prob_gauss_scale()
{
   rc3d_windows_frame::instance()->probability_gauss_scale();
}

void rc3d_menus::recon3d()
{
   vcl_cerr<<"goes 3d reconstruction\n";
   rc3d_windows_frame::instance()->reconstruct3d();
}
void rc3d_menus::manual_recon()
{
   //vcl_cerr<<"goes  reconstruction\n";
   rc3d_windows_frame::instance()->manual_reconstruction();
}
void rc3d_menus::manual_reconstruction_number()
{
   //vcl_cerr<<"goes  reconstruction\n";
   rc3d_windows_frame::instance()->manual_reconstruction_number();
}

void rc3d_menus::cad_N()
{
   //vcl_cerr<<"goes  reconstruction\n";
   rc3d_windows_frame::instance()->cad_back_N_frame();
}

void rc3d_menus::mouse_N()
{
   //vcl_cerr<<"goes  reconstruction\n";
   rc3d_windows_frame::instance()->mouse_point_selection_N_frame();
}

void rc3d_menus::manual_recon_recog_test()
{
   //vcl_cerr<<"goes  reconstruction\n";
   rc3d_windows_frame::instance()->manual_recon_rec_test();
}


void rc3d_menus::manual_recon_tri_opt()
{
   //vcl_cerr<<"goes  reconstruction\n";
   rc3d_windows_frame::instance()->manual_recon_tripplet_optimization();
}

void rc3d_menus::manual_recon_on_epi()
{
   //vcl_cerr<<"goes  reconstruction\n";
   rc3d_windows_frame::instance()->manual_reconstruction_on_epiline();
}

void rc3d_menus::manual_recon_on_epi_debug_temp()
{
   //vcl_cerr<<"goes  reconstruction\n";
   rc3d_windows_frame::instance()->manual_reconstruction_on_epiline_debug_temp();
}
void rc3d_menus::manual_recon_on_epi_BB_Front()
{
   //vcl_cerr<<"goes  reconstruction\n";
   rc3d_windows_frame::instance()->manual_reconstruction_on_epiline_BB_Front();
}

void rc3d_menus::write_points()
{
   rc3d_windows_frame::instance()->write_d3_points();
}
void rc3d_menus::BB()
{
   rc3d_windows_frame::instance()->BB();
}
void rc3d_menus::write_best_points()
{
   rc3d_windows_frame::instance()->write_best_points();
}

void rc3d_menus::best_pts_delete()
{
   rc3d_windows_frame::instance()->best_pts_delete();
}

void rc3d_menus::add_best_point()
{
   rc3d_windows_frame::instance()->add_best_point();
}


//////


void rc3d_menus::remove_curve3d()
{
   rc3d_windows_frame::instance()->remove_curve3d();
}
void rc3d_menus::find_epipole()
{
   vcl_cerr<<"goes epipole extraction using 5 point\n"<<vcl_endl;
   rc3d_windows_frame::instance()->find_epipole();
}
void rc3d_menus::show_epi_line()
{
   //vcl_cerr<<"goes epipole extraction using 5 point\n"<<vcl_endl;
   rc3d_windows_frame::instance()->show_epipolar_line();
}

void rc3d_menus::cs()
{
   rc3d_windows_frame::instance()->clear_selected();
}
void rc3d_menus::clear_display()
{
   rc3d_windows_frame::instance()->clear_display();
}
void rc3d_menus::clear_display3()
{
   rc3d_windows_frame::instance()->clear_display3();
}

void rc3d_menus::vs()
{
//  vcl_cerr<<"goes vd callback\n";
 rc3d_windows_frame::instance()->vd_edges();
}

void rc3d_menus::vs_subtraction()
{
   rc3d_windows_frame::instance()->vd_subtract();
}
/*void rc3d_menus::load_video_callback()
{
#ifdef HAS_MPEG2
   //need to define callbacks
   vidl_io::load_mpegcodec_callback = &vidfpl_load_mpegcodec_callback;
#endif
   vvid_file_manager::instance()->load_video_file();
}*/



void rc3d_menus::auto_vs()
{
 // vcl_cerr<<"goes vd callback\n";
 rc3d_windows_frame::instance()->auto_vd_edges();
}
      
void rc3d_menus::osl_line_fit()
{
   rc3d_windows_frame::instance()->fit_line();
}
void rc3d_menus::vd_edges_map()
{
 // vcl_cerr<<"goes vd callback\n";
 rc3d_windows_frame::instance()->vd_edges_map();
}
void rc3d_menus::vd_edges_map_3()
{
 // vcl_cerr<<"goes vd callback\n";
 rc3d_windows_frame::instance()->vd_edges_map_all_in();
}
void rc3d_menus::back_ground()
{
 // vcl_cerr<<"goes vd callback\n";
 rc3d_windows_frame::instance()->back_ground();
}
/*void rc3d_menus::kalman_go_callback()
{
  vcl_cerr<<"goes into kalman_go_callback\n";
  rc3d_windows_frame::instance()->go();
}

void rc3d_menus::show_predicted_curves()
{
  vcl_cerr<<"goes into predicted_curves\n";
  rc3d_windows_frame::instance()->show_predicted_curve();
}

void rc3d_menus::show_next_observes()
{
  rc3d_windows_frame::instance()->show_next_observes();
}
*/
/*void rc3d_menus::remove_debug_info()
{
  rc3d_windows_frame::instance()->remove_debug_info();
}
*/
/*void rc3d_menus::show_back_projection()
{
  rc3d_windows_frame::instance()->show_back_projection();
}
*/
void rc3d_menus::load_image()
{
  rc3d_windows_frame::instance()->load_image();
}

/*void rc3d_menus::show_epipole()
{
  rc3d_windows_frame::instance()->show_epipole();
}
*/
void rc3d_menus::create_line()
{
  rc3d_windows_frame::instance()->creat_line();
}

void rc3d_menus::load_status()
{
  rc3d_windows_frame::instance()->load_status();
}

void rc3d_menus::save_status()
{
  rc3d_windows_frame::instance()->save_status();
}

void rc3d_menus::select()
{
  rc3d_windows_frame::instance()->select_curve_corres();
}
void rc3d_menus::select_bb()
{
  rc3d_windows_frame::instance()->select_curve_corres_bb();
}
void rc3d_menus::probe()
{
   rc3d_windows_frame::instance()->probe();
}
void rc3d_menus::probe_design()
{
   rc3d_windows_frame::instance()->probe_design();
}
void rc3d_menus::probe_show()
{
   rc3d_windows_frame::instance()->probe_show();
}
void rc3d_menus::d3_probe_show()
{
   rc3d_windows_frame::instance()->d3_probe_show();
}
void rc3d_menus::d3_probe_show_tripplet()
{
   rc3d_windows_frame::instance()->d3_probe_show_tripplet();
}

void rc3d_menus::d3_probe_design()
{
   rc3d_windows_frame::instance()->d3_probe_design();
}
void rc3d_menus::d3_search()
{
   rc3d_windows_frame::instance()->d3_search();
}
void rc3d_menus::d3_search_trip()
{
   rc3d_windows_frame::instance()->d3_search_trip();
}


/// feature geometry constraint..
void rc3d_menus::feat_add()
{
   rc3d_windows_frame::instance()->feat_add();
}

void rc3d_menus::feat_delete()
{
   rc3d_windows_frame::instance()->feat_delete();
}


void rc3d_menus::front_windshield_constraint()
{
   rc3d_windows_frame::instance()->front_windshield_constraint();
}
void rc3d_menus::front_windshield_constraint_tube()
{
   rc3d_windows_frame::instance()->front_windshield_constraint_tube();
}
void rc3d_menus::feat_load()
{
   rc3d_windows_frame::instance()->feat_load();
}

void rc3d_menus::feat_save()
{
   rc3d_windows_frame::instance()->feat_save();
}


void rc3d_menus::auto_debug()
{
   rc3d_windows_frame::instance()->auto_debug();
}

void rc3d_menus::init_3_camera()
{
   rc3d_windows_frame::instance()->initialize_camera_3();
}
void rc3d_menus::init_3_camera_matt()
{
   rc3d_windows_frame::instance()->initialize_camera_3_matt();
}

void rc3d_menus::init_3_camera_string_scan()
{
   rc3d_windows_frame::instance()->initialize_camera_3_string_scan();
}

void rc3d_menus::init_N_camera_string_scan()
{
   rc3d_windows_frame::instance()->initialize_camera_N_string_scan();
}


void rc3d_menus::init_3_camera_debug_temp()
{
   rc3d_windows_frame::instance()->initialize_camera_3_debug_temp();
}
void rc3d_menus::dots_in_cube()
{
   rc3d_windows_frame::instance()->dots_in_cube();
}


void rc3d_menus::draw_feats_dots()
{
   rc3d_windows_frame::instance()->draw_feats_dots();
}


void rc3d_menus::read_iv()
{
   rc3d_windows_frame::instance()->read_iv();
}

void rc3d_menus::read_vrml()
{
   rc3d_windows_frame::instance()->read_vrml();
}



void rc3d_menus::test()
{
   rc3d_windows_frame::instance()->test();
}

void rc3d_menus::batch()
{
   rc3d_windows_frame::instance()->batch();
}
void rc3d_menus::batch_front_windshield()
{
   rc3d_windows_frame::instance()->batch_front_windshield();
}



////////////////////////////2 view add


void rc3d_menus::model_probe_dependency_test_2_2view()
{
   rc3d_windows_frame::instance()->model_probe_dependency_test_2_2view();
}
//

void rc3d_menus::read_models_2view()
{
   rc3d_windows_frame::instance()->read_models_2view();
}

void rc3d_menus::select_probe_tripplet_2view(){
   rc3d_windows_frame::instance()->select_probe_tripplet_2view();
}

void rc3d_menus::read_distance_one_shot_2view(){
   rc3d_windows_frame::instance()->read_distance_one_shot_2view();
}

void rc3d_menus::Detector_WC_infile_2view()
{
   rc3d_windows_frame::instance()->Detector_WC_infile_2view();
}

void rc3d_menus::IR_mxl_monte_carlo_2view(){
   rc3d_windows_frame::instance()->IR_mxl_monte_carlo_2view();
}
