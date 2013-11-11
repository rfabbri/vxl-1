// This is contrib/ozge/gui/matching_gui_main.cxx

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

#include "matching_gui_menu.h"


#include <bvis1/bvis1_macros.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_displayer_sptr.h>
#include <vidpro1/vidpro1_process_manager_sptr.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/vidpro1_repository.h>

// Storage type header files
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbskr/pro/dbskr_shock_match_storage.h>
#include <dbru/pro/dbru_rcor_storage.h>
#include <dbru/pro/dbru_osl_storage.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include <dbsk2d/vis/dbsk2d_shock_displayer.h>
#include <dbskr/vis/dbskr_shock_match_displayer.h>
#include <dbru/vis/dbru_rcor_displayer.h>
#include <dbru/vis/dbru_osl_displayer.h>


// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_save_video_process.h>
#include <vidpro1/process/vidpro1_load_video_and_polys_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>

//#include <vidpro1/process/vidpro1_background_diff_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/process/vidpro1_load_edg_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_edgeprune_process.h>
#include <vidpro1/process/vidpro1_smoothcem_process.h>

#include <dbdet/pro/dbdet_contour_tracer_process.h>

//#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
//#include <dbsk2d/pro/dbsk2d_prune_ishock_process.h>
//#include <dbsk2d/pro/dbsk2d_sample_ishock_process.h>
//#include <dbsk2d/pro/dbsk2d_compile_rich_map_process.h>
//#include <dbsk2d/pro/dbsk2d_assign_image_process.h>

#include <dbsk2d/pro/dbsk2d_load_bnd_process.h>
#include <dbsk2d/pro/dbsk2d_save_bnd_process.h>
#include <dbsk2d/pro/dbsk2d_load_esf_process.h>
#include <dbsk2d/pro/dbsk2d_save_esf_process.h>

//#include <dbskr/pro/dbskr_compute_sk_path_curve_process.h>
#include <dbskr/pro/dbskr_shock_match_process.h>
#include <dbskr/pro/dbskr_shock_match_pmi_process.h>

#include <dbru/pro/dbru_mutual_info_process.h>
#include <dbru/pro/dbru_dbinfo_process.h>
#include <dbru/pro/dbru_compute_rcor_process.h>
#include <dbru/pro/dbru_load_osl_process.h>
#include <dbru/pro/dbru_load_polygons_process.h>
//#include <dbru/pro/dbru_run_osl_process.h>
#include <dbru/pro/dbru_create_empty_osl_process.h>
#include <dbru/pro/dbru_save_osl_process.h>

#include <dbacm/pro/dbacm_geodesic_active_contour_process.h>
#include <dbsol/pro/dbsol_roi_image_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>

#include <ozge/pro/dbru_show_tps_process.h>


#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <dbbgm/vis/dbbgm_distribution_image_displayer.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process.h>
#include <dbbgm/pro/dbbgm_load_bg_model_process.h>
#include <dbbgm/pro/dbbgm_save_bg_model_process.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process.h>
#include <dbdet/pro/dbdet_blob_finder_process.h>
#include <dbsol/pro/dbsol_scan_polygon_process.h>
int main(int argc, char** argv)
{
#if defined(VCL_WIN32)
vcl_cout << '\n'<< "Max number of open files has been reset from " << _getmaxstdio();
_setmaxstdio(1500);
#endif

  vgui::init(argc, argv);

  // Register the displayers
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  //REG_DISPLAYER( dbsk2d_shock_displayer );
  REG_DISPLAYER( dbskr_shock_match_displayer );
  REG_DISPLAYER( dbru_rcor_displayer );
  REG_DISPLAYER( dbru_osl_displayer );
  REG_DISPLAYER( dbbgm_distribution_image_displayer );

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  //REG_STORAGE( dbsk2d_shock_storage );
  REG_STORAGE( dbskr_shock_match_storage );
  REG_STORAGE( dbru_rcor_storage );
  REG_STORAGE( dbru_osl_storage );
    REG_STORAGE( dbbgm_distribution_image_storage );

  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_save_video_process );
  REG_PROCESS( vidpro1_load_video_and_polys_process );
  REG_PROCESS( vidpro1_brightness_contrast_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_grey_image_process ); 

  //REG_PROCESS( vidpro1_RGB_to_IHS_process );
  REG_PROCESS( vidpro1_VD_edge_process );
  REG_PROCESS( vidpro1_harris_corners_process );
  //REG_PROCESS( vidpro1_gaussian_blur_process );
  //REG_PROCESS( vidpro1_grey_image_process ); 
 // REG_PROCESS( vidpro1_frame_diff_process );  
  //REG_PROCESS( vidpro1_motion_process );
  //REG_PROCESS( vidpro1_background_diff_process );
  REG_PROCESS( vidpro1_load_con_process );
  REG_PROCESS( vidpro1_load_edg_process );
  REG_PROCESS( vidpro1_load_cem_process );
  REG_PROCESS( vidpro1_save_cem_process );
  REG_PROCESS( vidpro1_save_con_process );
  REG_PROCESS( vidpro1_edgeprune_process );
  REG_PROCESS( vidpro1_smoothcem_process );
  REG_PROCESS( dbbgm_aerial_bg_model_process );
  REG_PROCESS( dbbgm_load_bg_model_process );
  REG_PROCESS( dbbgm_save_bg_model_process );
  REG_PROCESS( dbbgm_aerial_fg_uncertainity_detect_process);
  REG_PROCESS( dbdet_blob_finder_process );
  REG_PROCESS( dbsol_scan_polygon_process );


  // --- other processes ---
  REG_PROCESS( dbdet_contour_tracer_process );
  REG_PROCESS( dbsk2d_load_bnd_process );
  REG_PROCESS( dbsk2d_save_bnd_process );
  //REG_PROCESS( dbsk2d_compute_ishock_process );
 // REG_PROCESS( dbsk2d_prune_ishock_process );
  //REG_PROCESS( dbsk2d_sample_ishock_process );
  //REG_PROCESS( dbsk2d_compile_rich_map_process );
  //REG_PROCESS( dbsk2d_assign_image_process );
  REG_PROCESS( dbsk2d_load_esf_process );
  REG_PROCESS( dbsk2d_save_esf_process );
  
  //REG_PROCESS( dbskr_compute_sk_path_curve_process );
  REG_PROCESS( dbskr_shock_match_process );
  REG_PROCESS( dbskr_shock_match_pmi_process );

  REG_PROCESS( dbru_dbinfo_process );
  REG_PROCESS( dbru_compute_rcor_process );
  REG_PROCESS( dbru_mutual_info_process );
  REG_PROCESS( dbru_load_osl_process );
  REG_PROCESS( dbru_load_polygons_process );
  REG_PROCESS( dbru_save_osl_process );
  REG_PROCESS( dbru_create_empty_osl_process );
  //REG_PROCESS( dbru_run_osl_process );
  REG_PROCESS( dbacm_geodesic_active_contour_process );
  REG_PROCESS( dbsol_roi_image_process );
  //REG_PROCESS( dbinfo_multi_track_process );
  REG_PROCESS( dbru_show_tps_process );

  matching_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu(); 
  unsigned w = 512, h = 512;
  vcl_string title = "Shock 2D GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );

  //Amir:: The status bar operations are confusing and often unpredictable
  //until we can get it right, I think it's best to get used to the Ctrl-middlemousebutton
  //to pan the views instead of using the scroll bars

  //win->set_statusbar(true);
  //win->enable_vscrollbar(true);
  //win->enable_hscrollbar(true);
  win->show();
  return vgui::run(); 
}

