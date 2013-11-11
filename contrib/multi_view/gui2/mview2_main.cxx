// GUI main program for mview2

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

#include "mview2_menu.h"

#include <bvis/bvis_macros.h>
#include <bvis/bvis_mview_manager.h>
#include <bvis/bvis_video_manager.h>
#include <bvis/bvis_displayer_sptr.h>

#include <bpro/bpro_mview_process_manager.h>

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
#include <vidpro/process/vidpro_brightness_contrast_process.h>
#include <vidpro/process/vidpro_harris_corners_process.h>
#include <vidpro/process/vidpro_VD_edge_process.h>
#include <vidpro/process/vidpro_gaussian_blur_process.h>
#include <vidpro/process/vidpro_grey_image_process.h>
#include <bpro/bpro_storage_registry.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis_image_displayer, bvis_mview_manager );
  REG_DISPLAYER( bvis_vsol2D_displayer, bvis_mview_manager );
  REG_DISPLAYER( bvis_vtol_displayer, bvis_mview_manager );

  // Register the storage types
  REG_STORAGE( vidpro_image_storage );
  REG_STORAGE( vidpro_vsol2D_storage );
  REG_STORAGE( vidpro_vtol_storage );

  // Register all the processes
  REG_PROCESS( vidpro_load_image_process, bpro_mview_process_manager );
  REG_PROCESS( vidpro_brightness_contrast_process, bpro_mview_process_manager );
  REG_PROCESS( vidpro_VD_edge_process, bpro_mview_process_manager );
  REG_PROCESS( vidpro_harris_corners_process, bpro_mview_process_manager );
  REG_PROCESS( vidpro_gaussian_blur_process, bpro_mview_process_manager );
  REG_PROCESS( vidpro_grey_image_process, bpro_mview_process_manager );


#if defined(VCL_WIN32)
vcl_cout << '\n'<< "Max number of open files has been reset from " << _getmaxstdio();
_setmaxstdio(2048);
#endif
  
  mview2_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 600, h = 512;
  vcl_string title = "mview2 GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);

  win->get_adaptor()->set_tableau( bvis_mview_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}
