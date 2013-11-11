// GUI main program for itrack

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "itrack_menu.h"
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
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include <dbinfo/vis/dbinfo_track_displayer.h>
#include <dbbgm/vis/dbbgm_distribution_image_displayer.h>
// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_save_video_process.h>
#include <dbinfo/pro/dbinfo_load_vj_polys_process.h>
#include <dbinfo/pro/dbinfo_load_tracks_process.h>
#include <dbinfo/pro/dbinfo_save_tracks_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_motion_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>
#include <dbinfo/pro/dbinfo_multi_track_process.h>
#include <dbinfo/pro/dbinfo_refine_tracks_process.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process.h>
//#include <dbbgm/pro/dbbgm_blob_finder_process.h>
int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  REG_DISPLAYER( dbinfo_track_displayer );
  REG_DISPLAYER( dbbgm_distribution_image_displayer );
  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( dbinfo_track_storage );
  REG_STORAGE( dbbgm_distribution_image_storage );
  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_save_video_process );
  REG_PROCESS( dbinfo_load_vj_polys_process );
  REG_PROCESS( dbinfo_load_tracks_process );
  REG_PROCESS( dbinfo_save_tracks_process );
  REG_PROCESS( vidpro1_brightness_contrast_process );
  REG_PROCESS( vidpro1_VD_edge_process );
  REG_PROCESS( vidpro1_harris_corners_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_grey_image_process );
  REG_PROCESS( vidpro1_frame_diff_process );
  REG_PROCESS( vidpro1_motion_process );
  REG_PROCESS( dbinfo_multi_track_process );
  REG_PROCESS( dbinfo_refine_tracks_process );
  REG_PROCESS( dbbgm_aerial_bg_model_process );
  REG_PROCESS( dbbgm_aerial_fg_uncertainity_detect_process );
//<<<<<<< itrack_main.cxx
 // REG_PROCESS( dbbgm_blob_finder_process );
//=======
  //REG_PROCESS( dbbgm_blob_finder_process );
////>>>>>>> 1.7
#if defined(VCL_WIN32)
vcl_cout << '\n'<< "Max number of open files has been reset from " << _getmaxstdio();
_setmaxstdio(2048);
#endif
  
  itrack_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 600, h = 512;
  vcl_string title = "itrack GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
#if 0
  bvis1_manager::instance()->add_new_frame();
#endif
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}
