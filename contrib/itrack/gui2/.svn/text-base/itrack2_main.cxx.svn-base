// GUI main program for itrack2

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

#include "itrack2_menu.h"

#include <bvis/bvis_macros.h>
#include <bvis/bvis_video_manager.h>
#include <bvis/bvis_displayer_sptr.h>
#include <vidpro/vidpro_process_manager_sptr.h>
#include <vidpro/vidpro_process_manager.h>
#include <vidpro/vidpro_repository.h>

// Storage type header files
#include <vidpro/storage/vidpro_istream_storage.h>
#include <vidpro/storage/vidpro_ostream_storage.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vtol_storage.h>       
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
// Displayer header files
#include <bvis/displayer/bvis_image_displayer.h>
#include <bvis/displayer/bvis_vsol2D_displayer.h>
#include <bvis/displayer/bvis_vtol_displayer.h>
#include <dbinfo/vis/dbinfo_track_displayer.h>
#include <dbbgm/vis/dbbgm_distribution_image_displayer.h>
// Process header files
#include <vidpro/process/vidpro_open_istream_process.h>
#include <vidpro/process/vidpro_open_ostream_process.h>
#include <vidpro/process/vidpro_frame_from_istream_process.h>
#include <vidpro/process/vidpro_load_image_process.h>
#include <vidpro/process/vidpro_load_video_process.h>
#include <vidpro/process/vidpro_save_video_process.h>
#include <dbinfo/pro/dbinfo_load_vj_polys_process.h>
#include <dbinfo/pro/dbinfo_load_tracks_process.h>
#include <dbinfo/pro/dbinfo_save_tracks_process.h>
#include <vidpro/process/vidpro_brightness_contrast_process.h>
#include <vidpro/process/vidpro_harris_corners_process.h>
#include <vidpro/process/vidpro_global_harris_corners_process.h>
#include <vidpro/process/vidpro_VD_edge_process.h>
#include <vidpro/process/vidpro_gaussian_blur_process.h>
#include <vidpro/process/vidpro_grey_image_process.h>
#include <vidpro/process/vidpro_motion_process.h>
#include <vidpro/process/vidpro_frame_diff_process.h>
#include <dbinfo/pro/dbinfo_multi_track_process.h>
#include <dbinfo/pro/dbinfo_refine_tracks_process.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process.h>
//#include <dbbgm/pro/dbbgm_blob_finder_process.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis_image_displayer, bvis_video_manager );
  REG_DISPLAYER( bvis_vsol2D_displayer, bvis_video_manager );
  REG_DISPLAYER( bvis_vtol_displayer, bvis_video_manager );
  ////////////////////////////////////////////////////////REG_DISPLAYER( dbinfo_track_displayer );
  ////////////////////////////////////////////////////////REG_DISPLAYER( dbbgm_distribution_image_displayer );
  // Register the storage types
  REG_STORAGE( vidpro_image_storage );
  REG_STORAGE( vidpro_vsol2D_storage );
  REG_STORAGE( vidpro_vtol_storage );
  REG_STORAGE( vidpro_istream_storage);
  REG_STORAGE( vidpro_ostream_storage);
  ////////////////////////////////////////////////////////REG_STORAGE( dbinfo_track_storage );
  ////////////////////////////////////////////////////////REG_STORAGE( dbbgm_distribution_image_storage );
  // Register all the processes
  REG_PROCESS( vidpro_load_image_process, vidpro_process_manager );
  REG_PROCESS( vidpro_open_istream_process,vidpro_process_manager );
  REG_PROCESS( vidpro_open_ostream_process,vidpro_process_manager );
  REG_PROCESS( vidpro_load_video_process,  vidpro_process_manager );
  REG_PROCESS( vidpro_save_video_process, vidpro_process_manager );
  REG_PROCESS( vidpro_frame_from_istream_process, vidpro_process_manager );
  ////////////////////////////////////////////////////////REG_PROCESS( dbinfo_load_vj_polys_process );
  ////////////////////////////////////////////////////////REG_PROCESS( dbinfo_load_tracks_process );
  ////////////////////////////////////////////////////////REG_PROCESS( dbinfo_save_tracks_process );
  REG_PROCESS( vidpro_brightness_contrast_process, vidpro_process_manager );
  REG_PROCESS( vidpro_VD_edge_process, vidpro_process_manager );
  REG_PROCESS( vidpro_harris_corners_process, vidpro_process_manager );
  REG_PROCESS( vidpro_global_harris_corners_process, vidpro_process_manager );
  REG_PROCESS( vidpro_gaussian_blur_process, vidpro_process_manager );
  REG_PROCESS( vidpro_grey_image_process, vidpro_process_manager );
  REG_PROCESS( vidpro_frame_diff_process, vidpro_process_manager );
  REG_PROCESS( vidpro_motion_process, vidpro_process_manager );
  //////////////////////////////////////////////////////////REG_PROCESS( dbinfo_multi_track_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbinfo_refine_tracks_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbbgm_aerial_bg_model_process );
  //////////////////////////////////////////////////////////REG_PROCESS( dbbgm_aerial_fg_uncertainity_detect_process );
  //REG_PROCESS( dbbgm_blob_finder_process );

#if defined(VCL_WIN32)
vcl_cout << '\n'<< "Max number of open files has been reset from " << _getmaxstdio();
_setmaxstdio(2048);
#endif
  
  itrack2_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 600, h = 512;
  vcl_string title = "itrack2 GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
#if 0
  bvis_video_manager::instance()->add_new_frame();
#endif
  win->get_adaptor()->set_tableau( bvis_video_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}
