// GUI main program for dbsksp_gui

#include "en292_gui_menu.h"

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

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

#include <en292/pro/vidpro1_homog_2d_storage.h>

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

#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/process/vidpro1_load_edg_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>

#include <en292/pro/vidpro1_klt_process.h>
#include <en292/pro/vidpro1_compute_homography_2d_process.h>
#include <en292/pro/vidpro1_homog_2d_preimage_process.h>
#include <en292/pro/vidpro1_homog_2d_transform_process.h>
#include <en292/pro/vidpro1_compute_homog_2d_from_rectangle_process.h>
#include <en292/pro/vidpro1_homog_2d_transform_image_process.h>


#include <bvis1/displayer/bvis1_vtol_displayer.h>


int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( vidpro1_homog_2d_storage );

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

  REG_PROCESS( vidpro1_klt_process );
  REG_PROCESS( vidpro1_compute_homography_2d_process );
  REG_PROCESS( vidpro1_homog_2d_preimage_process );
  REG_PROCESS( vidpro1_homog_2d_transform_process );
  REG_PROCESS( vidpro1_compute_homog_2d_from_rectangle_process );
  REG_PROCESS( vidpro1_homog_2d_transform_image_process );

  
  REG_PROCESS( vidpro1_load_con_process );
  REG_PROCESS( vidpro1_load_edg_process );
  REG_PROCESS( vidpro1_load_cem_process );
  REG_PROCESS( vidpro1_save_cem_process );
  REG_PROCESS( vidpro1_save_con_process );

  en292_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "en292_gui GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}


