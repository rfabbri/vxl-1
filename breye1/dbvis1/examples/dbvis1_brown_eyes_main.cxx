#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "dbvis1_brown_eyes_menu.h"
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
#include <vidpro1/storage/vidpro1_fmatrix_storage.h>

//#include <dbmrf/pro/dbmrf_bmrf_storage.h>
//#include <dbmrf/pro/dbmrf_curvel_3d_storage.h>
#include <dbvrl/pro/dbvrl_region_storage.h>
#include <dbctrk/pro/dbctrk_storage.h>
#include <dbetrk/pro/dbetrk_storage.h>
#include <dbecl/pro/dbecl_episeg_storage.h>
#include <dbetl/pro/dbetl_track_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dvpgl/pro/dvpgl_camera_storage.h>


#include <dbcvr/pro/dbcvr_curvematch_storage.h>
#include <dbcvr/pro/dbcvr_curvematch_process.h>
#include <dbcvr/pro/dbcvr_interp_cvmatch_process.h>

//#include <dvidpro1/storage/vidpro1_shockshape_storage.h>
//#include <dvidpro1/storage/vidpro1_Xshock_storage.h>
//#include <dvidpro1/storage/vidpro1_curve_storage.h>
//#include <dvidpro1/storage/vidpro1_curvematch_storage.h>
//#include <dvidpro1/storage/vidpro1_keypoint_storage.h>
//#include <dvidpro1/storage/vidpro1_gradassign_storage.h>
//#include <dvidpro1/storage/vidpro1_bcdg_storage.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>

//#include <dbmrf/vis/dbmrf_bmrf_displayer.h>
#include <dbvrl/vis/dbvrl_region_displayer.h>
#include <dbecl/vis/dbecl_episeg_displayer.h>
#include <dbsk2d/vis/dbsk2d_shock_displayer.h>

#include <dbcvr/vis/dbcvr_curvematch_displayer.h>

//#include <dbvis1/displayer/dbvis1_Xshock_displayer.h>
//#include <dbvis1/displayer/dbvis1_curve_displayer.h>
//#include <dbvis1/displayer/dbvis1_curvematch_displayer.h>
//#include <dbvis1/displayer/dbvis1_keypoint_displayer.h>
//#include <dbvis1/displayer/dbvis1_ctrk_displayer.h>
//#include <dbvis1/displayer/dbvis1_edgetrk_displayer.h>
//#include <dbvis1/displayer/dbvis1_gradassign_displayer.h>
//#include <dbvis1/displayer/dbvis1_shockshape_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_motion_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>

#include <vidpro1/process/vidpro1_background_diff_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/process/vidpro1_load_edg_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_save_ps_process.h>
#include <vidpro1/process/vidpro1_edgeprune_process.h>
#include <vidpro1/process/vidpro1_kl_process.h>
#include <vidpro1/process/vidpro1_smoothcem_process.h>


//#include <dbmrf/pro/dbmrf_network_builder_process.h>
//#include <dbmrf/pro/dbmrf_curve_3d_builder_process.h>
//#include <dbmrf/pro/dbmrf_curve_projector_process.h>
#include <dbvrl/pro/dbvrl_video_process.h>
#include <dbvrl/pro/dbvrl_region_process.h>
#include <dbctrk/pro/dbctrk_process.h>
#include <dbspi/pro/dbspi_curvemap_process.h>
#include <dbetrk/pro/dbetrk_process.h>
#include <dbecl/pro/dbecl_builder_process.h>
#include <dbetl/pro/dbetl_tracker_process.h>

#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_load_bnd_process.h>
#include <dbsk2d/pro/dbsk2d_save_bnd_process.h>
#include <dbil/pro/dbil_wshed2d_process.h>

#include <dbctrk/vis/dbctrk_displayer.h>

//#include <dbcvr/pro/dbcvr_interp_cvmatch_process.h>

//#include <dvidpro1/process/vidpro1_sample_ishock_process.h>
//#include <dvidpro1/process/vidpro1_curvematch_process.h>
//#include <dvidpro1/process/vidpro1_logical_linear_process.h>
//#include <dvidpro1/process/vidpro1_susan_process.h>
//#include <dvidpro1/process/vidpro1_contourtracing_process.h>
//#include <dvidpro1/process/vidpro1_lowe_keypoint_process.h>
//#include <dvidpro1/process/vidpro1_keypoint_match_process.h>
//#include <dvidpro1/process/vidpro1_gradassign_process.h>
//#include <dvidpro1/process/vidpro1_eulerspiral_process.h>
//#include <dvidpro1/process/vidpro1_anlz_two_es_process.h>
//#include <dvidpro1/process/vidpro1_cvt_xtrema_process.h>
//#include <dvidpro1/process/vidpro1_es_contour_process.h>
//#include <dvidpro1/process/vidpro1_anlz_es_contour_process.h>
//#include <dvidpro1/process/vidpro1_finger_process.h>

#ifdef VXL_BGUI3D_FOUND
#include <bgui3d/bgui3d.h>
//#include <dbmrf/vis/SoCurvel3D.h>
#include <dbetl/vis/dbetl_track_displayer.h>
//#include <dbmrf/vis/dbmrf_curvel_3d_displayer.h>
//#include <dbvis1/displayer/dbvis1_bcdg_displayer.h>
#endif
 
//#include <dbcvr/vis/dbcvr_curvematch_displayer.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);

#ifdef VXL_BGUI3D_FOUND
  bgui3d_init();
  //: Initialize specialized Coin3d nodes
  SoCurvel3D::initClass();
#endif

  // Register the displayers
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  // --- dbvis1 displayers ---
//  REG_DISPLAYER( dbmrf_bmrf_displayer );
  REG_DISPLAYER( dbvrl_region_displayer );
  REG_DISPLAYER( dbecl_episeg_displayer );
  REG_DISPLAYER( dbsk2d_shock_displayer );
  //REG_DISPLAYER( dbvis1_curve_displayer );
  
  //REG_DISPLAYER( dbvis1_keypoint_displayer );
  //REG_DISPLAYER( dbvis1_gradassign_displayer );
  REG_DISPLAYER( dbctrk_displayer );
  //REG_DISPLAYER( dbvis1_edgetrk_displayer );
  //REG_DISPLAYER( dbvis1_shockshape_displayer );

  REG_DISPLAYER( dbcvr_curvematch_displayer );

#ifdef VXL_BGUI3D_FOUND
  REG_DISPLAYER( dbetl_track_displayer );
//  REG_DISPLAYER( dbmrf_curvel_3d_displayer );
  //REG_DISPLAYER( dbvis1_bcdg_displayer );
#endif
   
  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( vidpro1_fmatrix_storage );
  // --- dvidpro storage types ---
//  REG_STORAGE( dbmrf_bmrf_storage );
//  REG_STORAGE( dbmrf_curvel_3d_storage );
  REG_STORAGE( dbvrl_region_storage );
  REG_STORAGE( dbctrk_storage );
  REG_STORAGE( dbetrk_storage );
  REG_STORAGE( dbecl_episeg_storage );
  REG_STORAGE( dbetl_track_storage );
  REG_STORAGE( dbsk2d_shock_storage );
  REG_STORAGE( dvpgl_camera_storage );
  //REG_STORAGE( vidpro1_curve_storage );
  //REG_STORAGE( vidpro1_keypoint_storage );
  //REG_STORAGE( vidpro1_gradassign_storage );
  //REG_STORAGE( vidpro1_bcdg_storage );
  //REG_STORAGE( vidpro1_shockshape_storage );

  REG_STORAGE( dbcvr_curvematch_storage );

  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_brightness_contrast_process );
  REG_PROCESS( vidpro1_VD_edge_process );
  REG_PROCESS( vidpro1_harris_corners_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_grey_image_process ); 
  REG_PROCESS( vidpro1_frame_diff_process );  
  REG_PROCESS( vidpro1_motion_process );
  REG_PROCESS( vidpro1_background_diff_process );
  REG_PROCESS( vidpro1_load_con_process );
  REG_PROCESS( vidpro1_load_edg_process );
  REG_PROCESS( vidpro1_load_cem_process );
  REG_PROCESS( vidpro1_save_cem_process );
  REG_PROCESS( vidpro1_save_con_process );
  REG_PROCESS( vidpro1_save_ps_process );
  REG_PROCESS( vidpro1_edgeprune_process );
  REG_PROCESS( vidpro1_smoothcem_process );
  REG_PROCESS( vidpro1_kl_process );
  REG_PROCESS( dbil_wshed2d_process );

  // --- other processes ---
//  REG_PROCESS( dbmrf_network_builder_process );
//  REG_PROCESS( dbmrf_curve_3d_builder_process );
//  REG_PROCESS( dbmrf_curve_projector_process );
  REG_PROCESS( dbetrk_process );
  REG_PROCESS( dbctrk_process );
  REG_PROCESS( dbecl_builder_process );
  REG_PROCESS( dbetl_tracker_process );
  REG_PROCESS( dbvrl_video_process );
  REG_PROCESS( dbvrl_region_process );

  REG_PROCESS( dbsk2d_compute_ishock_process );
  REG_PROCESS( dbsk2d_load_bnd_process );
  REG_PROCESS( dbsk2d_save_bnd_process );

  //REG_PROCESS( vidpro1_logical_linear_process );
  //REG_PROCESS( vidpro1_susan_process );
  
  //REG_PROCESS( vidpro1_sample_ishock_process );
  REG_PROCESS( dbcvr_curvematch_process );
  REG_PROCESS( dbcvr_interp_cvmatch_process );

  //REG_PROCESS( vidpro1_contourtracing_process );
  //REG_PROCESS( vidpro1_lowe_keypoint_process );
  //REG_PROCESS( vidpro1_keypoint_match_process );
  //REG_PROCESS( dbspi_curvemap_process );
  //REG_PROCESS( vidpro1_eulerspiral_process );
  //REG_PROCESS( vidpro1_anlz_two_es_process );

  //REG_PROCESS( vidpro1_finger_process );
  //REG_PROCESS( vidpro1_es_contour_process );
  //REG_PROCESS( vidpro1_anlz_es_contour_process );

  dbvis1_brown_eyes_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu(); 
  unsigned w = 512, h = 512;
  vcl_string title = "Brown Eyes";
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

