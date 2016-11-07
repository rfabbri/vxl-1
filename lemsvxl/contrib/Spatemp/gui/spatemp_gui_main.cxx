#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "spatemp_gui_menu.h"
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
 
// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_motion_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>
#include <vidpro1/process/vidpro1_kl_process.h>


#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/vis/dbdet_edgemap_displayer.h>
#include <dbdet/pro/dbdet_load_edg_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>

#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>


#include <dbdet/pro/dbdet_sel_process.h>
#include <dbdet/pro/dbdet_sel_storage.h>

#include <dbdet/vis/dbdet_sel_displayer.h>

#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_generic_linker_process.h>
#include <dbdet/pro/dbdet_hybrid_linker_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>


#include <Spatemp/pro/spatemp_display_edge_maps_process.h>
#include <Spatemp/pro/spatemp_form_temporal_bundles_process.h>
#include <Spatemp/pro/dbdet_temporal_map_storage.h>
#include <Spatemp/vis/dbdet_temporal_map_displayer.h>
#include <Spatemp/pro/vidpro1_load_video_edgemap_process.h>
#include <Spatemp/pro/spatemp_forward_beta_process.h>
#include <Spatemp/pro/spatemp_backward_beta_process.h>

#include <dbctrk/pro/dbctrk_process.h>
#include <dbctrk/pro/dbctrk_curve_detector_process.h>
#include <dbctrk/pro/dbctrk_storage.h>
#include <dbctrk/vis/dbctrk_displayer.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  REG_DISPLAYER( dbdet_edgemap_displayer );
  REG_DISPLAYER( dbdet_sel_displayer );
  REG_DISPLAYER( dbdet_temporal_map_displayer );
  REG_DISPLAYER( dbctrk_displayer );

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( vidpro1_fmatrix_storage );
  REG_STORAGE( dbdet_edgemap_storage );
  REG_STORAGE( dbdet_sel_storage );
  REG_STORAGE( dbdet_temporal_map_storage );
  REG_STORAGE( dbctrk_storage );
  
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
  REG_PROCESS( vidpro1_kl_process );
  REG_PROCESS( dbctrk_process );
  REG_PROCESS( dbctrk_curve_detector_process );

  REG_PROCESS( dbdet_sel_extract_contours_process );
  REG_PROCESS( dbdet_generic_linker_process );
  REG_PROCESS( dbdet_hybrid_linker_process );
  REG_PROCESS( dbdet_sel_process );
  REG_PROCESS( vidpro1_load_video_edgemap_process );
  REG_PROCESS( dbdet_save_edg_process );
  REG_PROCESS( dbdet_load_edg_process );

  //: temporal tracking
  REG_PROCESS( dbdet_third_order_edge_detector_process);
  REG_PROCESS( spatemp_display_edge_maps_process );
  REG_PROCESS( spatemp_form_temporal_bundles_process);
  REG_PROCESS( spatemp_forward_beta_process);
    REG_PROCESS( spatemp_backward_beta_process);

  spatemp_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "Brown Eyes";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}
