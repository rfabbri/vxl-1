#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include <vgui/vgui_find.h>

#include "mw_menu.h"
#include "geno_app.h"
#include "epip_app.h"
#include "show_contours_process.h"
#include "mw_app.h"
#include "ctspheres_app.h"

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
#include <dvpgl/pro/dvpgl_camera_storage.h>
#include <dbsol/pro/dbsol_curve_sampling_process.h>
#include <dbdet/pro/dbdet_lvwr_process.h>
#include <dbctrk/pro/dbctrk_storage.h>
#include <dbecl/pro/dbecl_episeg_storage.h>
#include <dbetl/pro/dbetl_track_storage.h>
#include <dbcvr/pro/dbcvr_curvematch_storage.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <mw/pro/mw_discrete_corresp_storage.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include <dbecl/vis/dbecl_episeg_displayer.h>
#include <dbcvr/vis/dbcvr_curvematch_displayer.h>
#include <dbdet/vis/dbdet_sel_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
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
#include <vidpro1/process/vidpro1_smoothcem_process.h>
#include <vidpro1/process/vidpro1_kl_process.h>
#include <vidpro1/process/vidpro1_smoothcem_process.h>
#include <dbcvr/pro/dbcvr_curvematch_process.h>
#include <dbcvr/pro/dbcvr_interp_cvmatch_process.h>
#include <dbctrk/pro/dbctrk_process.h>
#include <dbecl/pro/dbecl_builder_process.h>
#include <dbetl/pro/dbetl_tracker_process.h>
#include <dbnl/algo/pro/dbnl_function_sampling_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbdet/pro/dbdet_nms_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <mw/pro/dvpgl_load_camera_process.h>
#include <mw/pro/mw_cvmatch_process.h>
#include <mw/pro/mw_load_discrete_corresp_process.h>
#include <mw/pro/dbdet_edgel_data_process.h>



int main(int argc, char** argv)
{

  vgui::init(argc, argv);

  // Register the displayers
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );

  REG_DISPLAYER( dbecl_episeg_displayer );
  REG_DISPLAYER( dbcvr_curvematch_displayer );
  REG_DISPLAYER( dbdet_sel_displayer );



  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  // --- dvidpro storage types ---
  REG_STORAGE( dbctrk_storage );
  REG_STORAGE( dbecl_episeg_storage );
  REG_STORAGE( dbetl_track_storage );
  REG_STORAGE( dvpgl_camera_storage );
  REG_STORAGE( dbcvr_curvematch_storage );
  REG_STORAGE( dbdet_sel_storage );
  REG_STORAGE( mw_discrete_corresp_storage );

  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
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

  REG_PROCESS( dvpgl_load_camera_process );
  REG_PROCESS( show_contours_process );


  // --- other processes ---
  REG_PROCESS( dbcvr_curvematch_process );
  REG_PROCESS( dbcvr_interp_cvmatch_process );
  REG_PROCESS( dbctrk_process );
  REG_PROCESS( dbecl_builder_process );
  
  REG_PROCESS( dbsol_curve_sampling_process );
  REG_PROCESS( dbnl_function_sampling_process );
  REG_PROCESS( dbdet_lvwr_process );

  REG_PROCESS( dbdet_contour_tracer_process );
  REG_PROCESS( dbdet_nms_process );
  REG_PROCESS( dbdet_sel_process );

  REG_PROCESS( mw_cvmatch_process );
  REG_PROCESS( mw_load_discrete_corresp_process );
  REG_PROCESS( dbdet_edgel_data_process );
  REG_PROCESS( vidpro1_load_vsol_process );
  REG_PROCESS( vidpro1_save_vsol_process );

  mw_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu(); 
  unsigned w = 2048, h = 1000;
  vcl_string title = "Brown Eyes";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  
  // insert geno shortcut tableau
//  vgui_shell_tableau_sptr psh;
//  psh.vertical_cast(vgui_find_below_by_type_name(MANAGER,"vgui_shell_tableau"));
//  vgui_tableau_sptr pgtab = geno_shortcut_tableau_new();
//  psh->add(pgtab);

  win->show();

  if (argc >= 2)
     call_show_contours_process(argv[1]);

  return vgui::run(); 
}
