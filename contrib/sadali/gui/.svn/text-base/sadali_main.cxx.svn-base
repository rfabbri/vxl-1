// GUI main program for sadali

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "sadali_menu.h"
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
#include <vidpro1/storage/vidpro1_vsol3D_storage.h>     



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
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <gui/vidpro1_kltrack_process.h>
#include <gui/vidpro1_factorize_process.h>
#include <gui/vidpro1_new_vsol2d_tablo_process.h>
#include <gui/vidpro1_homog_view_process.h>
#include <gui/vidpro1_affine_homog_est_process.h>
#include <gui/vidpro1_combine_storage_process.h>
#include <gui/vidpro1_edgemap_form_process.h>

#include <gui/vidpro1_plane_opt_process.h>
#include <gui/vidpro1_BB_visualize_process.h>
#include <gui/vidpro1_BB_opt_test_process.h>
#include <gui/vidpro1_3D_planar_curve_reconst_process.h>
#include <gui/vidpro1_homog_curve_process.h>





// Video Codecs
#include <vidl1/vidl1_io.h>
#include <vidl1/vidl1_image_list_codec.h>
#ifdef HAS_MPEG2
#include <vidl1/vidl1_mpegcodec.h>
#endif
#ifdef VCL_WIN32
#include <vidl1/vidl1_avicodec.h>
#endif

int main(int argc, char** argv)
{
  vgui::init(argc, argv);

#ifdef HAS_BGUI3D
  bgui3d_init();
  dbvis1_so3d_init();
#endif
  // register video codecs
  vidl1_io::register_codec(new vidl1_image_list_codec);
#ifdef HAS_MPEG2
  vidl1_io::register_codec(new vidl1_mpegcodec);
#endif
#ifdef VCL_WIN32
  vidl1_io::register_codec(new vidl1_avicodec);
#endif
  
  // Register the displayer
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );

 



  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );


 




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
  REG_PROCESS( vidpro1_harris_corners_process );
  REG_PROCESS( vidpro1_kltrack_process);
  REG_PROCESS( vidpro1_factorize_process );
  REG_PROCESS( vidpro1_new_vsol2d_tablo_process);
  REG_PROCESS( vidpro1_homog_view_process);
  REG_PROCESS( vidpro1_combine_storage_process);
  REG_PROCESS( vidpro1_edgemap_form_process);
  REG_PROCESS( vidpro1_plane_opt_process);
  REG_PROCESS( vidpro1_BB_visualize_process);
  REG_PROCESS( vidpro1_BB_opt_test_process);
  REG_PROCESS( vidpro1_3D_planar_curve_reconst_process);
  REG_PROCESS( vidpro1_homog_curve_process);

  REG_PROCESS( dvidpro_affine_homog_est_process);
  

  
  sadali_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 1024, h = 768;
  vcl_string title = "sadali GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  return vgui::run();
}
