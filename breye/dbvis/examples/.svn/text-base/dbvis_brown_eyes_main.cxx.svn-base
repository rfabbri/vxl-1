#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "dbvis_brown_eyes_menu.h"
#include <bvis/bvis_macros.h>
#include <bvis/bvis_video_manager.h>
#include <bvis/bvis_displayer_sptr.h>
#include <vidpro/vidpro_process_manager_sptr.h>
#include <vidpro/vidpro_process_manager.h>
#include <vidpro/vidpro_repository.h>


// Storage type header files
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vtol_storage.h>
#include <vidpro/storage/vidpro_fmatrix_storage.h>
#include <vidpro/vidpro_process_manager.h>
#include <bvis/bvis_video_manager.h>
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

//#include <dvidpro/storage/vidpro_shockshape_storage.h>
//#include <dvidpro/storage/vidpro_Xshock_storage.h>
//#include <dvidpro/storage/vidpro_curve_storage.h>
//#include <dvidpro/storage/vidpro_curvematch_storage.h>
//#include <dvidpro/storage/vidpro_keypoint_storage.h>
//#include <dvidpro/storage/vidpro_gradassign_storage.h>
//#include <dvidpro/storage/vidpro_bcdg_storage.h>

// Displayer header files
#include <bvis/displayer/bvis_image_displayer.h>
#include <bvis/displayer/bvis_vsol2D_displayer.h>
#include <bvis/displayer/bvis_vtol_displayer.h>

//#include <dbmrf/vis/dbmrf_bmrf_displayer.h>
#include <dbvrl/vis/dbvrl_region_displayer.h>
#include <dbecl/vis/dbecl_episeg_displayer.h>
#include <dbsk2d/vis/dbsk2d_shock_displayer.h>

#include <dbcvr/vis/dbcvr_curvematch_displayer.h>

//#include <dbvis/displayer/dbvis_Xshock_displayer.h>
//#include <dbvis/displayer/dbvis_curve_displayer.h>
//#include <dbvis/displayer/dbvis_curvematch_displayer.h>
//#include <dbvis/displayer/dbvis_keypoint_displayer.h>
//#include <dbvis/displayer/dbvis_ctrk_displayer.h>
//#include <dbvis/displayer/dbvis_edgetrk_displayer.h>
//#include <dbvis/displayer/dbvis_gradassign_displayer.h>
//#include <dbvis/displayer/dbvis_shockshape_displayer.h>

// Process header files
#include <vidpro/process/vidpro_load_image_process.h>
#include <vidpro/process/vidpro_load_video_process.h>
#include <vidpro/process/vidpro_brightness_contrast_process.h>
#include <vidpro/process/vidpro_harris_corners_process.h>
#include <vidpro/process/vidpro_gaussian_blur_process.h>
#include <vidpro/process/vidpro_grey_image_process.h>
#include <vidpro/process/vidpro_VD_edge_process.h>
#include <vidpro/process/vidpro_grey_image_process.h>
#include <vidpro/process/vidpro_motion_process.h>
#include <vidpro/process/vidpro_frame_diff_process.h>

#include <vidpro/process/vidpro_background_diff_process.h>
#include <vidpro/process/vidpro_load_con_process.h>
#include <vidpro/process/vidpro_load_edg_process.h>
#include <vidpro/process/vidpro_load_cem_process.h>
#include <vidpro/process/vidpro_save_cem_process.h>
#include <vidpro/process/vidpro_save_con_process.h>
#include <vidpro/process/vidpro_save_ps_process.h>
#include <vidpro/process/vidpro_edgeprune_process.h>
#include <vidpro/process/vidpro_kl_process.h>
#include <vidpro/process/vidpro_smoothcem_process.h>


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

//#include <dbcvr/pro/dbcvr_interp_cvmatch_process.h>

//#include <dvidpro/process/vidpro_sample_ishock_process.h>
//#include <dvidpro/process/vidpro_curvematch_process.h>
//#include <dvidpro/process/vidpro_logical_linear_process.h>
//#include <dvidpro/process/vidpro_susan_process.h>
//#include <dvidpro/process/vidpro_contourtracing_process.h>
//#include <dvidpro/process/vidpro_lowe_keypoint_process.h>
//#include <dvidpro/process/vidpro_keypoint_match_process.h>
//#include <dvidpro/process/vidpro_gradassign_process.h>
//#include <dvidpro/process/vidpro_eulerspiral_process.h>
//#include <dvidpro/process/vidpro_anlz_two_es_process.h>
//#include <dvidpro/process/vidpro_cvt_xtrema_process.h>
//#include <dvidpro/process/vidpro_es_contour_process.h>
//#include <dvidpro/process/vidpro_anlz_es_contour_process.h>
//#include <dvidpro/process/vidpro_finger_process.h>

#ifdef HAS_BGUI3D
#include <bgui3d/bgui3d.h>
//#include <dbmrf/vis/SoCurvel3D.h>
#include <dbetl/vis/dbetl_track_displayer.h>
//#include <dbmrf/vis/dbmrf_curvel_3d_displayer.h>
//#include <dbvis/displayer/dbvis_bcdg_displayer.h>
#endif
 
//#include <dbcvr/vis/dbcvr_curvematch_displayer.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);

#ifdef HAS_BGUI3D
  bgui3d_init();
  //: Initialize specialized Coin3d nodes
  SoCurvel3D::initClass();
#endif

  // Register the displayers
  REG_DISPLAYER( bvis_image_displayer,bvis_video_manager );
  REG_DISPLAYER( bvis_vsol2D_displayer,bvis_video_manager );
  REG_DISPLAYER( bvis_vtol_displayer,bvis_video_manager );
  // --- dbvis displayers ---
  ////////////////////////////////////////////REG_DISPLAYER( dbmrf_bmrf_displayer );
  ////////////////////////////////////////////REG_DISPLAYER( dbvrl_region_displayer );
  ////////////////////////////////////////////REG_DISPLAYER( dbecl_episeg_displayer );
  ////////////////////////////////////////////REG_DISPLAYER( dbsk2d_shock_displayer );
  //REG_DISPLAYER( dbvis_curve_displayer );
  
  //REG_DISPLAYER( dbvis_keypoint_displayer );
  //REG_DISPLAYER( dbvis_gradassign_displayer );
  //REG_DISPLAYER( dbvis_ctrk_displayer );
  //REG_DISPLAYER( dbvis_edgetrk_displayer );
  //REG_DISPLAYER( dbvis_shockshape_displayer );

  ////////////////////////////////////////////////////REG_DISPLAYER( dbcvr_curvematch_displayer );

#ifdef HAS_BGUI3D
  REG_DISPLAYER( dbetl_track_displayer,bvis_video_manager );
  //REG_DISPLAYER( dbmrf_curvel_3d_displayer,bvis_video_manager );
  //REG_DISPLAYER( dbvis_bcdg_displayer );
#endif
   
  // Register the storage types
  REG_STORAGE( vidpro_image_storage );
  REG_STORAGE( vidpro_vsol2D_storage );
  REG_STORAGE( vidpro_vtol_storage );
  REG_STORAGE( vidpro_fmatrix_storage );
  // --- dvidpro storage types ---
  ////////////////////////////////////////////////////////////REG_STORAGE( dbmrf_bmrf_storage );
  ////////////////////////////////////////////////////////////REG_STORAGE( dbmrf_curvel_3d_storage );
  ////////////////////////////////////////////////////////////REG_STORAGE( dbvrl_region_storage );
  ////////////////////////////////////////////////////////////REG_STORAGE( dbctrk_storage );
  ////////////////////////////////////////////////////////////REG_STORAGE( dbetrk_storage );
  ////////////////////////////////////////////////////////////REG_STORAGE( dbecl_episeg_storage );
  ////////////////////////////////////////////////////////////REG_STORAGE( dbetl_track_storage );
  ////////////////////////////////////////////////////////////REG_STORAGE( dbsk2d_shock_storage );
  ////////////////////////////////////////////////////////////REG_STORAGE( dvpgl_camera_storage );
  //REG_STORAGE( vidpro_curve_storage );
  //REG_STORAGE( vidpro_keypoint_storage );
  //REG_STORAGE( vidpro_gradassign_storage );
  //REG_STORAGE( vidpro_bcdg_storage );
  //REG_STORAGE( vidpro_shockshape_storage );

  //////////////////////////////////////////////////////////////REG_STORAGE( dbcvr_curvematch_storage );

  // Register all the processes
  REG_PROCESS( vidpro_load_image_process, vidpro_process_manager );
  REG_PROCESS( vidpro_load_video_process, vidpro_process_manager );
  REG_PROCESS( vidpro_brightness_contrast_process, vidpro_process_manager );
  REG_PROCESS( vidpro_VD_edge_process, vidpro_process_manager );
  REG_PROCESS( vidpro_harris_corners_process, vidpro_process_manager );
  REG_PROCESS( vidpro_gaussian_blur_process, vidpro_process_manager );
  REG_PROCESS( vidpro_grey_image_process, vidpro_process_manager ); 
  REG_PROCESS( vidpro_frame_diff_process, vidpro_process_manager );  
  REG_PROCESS( vidpro_motion_process, vidpro_process_manager );
  REG_PROCESS( vidpro_background_diff_process, vidpro_process_manager );
  REG_PROCESS( vidpro_load_con_process, vidpro_process_manager );
  REG_PROCESS( vidpro_load_edg_process, vidpro_process_manager );
  REG_PROCESS( vidpro_load_cem_process, vidpro_process_manager );
  REG_PROCESS( vidpro_save_cem_process, vidpro_process_manager );
  REG_PROCESS( vidpro_save_con_process, vidpro_process_manager );
  REG_PROCESS( vidpro_save_ps_process, vidpro_process_manager );
  REG_PROCESS( vidpro_edgeprune_process, vidpro_process_manager );
  REG_PROCESS( vidpro_smoothcem_process, vidpro_process_manager );
  REG_PROCESS( vidpro_kl_process, vidpro_process_manager );
  ////////////////////////////////////////////////////////////REG_PROCESS( dbil_wshed2d_process );

  // --- other processes ---
  //////////////////////////////////////////////////////////REG_PROCESS( dbmrf_network_builder_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbmrf_curve_3d_builder_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbmrf_curve_projector_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbetrk_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbctrk_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbecl_builder_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbetl_tracker_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbvrl_video_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbvrl_region_process );

  ////////////////////////////////////////////////////////////REG_PROCESS( dbsk2d_compute_ishock_process );
  ////////////////////////////////////////////////////////////REG_PROCESS( dbsk2d_load_bnd_process );
  ////////////////////////////////////////////////////////////REG_PROCESS( dbsk2d_save_bnd_process );

  //REG_PROCESS( vidpro_logical_linear_process );
  //REG_PROCESS( vidpro_susan_process );
  
  //REG_PROCESS( vidpro_sample_ishock_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbcvr_curvematch_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbcvr_interp_cvmatch_process );

  //REG_PROCESS( vidpro_contourtracing_process );
  //REG_PROCESS( vidpro_lowe_keypoint_process );
  //REG_PROCESS( vidpro_keypoint_match_process );
  //REG_PROCESS( dbspi_curvemap_process );
  //REG_PROCESS( vidpro_eulerspiral_process );
  //REG_PROCESS( vidpro_anlz_two_es_process );

  //REG_PROCESS( vidpro_finger_process );
  //REG_PROCESS( vidpro_es_contour_process );
  //REG_PROCESS( vidpro_anlz_es_contour_process );

  dbvis_brown_eyes_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu(); 
  unsigned w = 512, h = 512;
  vcl_string title = "Brown Eyes";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis_video_manager::instance() );

  //Amir:: The status bar operations are confusing and often unpredictable
  //until we can get it right, I think it's best to get used to the Ctrl-middlemousebutton
  //to pan the views instead of using the scroll bars

  //win->set_statusbar(true);
  //win->enable_vscrollbar(true);
  //win->enable_hscrollbar(true);
  win->show();
  return vgui::run(); 
}

