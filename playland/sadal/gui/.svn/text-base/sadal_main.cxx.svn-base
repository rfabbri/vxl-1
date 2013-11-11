// GUI main program for sadal

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "sadal_menu.h"
#include <bvis/bvis_macros.h>
#include <bvis/bvis_manager.h>
#include <bvis/bvis_displayer_sptr.h>
#include <vidpro/vidpro_process_manager_sptr.h>
#include <vidpro/vidpro_process_manager.h>
#include <vidpro/vidpro_repository.h>

// Storage type header files
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vtol_storage.h>  
#include <vidpro/storage/vidpro_vsol3D_storage.h>     
#include <vidpro/storage/vidpro_bmrf_storage.h> 
#include <vidpro/storage/vidpro_fmatrix_storage.h> 
#include <dvidpro/storage/vidpro_shock_storage.h>
#include <dvidpro/storage/vidpro_Xshock_storage.h>
#include <dvidpro/storage/vidpro_curve_storage.h>
#include <dvidpro/storage/vidpro_curvematch_storage.h>
#include <dvidpro/storage/vidpro_keypoint_storage.h>
#include <dvidpro/storage/vidpro_bvrl_storage.h>
#include <dvidpro/storage/vidpro_gradassign_storage.h>
#include <dvidpro/storage/vidpro_ctrk_storage.h>
#include <dvidpro/storage/vidpro_edgetrk_storage.h>
#include <dvidpro/storage/vidpro_episeg_storage.h>
  #include <vidpro/storage/vidpro_curvel_3d_storage.h>

// Displayer header files
#include <bvis/displayer/bvis_image_displayer.h>
#include <bvis/displayer/bvis_vsol2D_displayer.h>
#include <bvis/displayer/bvis_vtol_displayer.h>
#if HAS_BGUI3D
#include <dbvis/displayer/dbvis_vsol3D_displayer.h>
#endif
// Process header files
#include <vidpro/process/vidpro_load_image_process.h>
#include <vidpro/process/vidpro_load_video_process.h>
#include <vidpro/process/vidpro_brightness_contrast_process.h>
#include <vidpro/process/vidpro_harris_corners_process.h>
#include <vidpro/process/vidpro_VD_edge_process.h>
#include <vidpro/process/vidpro_gaussian_blur_process.h>
#include <vidpro/process/vidpro_grey_image_process.h>
#include <vidpro/process/vidpro_motion_process.h>
#include <vidpro/process/vidpro_frame_diff_process.h>
#include <vidpro/process/vidpro_harris_corners_process.h>
#include <blem/playland/sadal/gui/vidpro_kltrack_process.h>
#include <blem/playland/sadal/gui/vidpro_factorize_process.h>
#include <blem/playland/sadal/gui/vidpro_new_vsol2d_tablo_process.h>
#include <blem/playland/sadal/gui/vidpro_homog_view_process.h>
#include <blem/playland/sadal/gui/vidpro_affine_homog_est_process.h>
#include <blem/playland/sadal/gui/vidpro_combine_storage_process.h>

// Video Codecs
#include <vidl/vidl_io.h>
#include <vidl/vidl_image_list_codec.h>
#ifdef HAS_MPEG2
#include <vidl/vidl_mpegcodec.h>
#endif
#ifdef VCL_WIN32
#include <vidl/vidl_avicodec.h>
#endif

int main(int argc, char** argv)
{
  vgui::init(argc, argv);

  // register video codecs
  vidl_io::register_codec(new vidl_image_list_codec);
#ifdef HAS_MPEG2
  vidl_io::register_codec(new vidl_mpegcodec);
#endif
#ifdef VCL_WIN32
  vidl_io::register_codec(new vidl_avicodec);
#endif
  
  // Register the displayer
  REG_DISPLAYER( bvis_image_displayer );
  REG_DISPLAYER( bvis_vsol2D_displayer );
  REG_DISPLAYER( bvis_vtol_displayer );
#if HAS_BGUI3D
  REG_DISPLAYER( dbvis_vsol3D_displayer );
#endif
  // Register the storage types
  REG_STORAGE( vidpro_image_storage );
  REG_STORAGE( vidpro_vsol2D_storage );
  REG_STORAGE( vidpro_vtol_storage );
  REG_STORAGE( vidpro_vsol3D_storage );
    REG_STORAGE( vidpro_bmrf_storage );
    REG_STORAGE( vidpro_bvrl_storage );
      REG_STORAGE( vidpro_shock_storage );
  REG_STORAGE( vidpro_Xshock_storage );
  REG_STORAGE( vidpro_curve_storage );
  REG_STORAGE( vidpro_curvematch_storage );
  REG_STORAGE( vidpro_keypoint_storage );

  REG_STORAGE( vidpro_gradassign_storage );
  REG_STORAGE( vidpro_ctrk_storage );
  REG_STORAGE( vidpro_edgetrk_storage );
  REG_STORAGE( vidpro_episeg_storage );
  REG_STORAGE( vidpro_curvel_3d_storage );
  REG_STORAGE( vidpro_fmatrix_storage );
  
  // Register all the processes
  REG_PROCESS( vidpro_load_image_process );
  REG_PROCESS( vidpro_load_video_process );
  REG_PROCESS( vidpro_brightness_contrast_process );
  REG_PROCESS( vidpro_VD_edge_process );
  REG_PROCESS( vidpro_harris_corners_process );
  REG_PROCESS( vidpro_gaussian_blur_process );
  REG_PROCESS( vidpro_grey_image_process );
  REG_PROCESS( vidpro_frame_diff_process );
  REG_PROCESS( vidpro_motion_process );
  REG_PROCESS( vidpro_harris_corners_process );
  REG_PROCESS( vidpro_kltrack_process);
  REG_PROCESS( vidpro_factorize_process );
  REG_PROCESS( vidpro_new_vsol2d_tablo_process);
  REG_PROCESS( vidpro_homog_view_process);
  REG_PROCESS( vidpro_combine_storage_process);

  REG_PROCESS( dvidpro_affine_homog_est_process);
  

  
  sadal_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 1024, h = 768;
  vcl_string title = "sadal GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis_manager::instance() );
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  return vgui::run();
}
