// GUI main program for can

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "can_menu.h"
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

// Displayer header files
#include <bvis/displayer/bvis_image_displayer.h>
#include <bvis/displayer/bvis_vsol2D_displayer.h>
#include <bvis/displayer/bvis_vtol_displayer.h>

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

#include <dbil/pro/dbil_wshed2d_process.h>


int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis_image_displayer );
  REG_DISPLAYER( bvis_vsol2D_displayer );
  REG_DISPLAYER( bvis_vtol_displayer );

  // Register the storage types
  REG_STORAGE( vidpro_image_storage );
  REG_STORAGE( vidpro_vsol2D_storage );
  REG_STORAGE( vidpro_vtol_storage );
  
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
  REG_PROCESS( dbil_wshed2d_process )

  
  can_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "can GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}
